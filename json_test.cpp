#include <gtest/gtest.h>
#include <iostream>
#include "lib/Json.h"

using namespace xuranus::jsoncpp;

struct Certificate {
  std::string name;
  int degree;
  float score;
  bool expired;
  
  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(degree, degree);
  SERIALIZE_FIELD(score, score);
  SERIALIZE_FIELD(expired, expired);
  SERIALIZE_SECTION_END
};

struct Employee {
  std::string name;
  Certificate certificate;
  std::vector<int> seq;
  std::map<std::string, int> mp;

  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, name);
  SERIALIZE_FIELD(certificate, certificate);
  SERIALIZE_FIELD(seq, seq);
  SERIALIZE_FIELD(mp, mp);
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
  util::Deserialize(R"({"certificate":{"degree":2,"name":"Java","score":114.514,"expired":true},"seq":[1,2,3], "name":"xuranus","mp":{"x":114514}})", employee2);
  std::cout << employee2.name << std::endl;
  std::cout << employee2.seq.size() << " " << employee2.seq[0] << " " << employee2.seq[1] << " " << employee2.seq[2] << std::endl;
  std::cout << employee2.certificate.name << std::endl;
  std::cout << employee2.certificate.degree << std::endl;
  std::cout << employee2.certificate.score << std::endl;
  std::cout << employee2.certificate.expired << std::endl;
  std::cout << employee2.mp["x"] << std::endl;
}

// TEST(SerializationTest, XX) {
//   std::string str = "";
//   EXPECT_EQ(JsonParser(str).Parse().Serialize(), "114514");
// }



// TEST(StringSerializationTest, StringWithEscapeChar) {
//   std::string str = R"("hello\n\r\world\t")";
//   EXPECT_EQ(JsonParser(str).Parse().Serialize(), str);
// }

