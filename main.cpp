#include <iostream>
#include "Json.h"

using namespace xuranus::jsoncpp;

struct Certificate {
  std::string name;
  long degree;

  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(degree, degree);
  SERIALIZE_SECTION_END
};

struct Employee {
  std::string name;
  Certificate certificate;

  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(certificate, certificate);
  SERIALIZE_SECTION_END
};

int main()
{
  Employee employee {"xuranus", {"Java", 2}};
  std::cout << util::Serialize(employee) << std::endl;

  Employee employee2;
  //util::Deserialize(util::Serialize(employee), employee2);
  util::Deserialize(R"({"certificate":{"degree":2,"name":"Java"},"name":"xuranus"})", employee2);
  std::cout << employee2.name << std::endl;
  std::cout << employee2.certificate.name << std::endl;
  std::cout << employee2.certificate.degree << std::endl;
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