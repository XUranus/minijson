#include <iostream>
#include "Json.h"

using namespace xuranus::jsoncpp;

#define SERIALIZE_SECTION_BEGIN \
  void _json_field_map_(xuranus::jsoncpp::JsonObject& object, bool toJson) const \
  { \

#define SERIALIZE_SECTION_END \
  }; \

#define SERIALIZE_FIELD(KEY_NAME, ATTR_NAME) \
  do { \
    if (toJson) { \
      xuranus::jsoncpp::util::SerializeTo(object, #KEY_NAME, ATTR_NAME); \
    } else { \
      xuranus::jsoncpp::util::DeserializeFrom(object, #KEY_NAME, ATTR_NAME); \
    } \
  } while (0) \


struct Certificate {
  std::string name;
  long degree;

  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(degree, degree);
  SERIALIZE_SECTION_END
};

int main()
{
    Certificate cer {"C++", 1};
    std::cout << util::Serialize(cer) << std::endl;

    Certificate cer2 {};
    util::Deserialize(util::Serialize(cer), cer2);
    std::cout << cer2.name << std::endl;
}

int main1()
{
  //std::string str1 = R"({"name":"xuranus"})";
  // std::string str1 = R"(null)";
  // std::string str1 = R"(123.44)";
  // std::string str1 = R"("helloworld")";
  // std::string str1 = R"([1,2,3,4,5])";
  // std::string str1 = R"(["one", "two", "three", "four"])";
  std::string str1 = R"({
            "array": ["one", false, 1, null],
      "name":           "xxx",
      "object":
        {
          "hello":true   
        }
    }
  )";
  std::cout << str1 << std::endl;
  //std::cout << str1[60] << str1[61] << str1[62] << std::endl;
  JsonParser parser(str1);
  JsonElement ele = parser.Parse();
  std::cout << ele.Serialize() << std::endl;
  return 0;
}