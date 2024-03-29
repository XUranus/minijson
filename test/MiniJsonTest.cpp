/*================================================================
*   Copyright (C) 2022 XUranus All rights reserved.
*   
*   File:         MiniJsonTest.cpp
*   Author:       XUranus
*   Date:         2022-11-21
*   Description:  a tiny C++ Json library
*                 https://github.com/XUranus/minicpp
*
================================================================*/

#include <gtest/gtest.h>
#include <iostream>
#include "StructSample.h"
#include "../Json.h"

using namespace xuranus::minijson;

TEST(SerializationTest, BasicString) {
    std::string jsonStr = R"("this is a basic string")";
    std::string str = "this is a basic string";
    JsonElement element = JsonParser(jsonStr).Parse();
    EXPECT_TRUE(element.IsString());
    EXPECT_EQ(element.ToString(), str);
    EXPECT_EQ(element.Serialize(), jsonStr);
    // test string content modification
    EXPECT_EQ(element.AsString(), str);
    std::string newStr = "this is a new string"; // TODO:: add operator = overload
    element.AsString() = newStr;
    EXPECT_EQ(element.ToString(), newStr);
}

TEST(SerializationTest, EscapedString) {
    std::string jsonStr = R"("this is \t a string \n with escaped \rchar")";
    std::string str = "this is \t a string \n with escaped \rchar";
    JsonElement element = JsonParser(jsonStr).Parse();
    EXPECT_TRUE(element.IsString());
    EXPECT_EQ(element.ToString(), str);
    EXPECT_EQ(element.Serialize(), jsonStr);
}

TEST(SerializationTest, NullType) {
    std::string str = "null";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "null");
    EXPECT_TRUE(element.IsNull());
    EXPECT_EQ(element.AsNull(), nullptr);
    EXPECT_EQ(element.ToNull(), nullptr);
}

TEST(SerializationTest, BooleanTrue) {
    std::string str = "true";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "true");
    EXPECT_TRUE(element.IsBool());
    EXPECT_TRUE(element.AsBool());
    EXPECT_TRUE(element.ToBool());
}

TEST(SerializationTest, BooleanFalse) {
    std::string str = "false";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "false");
    EXPECT_TRUE(element.IsBool());
    EXPECT_FALSE(element.AsBool());
    EXPECT_FALSE(element.ToBool());
}

TEST(SerializationTest, NumberInteger) {
    std::string str = "1919810";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "1919810");
    EXPECT_TRUE(element.IsLongInt());
    EXPECT_EQ(element.AsLongInt(), 1919810);
    EXPECT_EQ(element.ToLongInt(), 1919810);
}

TEST(SerializationTest, NumberBasicFloat) {
    std::string str = "114.51400";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "114.514");
    EXPECT_TRUE(element.IsDouble());
    EXPECT_EQ(element.AsDouble(), 114.514);
    EXPECT_EQ(element.ToDouble(), 114.514);
}

TEST(SerializationTest, NumberFloatScientificNotation) {
    std::string str = "-114.51E+4";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "-1145100");
    EXPECT_TRUE(element.IsDouble());
    EXPECT_EQ(element.ToDouble(), -1145100);
}

TEST(SerializationTest, LongIntNumber) {
    std::string str = "1099511627776";
    JsonElement element = JsonParser(str).Parse();
    EXPECT_EQ(element.Serialize(), "1099511627776");
    EXPECT_TRUE(element.IsLongInt());
    EXPECT_EQ(element.AsLongInt(), 1099511627776L);
}

TEST(SerializationTest, JsonElementConstructorTest) {
    JsonElement jsonNull(JsonElement::Type::JSON_NULL);
    EXPECT_TRUE(jsonNull.IsNull());
    EXPECT_EQ(jsonNull.TypeName(), "JSON_NULL");

    JsonElement jsonNumber(JsonElement::Type::JSON_NUMBER_DOUBLE);
    EXPECT_TRUE(jsonNumber.IsDouble());
    EXPECT_EQ(jsonNumber.TypeName(), "JSON_NUMBER_DOUBLE");

    JsonElement jsonBool(JsonElement::Type::JSON_BOOL);
    EXPECT_TRUE(jsonBool.IsBool());
    EXPECT_EQ(jsonBool.TypeName(), "JSON_BOOL");

    JsonElement jsonString(JsonElement::Type::JSON_STRING);
    EXPECT_TRUE(jsonString.IsString());
    EXPECT_EQ(jsonString.TypeName(), "JSON_STRING");

    JsonElement jsonArray(JsonElement::Type::JSON_ARRAY);
    EXPECT_TRUE(jsonArray.IsJsonArray());
    EXPECT_EQ(jsonArray.TypeName(), "JSON_ARRAY");

    JsonElement jsonObject(JsonElement::Type::JSON_OBJECT);
    EXPECT_TRUE(jsonObject.IsJsonObject());
    EXPECT_EQ(jsonObject.TypeName(), "JSON_OBJECT");
}

TEST(SerializationTest, JsonParserBasicTest) {
    std::string str = R"(
        {
            "name" : "xuranus",
            "age" : 300,
            "skills" : ["C++", "Java", "Python"]
        }
    )";
    JsonElement element = JsonParser(str).Parse();
    JsonObject object = element.AsJsonObject();
    EXPECT_EQ(object["name"].AsString(), "xuranus");
    EXPECT_EQ(object["age"].AsLongInt(), 300);
    EXPECT_EQ(object["skills"].AsJsonArray()[0].AsString(), "C++");
    EXPECT_EQ(object["skills"].AsJsonArray()[1].AsString(), "Java");
    EXPECT_EQ(object["skills"].AsJsonArray()[2].AsString(), "Python");
}

TEST(SerializationTest, JsonElementSerialization) {
    EXPECT_EQ(JsonElement("hello world").Serialize(), R"("hello world")");
    EXPECT_EQ(JsonElement(true).Serialize(), "true");
    EXPECT_EQ(JsonElement(114.514).Serialize(), "114.514");
    EXPECT_EQ(JsonElement().Serialize(), "null");

    JsonObject object {};
    object["name"] = JsonElement("xuranus");
    object["age"] = JsonElement(300.0); // TODO:: solve literal ambigous issue
    EXPECT_EQ(object.Serialize(), R"({"age":300,"name":"xuranus"})");
}

TEST(SerializationTest, BasicStructSerialization) {
    Book book1 {};
    book1.m_name = "C++ Primer";
    book1.m_id = 114514;
    book1.m_currentPrice = 114.5;
    book1.m_soldOut = true;
    book1.m_tags = {"C++", "Programming", "Language"};

    Book book2 {};
    std::string jsonStr = util::Serialize(book1);
    util::Deserialize(jsonStr, book2);
    EXPECT_EQ(book1.m_name, book2.m_name);
    EXPECT_EQ(book1.m_id, book2.m_id);
    EXPECT_EQ(book1.m_currentPrice, book2.m_currentPrice);
    EXPECT_EQ(book1.m_soldOut, book2.m_soldOut);
    EXPECT_EQ(book1.m_tags, book2.m_tags);
}

TEST(SerializationTest, NestedStructSerialization) {
    Book book1 {};
    book1.m_name = "C++ Primer";
    book1.m_id = 114514;
    book1.m_currentPrice = 11.4;
    book1.m_soldOut = true;
    book1.m_tags = { "C++", "Programming", "Language" };
    book1.m_pageWithPic = { { 11, 45}, { 141, 9810} };

    Book book2 {};
    book2.m_name = "Essential C++";
    book2.m_id = 1919810;
    book2.m_currentPrice = 19.19;
    book2.m_soldOut = false;
    book2.m_tags = { "Programming", "Computer Science" };

    Author author1 {};
    author1.m_name = "Stanley B. LippmanBarbara E. Moo JoséeLaJoie";
    author1.m_books = { book1, book2 };

    Author author2 {};
    std::string jsonStr = util::Serialize(author1);
    util::Deserialize(jsonStr, author2);

    EXPECT_EQ(author2.m_name, author1.m_name);

    JsonElement ele = JsonParser(jsonStr).Parse();
    EXPECT_TRUE(ele.IsJsonObject());
    JsonObject authorObject = ele.AsJsonObject();
    EXPECT_TRUE(authorObject["name"].IsString());
    EXPECT_TRUE(authorObject["books"].IsJsonArray());
    JsonArray booksArray = authorObject["books"].AsJsonArray();
    EXPECT_EQ(booksArray[0].AsJsonObject()["name"].AsString(), "C++ Primer"); // TODO:: complete it
}


