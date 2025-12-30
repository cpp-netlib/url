// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <skyr/url.hpp>

namespace {

constexpr auto WPT_URL =
    "https://raw.githubusercontent.com/web-platform-tests/wpt/master/url/resources/urltestdata.json";
constexpr auto CACHE_FILE = "urltestdata.json";
constexpr auto CACHE_MAX_AGE_HOURS = 24;

struct TestCase {
  std::string input;
  std::string base;
  bool should_fail{false};
  std::string expected_href;
  std::string expected_protocol;
  std::string expected_username;
  std::string expected_password;
  std::string expected_host;
  std::string expected_hostname;
  std::string expected_port;
  std::string expected_pathname;
  std::string expected_search;
  std::string expected_hash;
  std::string comment;
};

struct TestResult {
  enum class Status { Success, ParseFailure, ComponentMismatch };

  Status status;
  TestCase test_case;
  std::string actual_href;
  std::string mismatch_details;
};

struct Statistics {
  size_t total{0};
  size_t successful{0};
  size_t parse_failures{0};
  size_t component_mismatches{0};
  std::vector<TestResult> failures;
};

bool is_cache_valid(const std::filesystem::path& cache_path) {
  if (!std::filesystem::exists(cache_path)) {
    return false;
  }

  auto last_write = std::filesystem::last_write_time(cache_path);
  auto now = std::filesystem::file_time_type::clock::now();
  auto age = std::chrono::duration_cast<std::chrono::hours>(now - last_write);

  return age.count() < CACHE_MAX_AGE_HOURS;
}

bool download_test_data(const std::filesystem::path& output_path) {
  std::cout << "Downloading latest WPT test data from:\n  " << WPT_URL << "\n";

  auto curl_command = std::format("curl -L -s -o \"{}\" \"{}\"", output_path.string(), WPT_URL);

  auto result = std::system(curl_command.c_str());
  if (result != 0) {
    std::cerr << "Error: Failed to download test data (curl exit code: " << result << ")\n";
    return false;
  }

  if (!std::filesystem::exists(output_path) || std::filesystem::file_size(output_path) == 0) {
    std::cerr << "Error: Downloaded file is missing or empty\n";
    return false;
  }

  std::cout << "Successfully downloaded test data (" << std::filesystem::file_size(output_path) << " bytes)\n\n";
  return true;
}

std::vector<TestCase> load_test_cases(const std::filesystem::path& json_path) {
  std::ifstream file(json_path);
  if (!file) {
    throw std::runtime_error(std::format("Failed to open file: {}", json_path.string()));
  }

  nlohmann::json json_data;
  file >> json_data;

  std::vector<TestCase> test_cases;

  for (const auto& item : json_data) {
    // Skip comments and metadata
    if (!item.is_object()) {
      continue;
    }

    if (item.contains("comment") && !item.contains("input")) {
      // This is a section comment, skip it
      continue;
    }

    TestCase test_case;

    // Helper to safely get string values (handles null)
    auto get_string = [&](const std::string& key) -> std::string {
      if (item.contains(key) && item[key].is_string()) {
        return item[key].get<std::string>();
      }
      return "";
    };

    test_case.input = get_string("input");
    test_case.base = get_string("base");

    // Check if this test should fail
    if (item.contains("failure")) {
      test_case.should_fail = item["failure"].get<bool>();
    }

    // Load expected results (if not a failure case)
    if (!test_case.should_fail) {
      test_case.expected_href = get_string("href");
      test_case.expected_protocol = get_string("protocol");
      test_case.expected_username = get_string("username");
      test_case.expected_password = get_string("password");
      test_case.expected_host = get_string("host");
      test_case.expected_hostname = get_string("hostname");
      test_case.expected_port = get_string("port");
      test_case.expected_pathname = get_string("pathname");
      test_case.expected_search = get_string("search");
      test_case.expected_hash = get_string("hash");
    }

    test_case.comment = get_string("comment");

    test_cases.push_back(std::move(test_case));
  }

  return test_cases;
}

TestResult run_test_case(const TestCase& test_case) {
  TestResult result;
  result.test_case = test_case;

  // Try to parse the URL (may throw on error)
  try {
    skyr::url url;

    if (test_case.base.empty()) {
      url = skyr::url(test_case.input);
    } else {
      auto base_url = skyr::url(test_case.base);
      url = skyr::url(test_case.input, base_url);
    }

    // Parse succeeded
    if (test_case.should_fail) {
      // Parser is more permissive than WPT - report this
      result.status = TestResult::Status::ComponentMismatch;
      result.actual_href = url.href();
      result.mismatch_details = "Parse succeeded (expected failure)";
      return result;
    }

    // Check components
    result.actual_href = url.href();

    std::vector<std::string> mismatches;

    if (!test_case.expected_href.empty() && url.href() != test_case.expected_href) {
      mismatches.push_back(std::format("href: expected '{}', got '{}'", test_case.expected_href, url.href()));
    }

    if (!test_case.expected_protocol.empty() && url.protocol() != test_case.expected_protocol) {
      mismatches.push_back(
          std::format("protocol: expected '{}', got '{}'", test_case.expected_protocol, url.protocol()));
    }

    if (!test_case.expected_username.empty() && url.username() != test_case.expected_username) {
      mismatches.push_back(
          std::format("username: expected '{}', got '{}'", test_case.expected_username, url.username()));
    }

    if (!test_case.expected_password.empty() && url.password() != test_case.expected_password) {
      mismatches.push_back(
          std::format("password: expected '{}', got '{}'", test_case.expected_password, url.password()));
    }

    if (!test_case.expected_host.empty() && url.host() != test_case.expected_host) {
      mismatches.push_back(std::format("host: expected '{}', got '{}'", test_case.expected_host, url.host()));
    }

    if (!test_case.expected_hostname.empty() && url.hostname() != test_case.expected_hostname) {
      mismatches.push_back(
          std::format("hostname: expected '{}', got '{}'", test_case.expected_hostname, url.hostname()));
    }

    if (!test_case.expected_port.empty() && url.port() != test_case.expected_port) {
      mismatches.push_back(std::format("port: expected '{}', got '{}'", test_case.expected_port, url.port()));
    }

    if (!test_case.expected_pathname.empty() && url.pathname() != test_case.expected_pathname) {
      mismatches.push_back(
          std::format("pathname: expected '{}', got '{}'", test_case.expected_pathname, url.pathname()));
    }

    if (!test_case.expected_search.empty() && url.search() != test_case.expected_search) {
      mismatches.push_back(std::format("search: expected '{}', got '{}'", test_case.expected_search, url.search()));
    }

    if (!test_case.expected_hash.empty() && url.hash() != test_case.expected_hash) {
      mismatches.push_back(std::format("hash: expected '{}', got '{}'", test_case.expected_hash, url.hash()));
    }

    if (mismatches.empty()) {
      result.status = TestResult::Status::Success;
    } else {
      result.status = TestResult::Status::ComponentMismatch;
      // Manually join mismatches
      for (size_t i = 0; i < mismatches.size(); ++i) {
        if (i > 0)
          result.mismatch_details += "; ";
        result.mismatch_details += mismatches[i];
      }
    }

    return result;

  } catch (...) {
    // Parse failed with exception
    if (test_case.should_fail) {
      result.status = TestResult::Status::Success;
    } else {
      result.status = TestResult::Status::ParseFailure;
      result.mismatch_details = "Parse threw exception";
    }
    return result;
  }
}

void print_report(const Statistics& stats) {
  auto total = stats.total;
  auto successful = stats.successful;
  auto parse_failures = stats.parse_failures;
  auto component_mismatches = stats.component_mismatches;
  auto failed = parse_failures + component_mismatches;

  auto success_pct = total > 0 ? (successful * 100.0 / total) : 0.0;
  auto failure_pct = total > 0 ? (failed * 100.0 / total) : 0.0;

  std::cout << "=================================================\n";
  std::cout << "WPT URL Test Suite Report\n";
  std::cout << "=================================================\n";
  std::cout << "Test Data: " << WPT_URL << "\n";

  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  std::cout << std::format("Generated: {:%Y-%m-%d %H:%M:%S} UTC\n\n", std::chrono::system_clock::from_time_t(time));

  std::cout << "SUMMARY:\n";
  std::cout << std::format("  Total Tests:      {:>6}\n", total);
  std::cout << std::format("  Successful:       {:>6} ({:.1f}%)\n", successful, success_pct);
  std::cout << std::format("  Failed:           {:>6} ({:.1f}%)\n\n", failed, failure_pct);

  if (failed > 0) {
    auto parse_fail_pct = failed > 0 ? (parse_failures * 100.0 / failed) : 0.0;
    auto mismatch_pct = failed > 0 ? (component_mismatches * 100.0 / failed) : 0.0;

    std::cout << "FAILURE BREAKDOWN:\n";
    std::cout << std::format("  Parse Errors:     {:>6} ({:.1f}% of failures)\n", parse_failures, parse_fail_pct);
    std::cout << std::format("  Component Mismatch:{:>6} ({:.1f}% of failures)\n\n", component_mismatches,
                             mismatch_pct);

    // Show sample failures
    constexpr size_t MAX_SAMPLES = 20;
    auto sample_count = std::min(stats.failures.size(), MAX_SAMPLES);

    std::cout << std::format("SAMPLE FAILURES (first {}):\n", sample_count);

    for (size_t i = 0; i < sample_count; ++i) {
      const auto& failure = stats.failures[i];
      std::cout << std::format("  [{}] Input: \"{}\"\n", i + 1, failure.test_case.input);

      if (!failure.test_case.base.empty()) {
        std::cout << std::format("      Base: \"{}\"\n", failure.test_case.base);
      }

      if (!failure.test_case.comment.empty()) {
        std::cout << std::format("      Comment: {}\n", failure.test_case.comment);
      }

      if (failure.status == TestResult::Status::ParseFailure) {
        std::cout << std::format("      Expected: parsed successfully\n");
        std::cout << std::format("      Actual: parse failure\n");
      } else {
        std::cout << std::format("      Details: {}\n", failure.mismatch_details);
      }

      std::cout << "\n";
    }

    if (stats.failures.size() > MAX_SAMPLES) {
      std::cout << std::format("  ... and {} more failures\n\n", stats.failures.size() - MAX_SAMPLES);
    }
  }

  std::cout << "=================================================\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    std::cout << "WPT URL Integration Test Runner\n\n";

    // Determine cache path (in current directory or specified location)
    std::filesystem::path cache_path = CACHE_FILE;
    bool force_download = false;

    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--force-download" || arg == "-f") {
        force_download = true;
      } else if (arg == "--cache" || arg == "-c") {
        if (i + 1 < argc) {
          cache_path = argv[++i];
        }
      }
    }

    // Download or use cached data
    if (force_download || !is_cache_valid(cache_path)) {
      if (!download_test_data(cache_path)) {
        return 1;
      }
    } else {
      std::cout << "Using cached test data: " << cache_path << "\n";
      std::cout << "(Use --force-download to fetch latest)\n\n";
    }

    // Load test cases
    std::cout << "Loading test cases...\n";
    auto test_cases = load_test_cases(cache_path);
    std::cout << std::format("Loaded {} test cases\n\n", test_cases.size());

    // Run tests sequentially
    std::cout << "Running tests...\n";
    Statistics stats;

    for (const auto& test_case : test_cases) {
      stats.total++;
      auto result = run_test_case(test_case);

      switch (result.status) {
        case TestResult::Status::Success:
          stats.successful++;
          break;
        case TestResult::Status::ParseFailure:
          stats.parse_failures++;
          // Only report unexpected failures
          stats.failures.push_back(result);
          break;
        case TestResult::Status::ComponentMismatch:
          stats.component_mismatches++;
          // Only report mismatches (parser didn't behave as expected)
          stats.failures.push_back(result);
          break;
      }
    }

    std::cout << "Tests complete!\n\n";

    // Print report
    print_report(stats);

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}