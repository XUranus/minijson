#include <gtest/gtest.h>
#include <iostream>
#include "lib/Json.h"

using namespace xuranus::jsoncpp;

struct Certificate {
  std::string name;
  int degree;
  float score;
  bool expired;
  std::vector<int> seq;
  
  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(degree, degree);
  SERIALIZE_FIELD(score, score);
  SERIALIZE_FIELD(expired, expired);
  SERIALIZE_FIELD(seq, seq);
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

TEST(SerializationTest, BasicString) {
  std::string str = R"("this is a basic string")";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), str);
}

TEST(SerializationTest, BasicBoolean) {
  std::string str = "\n  true \t\r";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), "true");

  str = "\n\t  false \t\r";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), "false");
}

TEST(SerializationTest, BasicNumber) {
  std::string str = " 114514 ";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), "114514");

  str = "1919.810 ";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), "1919.81");
}

TEST(SerializationTest, BasicNumberFloat) {
  std::string str = "114.51E4";
  EXPECT_EQ(JsonParser(str).Parse().Serialize(), "1145100");
}

TEST(SerializationTest, SerializeStruct) {
  Employee employee {"xuranus", {"Java", 2}};
  std::cout << util::Serialize(employee) << std::endl;
}


TEST(SerializationTest, DeserializeToStruct) {
  Employee employee2;
  util::Deserialize(R"({"certificate":{"degree":2,"name":"Java","score":114.514,"expired":true, "seq":[]},"name":"xuranus"})", employee2);
  std::cout << employee2.name << std::endl;
  std::cout << employee2.certificate.name << std::endl;
  std::cout << employee2.certificate.degree << std::endl;
  std::cout << employee2.certificate.score << std::endl;
  std::cout << employee2.certificate.expired << std::endl;
}

// TEST(SerializationTest, XX) {
//   std::string str = "";
//   EXPECT_EQ(JsonParser(str).Parse().Serialize(), "114514");
// }



// TEST(StringSerializationTest, StringWithEscapeChar) {
//   std::string str = R"("hello\n\r\world\t")";
//   EXPECT_EQ(JsonParser(str).Parse().Serialize(), str);
// }

