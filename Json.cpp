#include "Json.h"
#include <cstdio>

using namespace xuranus::jsoncpp;
using namespace xuranus::jsoncpp::util;

JsonElement::JsonElement()
{
  m_type = JsonElement::Type::JSON_NULL;
}

JsonElement::JsonElement(JsonElement::Type type)
{
  m_type = type;
  switch (type) {
    case JsonElement::Type::JSON_OBJECT: {
      m_value.objectValue = new JsonObject();
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      m_value.arrayValue = new JsonArray();
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      m_value.stringValue = new std::string();
      break;
    }
    case JsonElement::Type::JSON_NUMBER: {
      m_value.numberValue = 0;
      break;
    }
    case JsonElement::Type::JSON_BOOL: {
      m_value.boolValue = false;
      break;
    }
    case JsonElement::Type::JSON_NULL: {
      break;
    }
  }
}

JsonElement::JsonElement(bool value): m_type(JsonElement::Type::JSON_BOOL)
{
  m_value.boolValue = value;
}

JsonElement::JsonElement(double num): m_type(JsonElement::Type::JSON_NUMBER)
{
  m_value.numberValue = num;
}

JsonElement::JsonElement(long num): m_type(JsonElement::Type::JSON_NUMBER)
{
  m_value.numberValue = (double)num;
}

JsonElement::JsonElement(const std::string &str): m_type(JsonElement::Type::JSON_STRING)
{
  m_value.stringValue = new std::string(str);
}

JsonElement::JsonElement(const JsonObject& object): m_type(JsonElement::Type::JSON_OBJECT)
{
  m_value.objectValue = new JsonObject(object);
}

JsonElement::JsonElement(const JsonArray& array): m_type(JsonElement::Type::JSON_ARRAY)
{
  m_value.arrayValue = new JsonArray(array);;
}



JsonElement::JsonElement(const JsonElement& ele): m_type(ele.m_type)
{
  switch (ele.m_type) {
    case JsonElement::Type::JSON_OBJECT: {
      m_value.objectValue = new JsonObject(*ele.m_value.objectValue);
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      m_value.arrayValue = new JsonArray(*ele.m_value.arrayValue);
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      m_value.stringValue = new std::string(*ele.m_value.stringValue);
      break;
    }
    case JsonElement::Type::JSON_NUMBER: {
      m_value.numberValue = ele.m_value.numberValue;
      break;
    }
    case JsonElement::Type::JSON_BOOL: {
      m_value.boolValue = ele.m_value.boolValue;
      break;
    }
    case JsonElement::Type::JSON_NULL: {
      break;
    }
  }
}

JsonElement::JsonElement(JsonElement&& ele): m_type(ele.m_type)
{
  switch (ele.m_type) {
    case JsonElement::Type::JSON_OBJECT: {
      m_value.objectValue = ele.m_value.objectValue;
      ele.m_value.objectValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      m_value.arrayValue = ele.m_value.arrayValue;
      ele.m_value.arrayValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      m_value.stringValue = ele.m_value.stringValue;
      ele.m_value.stringValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_NUMBER: {
      m_value.numberValue = ele.m_value.numberValue;
      break;
    }
    case JsonElement::Type::JSON_BOOL: {
      m_value.boolValue = ele.m_value.boolValue;
      break;
    }
    case JsonElement::Type::JSON_NULL: {
      break;
    }
  }
}

JsonElement& JsonElement::operator = (const JsonElement& ele)
{
  if (this == &ele) {
    return *this;
  }
  switch (m_type) {
    case JsonElement::Type::JSON_OBJECT: {
      delete m_value.objectValue;
      m_value.objectValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      delete m_value.arrayValue;
      m_value.arrayValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      delete m_value.stringValue;
      m_value.stringValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_NUMBER:
    case JsonElement::Type::JSON_BOOL:
    case JsonElement::Type::JSON_NULL:
      break;
  }
  m_type = ele.m_type;
  switch (ele.m_type) {
    case JsonElement::Type::JSON_OBJECT: {
      m_value.objectValue = new JsonObject(*ele.m_value.objectValue);
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      m_value.arrayValue = new JsonArray(*ele.m_value.arrayValue);
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      m_value.stringValue = new std::string(*ele.m_value.stringValue);
      break;
    }
    case JsonElement::Type::JSON_NUMBER: {
      m_value.numberValue = ele.m_value.numberValue;
      break;
    }
    case JsonElement::Type::JSON_BOOL: {
      m_value.boolValue = ele.m_value.boolValue;
      break;
    }
    case JsonElement::Type::JSON_NULL: {
      break;
    }
  }
  return *this;
}


JsonElement::~JsonElement()
{
  switch (m_type) {
    case JsonElement::Type::JSON_OBJECT: {
      delete m_value.objectValue;
      m_value.objectValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_ARRAY: {
      delete m_value.arrayValue;
      m_value.arrayValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_STRING: {
      delete m_value.stringValue;
      m_value.stringValue = nullptr;
      break;
    }
    case JsonElement::Type::JSON_NUMBER:
    case JsonElement::Type::JSON_BOOL:
    case JsonElement::Type::JSON_NULL:
      break;
  }
}

bool& JsonElement::AsBool()
{
  if (m_type != JsonElement::Type::JSON_BOOL) {
    Panic("failed to convert json element as a bool");
  }
  return m_value.boolValue;
}

double& JsonElement::AsNumber()
{
  if (m_type != JsonElement::Type::JSON_NUMBER) {
    Panic("failed to convert json element as a number");
  }
  return m_value.numberValue;
}

void* JsonElement::AsNull()
{
  if (m_type != JsonElement::Type::JSON_NULL) {
    Panic("failed to convert json element as a null");
  }
  return nullptr;
}

std::string& JsonElement::AsString()
{
  if (m_type != JsonElement::Type::JSON_STRING) {
    Panic("failed to convert json element as a string");
  }
  return *(m_value.stringValue);
}

JsonObject& JsonElement::AsJsonObject()
{
  if (m_type != JsonElement::Type::JSON_OBJECT) {
    Panic("failed to convert json element as an object");
  }
  return *(m_value.objectValue);
}

JsonArray& JsonElement::AsJsonArray()
{
  if (m_type != JsonElement::Type::JSON_ARRAY) {
    Panic("failed to convert json element as an array");
  }
  return *(m_value.arrayValue);
}


bool JsonElement::IsNull() const { return m_type == JsonElement::Type::JSON_NULL; }
bool JsonElement::IsBool() const { return m_type == JsonElement::Type::JSON_BOOL; }
bool JsonElement::IsNumber() const { return m_type == JsonElement::Type::JSON_NUMBER; }
bool JsonElement::IsString() const { return m_type == JsonElement::Type::JSON_STRING; }
bool JsonElement::IsJsonObject() const { return m_type == JsonElement::Type::JSON_OBJECT; }
bool JsonElement::IsJsonArray() const { return m_type == JsonElement::Type::JSON_ARRAY; }

std::string JsonElement::TypeName() const
{
  switch (m_type) {
    case JsonElement::Type::JSON_OBJECT:
      return "JSON_OBJECT";
    case JsonElement::Type::JSON_ARRAY:
      return "JSON_ARRAY";
    case JsonElement::Type::JSON_STRING:
      return "JSON_STRING";
    case JsonElement::Type::JSON_NUMBER:
      return "JSON_NUMBER";
    case JsonElement::Type::JSON_BOOL:
      return "JSON_BOOL";
    case JsonElement::Type::JSON_NULL:
      return "JSON_NULL";
  }
  Panic("invalid type");
  return "";
}

template<> void JsonElement::Cast<long>(long& value) const {
  if (!IsNumber()) {
    Panic("illegal cast from %s to long", TypeName().c_str());
  }
  value = static_cast<long>(m_value.numberValue);
  return;
}

template<> void JsonElement::Cast<std::string>(std::string& value) const {
  if (!IsString()) {
    Panic("illegal cast from %s to string", TypeName().c_str());
  }
  value = *(m_value.stringValue);
  return;
}

std::string JsonElement::Serialize() const
{
  switch (m_type) {
    case JsonElement::Type::JSON_NULL: {
      return "null";
    }
    case JsonElement::Type::JSON_BOOL: {
      return m_value.boolValue ? "true" : "false";
    }
    case JsonElement::Type::JSON_NUMBER: {
      // TODO:: extract
      std::string res = std::to_string(m_value.numberValue);
      if (res.find('.') != std::string::npos) {
        while(res.back() == '0') {
          res.pop_back();
        }
      }
      if (res.back() == '.') {
        res.pop_back();
      }
      return res;
    }
    case JsonElement::Type::JSON_STRING: {
      return std::string("\"") + EscapeString(*m_value.stringValue) + "\"";
    }
    case JsonElement::Type::JSON_OBJECT: {
      return JsonObject(*m_value.objectValue).Serialize();
    }
    case JsonElement::Type::JSON_ARRAY: {
      return JsonArray(*m_value.arrayValue).Serialize();
    }
  }
  Panic("unknown type to serialize");
  return "";
}

std::string JsonObject::Serialize() const
{
  std::string res = "{";
  bool moreThanOneItem = false;
  for (const auto& kv: *this) {
    moreThanOneItem = true;
    res += std::string("\"") + EscapeString(kv.first) + "\":" + kv.second.Serialize() + ",";
  }
  if (moreThanOneItem) {
    res.pop_back();
  }
  res += "}";
  return res;
}

std::string JsonArray::Serialize() const
{
  std::string res = "[";
  for (const auto& v: *this) {
    res += v.Serialize() + ",";
  }
  res.pop_back();
  res += "]";
  return res;
}


JsonScanner::JsonScanner(const std::string &str): m_str(str), m_pos(0), m_prevPos(0)
{}

// return a non space token
JsonScanner::Token JsonScanner::Next()
{
  m_prevPos = m_pos;
  if (m_str.length() <= m_pos || !SkipWhitespaceToken()) {
    return Token::EOF_TOKEN;
  }

  char curChar = m_str[m_pos];
  if (IsDigit(curChar) || curChar == '-') {
    ScanNextNumber();
    return Token::NUMBER;
  }
  switch (curChar) {
    case '\"':
      ScanNextString();
      return Token::STRING;
    case 't':
      ScanLiteral("true", 4);
      return Token::LITERAL_TRUE;
    case 'f':
      ScanLiteral("false", 5);
      return Token::LITERAL_FALSE;
    case 'n':
      ScanLiteral("null", 4);
      return Token::LITERAL_NULL;
    case '[':
      m_pos ++;
      return Token::ARRAY_BEGIN;
    case ']':
      m_pos ++;
      return Token::ARRAY_END;
    case '{':
      m_pos ++;
      return Token::OBJECT_BEGIN;
    case '}':
      m_pos ++;
      return Token::OBJECT_END;
    case ',':
      m_pos ++;
      return Token::COMMA;
    case ':':
      m_pos ++;
      return Token::COLON;
  }
  Panic("Invalid token at position %u", m_pos);
  return Token::LITERAL_NULL;
}


void JsonScanner::ScanNextString()
{
  size_t beginPos = m_pos;
  m_pos ++; // skip left "
  while (m_pos < m_str.size() && m_str[m_pos] != '\"') {
    char curChar = m_str[m_pos ++];
    if (curChar == '\\') {
      // " quotation mark
      // \ reverse soildus
      // / sodilus
      // b backspace
      // f formfeed
      // n linefeed
      // r carriage return
      // t horizontal tab
      // u (4 hex digits)
      if (m_pos >= m_str.size()) {
        Panic("missing token, position: %s", m_pos);
        return;
      } else {
        char escapeChar = m_str[m_pos];
        if (escapeChar == '\"' || escapeChar == 'r' || escapeChar == 'f' || escapeChar == 'n' ||
          escapeChar == 't' || escapeChar == 'b' || escapeChar == '\\' || escapeChar == '/') {
          // TODO:: / sodilus
          m_pos ++;
        } else if (escapeChar == 'u') {
          m_pos += 4;
        }
      }
    }
  }
  if (m_pos >= m_str.size()) {
    Panic("missing end of string, position: %u", beginPos);
  }
  m_pos ++; // skip right "
  m_tmpStrValue = m_str.substr(beginPos + 1, m_pos - beginPos - 2);
}

void JsonScanner::ScanNextNumber()
{
  size_t beginPos = m_pos;
  m_pos ++; // skip + or -
  while (m_pos < m_str.size() && IsDigit(m_str[m_pos])) {
    m_pos ++;
  }
  // TODO::114E514
  if (m_pos + 1 < m_str.size() && m_str[m_pos] == '.' && IsDigit(m_str[m_pos + 1])) {
    m_pos ++;
    while(m_pos < m_str.size() && IsDigit(m_str[m_pos])) {
      m_pos ++;
    }
  }
  std::string numberStr = m_str.substr(beginPos, m_pos - beginPos);
  try {
    m_tmpNumberValue = std::atof(numberStr.c_str());
  } catch (std::exception &e) {
    Panic("invalid number %lf, pos: %u", m_tmpNumberValue, beginPos);
  }
}

double JsonScanner::GetNumberValue() { return m_tmpNumberValue; }

std::string JsonScanner::GetStringValue() { return m_tmpStrValue; }






JsonParser::JsonParser(const std::string str): m_scanner(str)
{}

JsonElement JsonParser::Parse()
{
  JsonScanner::Token token = m_scanner.Next();
  switch (token) {
    case JsonScanner::Token::OBJECT_BEGIN: {
      return ParseJsonObject();
    }
    case JsonScanner::Token::ARRAY_BEGIN: {
      return ParseJsonArray();
    }
    case JsonScanner::Token::STRING: {
      return JsonElement(m_scanner.GetStringValue());
    }
    case JsonScanner::Token::NUMBER: {
      return JsonElement(m_scanner.GetNumberValue());
    }
    case JsonScanner::Token::LITERAL_TRUE: {
      return JsonElement(true);
    }
    case JsonScanner::Token::LITERAL_FALSE: {
      return JsonElement(false);
    }
    case JsonScanner::Token::LITERAL_NULL: {
      return JsonElement();
    }
  }
  Panic("scanner return unexpected token: %u", token);
  return JsonElement();
}

JsonObject JsonParser::ParseJsonObject()
{
  JsonObject object {};
  JsonScanner::Token token = m_scanner.Next();
  if (token == JsonScanner::Token::OBJECT_END) {
    return object;
  }
  m_scanner.RollBack();

  while (true) {
    size_t pos = m_scanner.Position();
    token = m_scanner.Next();
    if (token != JsonScanner::Token::STRING) {
      Panic("expect a string as key for json object, position: %u", pos);
    }
    std::string key = m_scanner.GetStringValue();

    pos = m_scanner.Position();
    token = m_scanner.Next();
    if (token != JsonScanner::Token::COLON) {
      Panic("expect ':' in json object, position: %u", pos);
    }
    JsonElement ele = Parse();
    object[key] = ele;

    pos = m_scanner.Position();
    token = m_scanner.Next();
    if (token == JsonScanner::Token::OBJECT_END) {
      break;
    }
    if (token != JsonScanner::Token::COMMA) {
      Panic("expect ',' in json object, position: %u", pos);
    }
  }
  return object;
}

JsonArray JsonParser::ParseJsonArray()
{
  JsonArray array {};
  JsonScanner::Token token = m_scanner.Next();
  if (token == JsonScanner::Token::ARRAY_END) {
    return array;
  }
  m_scanner.RollBack();

  while (true) {
    array.push_back(Parse());
    size_t pos = m_scanner.Position();
    token = m_scanner.Next();
    if (token == JsonScanner::Token::ARRAY_END) {
      break;
    }
    if (token != JsonScanner::Token::COMMA) {
      Panic("expect ',' in array, pos: %u", pos);
    }
  }
  return array;
}




std::string util::EscapeString(const std::string& str)
{
  std::string res;
  for (const char ch: str) {
    switch (ch) {
      case '"':
      case '\\':
      case '/':
        res.push_back('\\');
        res.push_back(ch);
      case '\f': {
        res.push_back('\\');
        res.push_back('f');
        break;
      }
      case '\b': {
        res.push_back('\\');
        res.push_back('b');
        break;
      }
      case '\r': {
        res.push_back('\\');
        res.push_back('r');
        break;
      }
      case '\n': {
        res.push_back('\\');
        res.push_back('n');
        break;
      }
      case '\t': {
        res.push_back('\\');
        res.push_back('t');
        break;
      }
      default: {
        res.push_back(ch);
        break;
      }
    }
  }
  return res;
}