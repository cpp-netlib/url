JSON
====

Description
-----------

The functions below convert between a URL query string and a JSON
object. They depend on the
`nlohmann-json <https://github.com/nlohmann/json>`_ library.

This functionality is **optional** and only available when nlohmann-json
is installed.

Headers
-------

.. code-block:: c++

    #include <skyr/json.hpp>

Example
-------

.. code-block:: c++

    #include <skyr/json.hpp>
    #include <nlohmann/json.hpp>
    #include <print>

    int main() {
      // Convert JSON object to URL query string
      auto data = nlohmann::json{
          {"name", "John Doe"},
          {"age", "30"},
          {"tags", nlohmann::json::array({"developer", "c++"})}};

      auto query = skyr::json::encode_query(data);
      if (query) {
        std::println("Query string: {}", query.value());
        // Output: name=John%20Doe&age=30&tags=developer&tags=c%2B%2B
      }

      // Convert URL query string to JSON object
      auto json_obj = skyr::json::decode_query("name=John%20Doe&age=30&tags=developer&tags=c%2B%2B");
      std::println("JSON object: {}", json_obj.dump(2));
      // Output:
      // {
      //   "age": "30",
      //   "name": "John Doe",
      //   "tags": ["developer", "c++"]
      // }
    }

API
---

JSON Functions
^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::json::encode_query

.. doxygenfunction:: skyr::json::decode_query

Error Codes
^^^^^^^^^^^

.. doxygenenum:: skyr::json::json_errc
