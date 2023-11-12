/*================================================================
*   Copyright (C) 2022 XUranus All rights reserved.
*   
*   File:         Json.cpp
*   Author:       XUranus
*   Date:         2022-11-21
*   Description:  a tiny C++ Json library
*                 https://github.com/XUranus/minicpp
*
================================================================*/

#include "Json.h"
#include <cstdio>

// to prevent header corruption
namespace xuranus {
namespace minijson {

// to split json string into token
class JsonScanner {
    public:
        enum class Token {
            WHITESPACE, // ' ', '\n', '\r', '\t'
            NUMBER,
            STRING,
            LITERAL_TRUE, // true
            LITERAL_FALSE, // false
            LITERAL_NULL, // null
            COMMA, // ,
            COLON, // :
            ARRAY_BEGIN, // [
            ARRAY_END, // ]
            OBJECT_BEGIN, // {
            OBJECT_END, // }
            EOF_TOKEN // mark the end of the json string
        };

    public:
        JsonScanner(const std::string &str);
        void Reset();
        Token Next();
        double GetDoubleValue() const;
        int64_t GetLongIntValue() const;
        bool IsNumberLongInt() const;
        std::string GetStringValue() const;
        inline void RollBack() { m_pos = m_prevPos; }
        inline size_t Position() { return m_pos; }
        static std::string TokenName(Token token);

    private:  
        void ScanNextString();
        void ScanNextNumber();

        inline bool IsWhiltespaceToken(char ch)
        {
            return (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
        }

        inline bool IsDigit(char ch)
        {
            return '0' <= ch && ch <= '9';
        }
        
        inline bool SkipWhitespaceToken()
        {
            while(m_pos < m_str.size() && IsWhiltespaceToken(m_str[m_pos])) {
                m_pos++;
            }
            return m_pos < m_str.size();
        }

        inline void ScanLiteral(const std::string& literal, int offset)
        {
            if (m_str.compare(m_pos, offset, literal) == 0) {
                m_pos += offset;
            } else {
                Panic("unknown literal token at position = %lu, do you mean: %s ?", m_pos, literal.c_str());
            }
        }
    private:
        std::string m_str;
        std::size_t m_pos = 0;
        std::size_t m_prevPos = 0;

        std::string m_tmpStrValue {};
        double m_tmpNumberDoubleValue {0};
        int64_t m_tmpNumberLongValue {0};
        bool m_int64Number { true };
        std::map<char, char> m_escapeMap {};
};

namespace util {
    std::string EscapeString(const std::string& str);
    std::string UnescapeString(const std::string& str);
    std::string DoubleToString(double value);
    std::string LongIntToString(int64_t value);
}

}
}

using namespace xuranus::minijson;
using namespace xuranus::minijson::util;

// implement start from here

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
        case JsonElement::Type::JSON_NUMBER_LONG: {
            m_value.numberLongValue = 0;
            break;
        }
        case JsonElement::Type::JSON_NUMBER_DOUBLE: {
            m_value.numberDoubleValue = 0;
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

JsonElement::JsonElement(double num): m_type(JsonElement::Type::JSON_NUMBER_DOUBLE)
{
    m_value.numberDoubleValue = num;
}

JsonElement::JsonElement(int64_t num): m_type(JsonElement::Type::JSON_NUMBER_LONG)
{
    m_value.numberLongValue = num;
}

JsonElement::JsonElement(const std::string &str): m_type(JsonElement::Type::JSON_STRING)
{
    m_value.stringValue = new std::string(str);
}

JsonElement::JsonElement(char const *str): m_type(JsonElement::Type::JSON_STRING)
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
        case JsonElement::Type::JSON_NUMBER_LONG: {
            m_value.numberLongValue = ele.m_value.numberLongValue;
            break;
        }
        case JsonElement::Type::JSON_NUMBER_DOUBLE: {
            m_value.numberDoubleValue = ele.m_value.numberDoubleValue;
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
        case JsonElement::Type::JSON_NUMBER_LONG: {
            m_value.numberLongValue = ele.m_value.numberLongValue;
            break;
        }
        case JsonElement::Type::JSON_NUMBER_DOUBLE: {
            m_value.numberDoubleValue = ele.m_value.numberDoubleValue;
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
        case JsonElement::Type::JSON_NUMBER_DOUBLE:
        case JsonElement::Type::JSON_NUMBER_LONG:
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
        case JsonElement::Type::JSON_NUMBER_LONG: {
            m_value.numberLongValue = ele.m_value.numberLongValue;
            break;
        }
        case JsonElement::Type::JSON_NUMBER_DOUBLE: {
            m_value.numberDoubleValue = ele.m_value.numberDoubleValue;
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
        case JsonElement::Type::JSON_NUMBER_LONG:
        case JsonElement::Type::JSON_NUMBER_DOUBLE:
        case JsonElement::Type::JSON_BOOL:
        case JsonElement::Type::JSON_NULL:
            break;
    }
}

bool& JsonElement::AsBool()
{
    if (m_type != JsonElement::Type::JSON_BOOL) {
        Panic("failed to convert json element %s as a bool", TypeName().c_str());
    }
    return m_value.boolValue;
}

double& JsonElement::AsDouble()
{
    if (m_type != JsonElement::Type::JSON_NUMBER_DOUBLE) {
        Panic("failed to convert json element %s as a double", TypeName().c_str());
    }
    return m_value.numberDoubleValue;
}

int64_t& JsonElement::AsLongInt()
{
    if (m_type != JsonElement::Type::JSON_NUMBER_LONG) {
        Panic("failed to convert json element %s as a long int", TypeName().c_str());
    }
    return m_value.numberLongValue;
}

void* JsonElement::AsNull() const
{
    if (m_type != JsonElement::Type::JSON_NULL) {
        Panic("failed to convert json element %s as a null", TypeName().c_str());
    }
    return nullptr;
}

std::string& JsonElement::AsString()
{
    if (m_type != JsonElement::Type::JSON_STRING) {
        Panic("failed to convert json element %s as a string", TypeName().c_str());
    }
    return *(m_value.stringValue);
}

JsonObject& JsonElement::AsJsonObject()
{
    if (m_type != JsonElement::Type::JSON_OBJECT) {
        Panic("failed to convert json element %s as an object", TypeName().c_str());
    }
    return *(m_value.objectValue);
}

JsonArray& JsonElement::AsJsonArray()
{
    if (m_type != JsonElement::Type::JSON_ARRAY) {
        Panic("failed to convert json element %s as an array", TypeName().c_str());
    }
    return *(m_value.arrayValue);
}



bool JsonElement::ToBool() const
{
    if (m_type != JsonElement::Type::JSON_BOOL) {
        Panic("failed to convert json element %s as a bool", TypeName().c_str());
    }
    return m_value.boolValue;
}

double JsonElement::ToDouble() const
{
    if (m_type != JsonElement::Type::JSON_NUMBER_LONG && m_type != JsonElement::Type::JSON_NUMBER_DOUBLE) {
        Panic("failed to convert json element %s as a double", TypeName().c_str());
    }
    return static_cast<double>(m_value.numberDoubleValue);
}

int64_t JsonElement::ToLongInt() const
{
    if (m_type != JsonElement::Type::JSON_NUMBER_LONG && m_type != JsonElement::Type::JSON_NUMBER_DOUBLE) {
        Panic("failed to convert json element %s as a long int", TypeName().c_str());
    }
    return static_cast<int64_t>(m_value.numberLongValue);
}

void* JsonElement::ToNull() const
{
    if (m_type != JsonElement::Type::JSON_NULL) {
        Panic("failed to convert json element %s as a null", TypeName().c_str());
    }
    return nullptr;
}

std::string JsonElement::ToString() const
{
    if (m_type != JsonElement::Type::JSON_STRING) {
        Panic("failed to convert json element %s as a string", TypeName().c_str());
    }
    return *(m_value.stringValue);
}

JsonObject JsonElement::ToJsonObject() const
{
    if (m_type != JsonElement::Type::JSON_OBJECT) {
        Panic("failed to convert json element %s as an object", TypeName().c_str());
    }
    return *(m_value.objectValue);
}

JsonArray JsonElement::ToJsonArray() const
{
    if (m_type != JsonElement::Type::JSON_ARRAY) {
        Panic("failed to convert json element %s as an array", TypeName().c_str());
    }
    return *(m_value.arrayValue);
}


bool JsonElement::IsNull() const { return m_type == JsonElement::Type::JSON_NULL; }
bool JsonElement::IsBool() const { return m_type == JsonElement::Type::JSON_BOOL; }
bool JsonElement::IsDouble() const { return m_type == JsonElement::Type::JSON_NUMBER_DOUBLE; }
bool JsonElement::IsLongInt() const { return m_type == JsonElement::Type::JSON_NUMBER_LONG; }
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
        case JsonElement::Type::JSON_NUMBER_LONG:
            return "JSON_NUMBER_LONG";
        case JsonElement::Type::JSON_NUMBER_DOUBLE:
            return "JSON_NUMBER_DOUBLE";
        case JsonElement::Type::JSON_BOOL:
            return "JSON_BOOL";
        case JsonElement::Type::JSON_NULL:
            return "JSON_NULL";
    }
    Panic("invalid type for TypeName()");
    return "";
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
        case JsonElement::Type::JSON_NUMBER_DOUBLE: {
            return DoubleToString(m_value.numberDoubleValue);
        }
        case JsonElement::Type::JSON_NUMBER_LONG: {
            return LongIntToString(m_value.numberLongValue);
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
    Panic("unknown type to serialize: %s", TypeName().c_str());
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
    bool moreThanOne = false;
    for (const auto& v: *this) {
        moreThanOne = true;
        res += v.Serialize() + ",";
    }
    if (moreThanOne) {
        res.pop_back();
    }
    res += "]";
    return res;
}


JsonScanner::JsonScanner(const std::string &str): m_str(str), m_pos(0), m_prevPos(0)
{}

void JsonScanner::Reset() { m_pos = 0; m_prevPos = 0; }

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
    Panic("Invalid token at position %lu", m_pos);
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
                Panic("missing token, position: %lu", m_pos);
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
        Panic("missing end of string, position: %lu", beginPos);
    }
    m_pos ++; // skip right "
    std::string rawStr = m_str.substr(beginPos + 1, m_pos - beginPos - 2);
    m_tmpStrValue = util::UnescapeString(rawStr);
}

void JsonScanner::ScanNextNumber()
{
    size_t beginPos = m_pos;
    // example: "-114.51E-4"
    m_pos ++; // skip + or - or first digit
    while (m_pos < m_str.size() && IsDigit(m_str[m_pos])) {
        m_pos ++;
    }
    if (m_pos + 1 < m_str.size() && m_str[m_pos] == '.' && IsDigit(m_str[m_pos + 1])) {
        m_pos ++; // skip .
        while(m_pos < m_str.size() && IsDigit(m_str[m_pos])) {
            m_pos ++;
        }
    }
    if (m_pos + 1 < m_str.size() && (m_str[m_pos] == 'E' || m_str[m_pos] == 'e')) {
        m_pos ++;
        if (m_str[m_pos] == '-' || m_str[m_pos] == '+') {
            m_pos ++;
        }
        // parse number
        while (m_pos < m_str.size() && IsDigit(m_str[m_pos])) {
            m_pos ++;
        }
    }

    std::string numberStr = m_str.substr(beginPos, m_pos - beginPos);
    if (numberStr.find_last_of("eE.") == std::string::npos) {
        try {
            m_tmpNumberLongValue = std::atoll(numberStr.c_str());
            m_int64Number = true;
        } catch (...) {
            Panic("invalid number %lf, pos: %lu", m_tmpNumberLongValue, beginPos);
        }
    } else {
        try {
            m_tmpNumberDoubleValue = std::atof(numberStr.c_str());
            m_int64Number = false;
        } catch (...) {
            Panic("invalid number %llu, pos: %lu", m_tmpNumberDoubleValue, beginPos);
        }
    }

}

double JsonScanner::GetDoubleValue() const { return m_tmpNumberDoubleValue; }

int64_t JsonScanner::GetLongIntValue() const { return m_tmpNumberLongValue; }

bool JsonScanner::IsNumberLongInt() const { return m_int64Number; }

std::string JsonScanner::GetStringValue() const { return m_tmpStrValue; }

std::string JsonScanner::TokenName(Token token)
{
    switch (token) {
        case Token::WHITESPACE:     return "WHITESPACE";
        case Token::NUMBER:         return "NUMBER";
        case Token::STRING:         return "STRING";
        case Token::LITERAL_TRUE:   return "LITERAL_TRUE";
        case Token::LITERAL_FALSE:  return "LITERAL_FALSE";
        case Token::LITERAL_NULL:   return "LITERAL_NULL";
        case Token::COMMA:          return "COMMA";
        case Token::COLON:          return "COLON";
        case Token::ARRAY_BEGIN:    return "ARRAY_BEGIN";
        case Token::ARRAY_END:      return "ARRAY_END";
        case Token::OBJECT_BEGIN:   return "OBJECT_BEGIN";
        case Token::OBJECT_END:     return "OBJECT_END";
        case Token::EOF_TOKEN:      return "EOF_TOKEN";
    }
    Panic("Unexpected token");
    return "";
}



JsonParser::JsonParser(const std::string& str) 
{
    m_scanner = new JsonScanner(str);
}

JsonParser::~JsonParser()
{
    if (m_scanner != nullptr) {
        delete m_scanner;
        m_scanner = nullptr;
    }
}

JsonElement JsonParser::Parse()
{
    m_scanner->Reset();
    JsonElement ele = ParseNext();
    if (m_scanner->Next() != JsonScanner::Token::EOF_TOKEN) {
        Panic("json scanner reached non-eof token, position = %lu", m_scanner->Position());
    }
    return ele;
}

bool JsonParser::IsValid()
{
    try {
        Parse();
    } catch (...) {
        return false;
    }
    return true;
}

JsonElement JsonParser::ParseNext()
{
    JsonScanner::Token token = m_scanner->Next();
    switch (token) {
        case JsonScanner::Token::OBJECT_BEGIN: {
            return ParseJsonObject();
        }
        case JsonScanner::Token::ARRAY_BEGIN: {
            return ParseJsonArray();
        }
        case JsonScanner::Token::STRING: {
            return JsonElement(m_scanner->GetStringValue());
        }
        case JsonScanner::Token::NUMBER: {
            return m_scanner->IsNumberLongInt() ? 
                JsonElement(m_scanner->GetLongIntValue()) : JsonElement(m_scanner->GetDoubleValue());
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
        case JsonScanner::Token::WHITESPACE:
        case JsonScanner::Token::COMMA:
        case JsonScanner::Token::COLON:
        case JsonScanner::Token::ARRAY_END:
        case JsonScanner::Token::OBJECT_END:
        case JsonScanner::Token::EOF_TOKEN:
        default : Panic("scanner return unexpected token: %s", JsonScanner::TokenName(token).c_str());
    }
    return JsonElement();
}

JsonObject JsonParser::ParseJsonObject()
{
    JsonObject object {};
    JsonScanner::Token token = m_scanner->Next();
    if (token == JsonScanner::Token::OBJECT_END) {
        return object;
    }
    m_scanner->RollBack();

    while (true) {
        size_t pos = m_scanner->Position();
        token = m_scanner->Next();
        if (token != JsonScanner::Token::STRING) {
            Panic("expect a string as key for json object, position: %lu", pos);
        }
        std::string key = m_scanner->GetStringValue();

        pos = m_scanner->Position();
        token = m_scanner->Next();
        if (token != JsonScanner::Token::COLON) {
            Panic("expect ':' in json object, position: %lu", pos);
        }
        JsonElement ele = ParseNext();
        object[key] = ele;

        pos = m_scanner->Position();
        token = m_scanner->Next();
        if (token == JsonScanner::Token::OBJECT_END) {
            break;
        }
        if (token != JsonScanner::Token::COMMA) {
            Panic("expect ',' in json object, position: %lu", pos);
        }
    }
    return object;
}

JsonArray JsonParser::ParseJsonArray()
{
    JsonArray array {};
    JsonScanner::Token token = m_scanner->Next();
    if (token == JsonScanner::Token::ARRAY_END) {
        return array;
    }
    m_scanner->RollBack();

    while (true) {
        array.push_back(ParseNext());
        size_t pos = m_scanner->Position();
        token = m_scanner->Next();
        if (token == JsonScanner::Token::ARRAY_END) {
            break;
        }
        if (token != JsonScanner::Token::COMMA) {
            Panic("expect ',' in array, pos: %lu", pos);
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
                break;
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

std::string util::UnescapeString(const std::string& str)
{
    std::string res;
    for (size_t i = 0; i < str.size(); ++i) {
        char curChar = str[i];
        if (curChar == '\\' && i + 1 < str.size()) {
            char escapeChar = str[++i];
            switch (escapeChar) {
                case '"':
                    res.push_back('"');
                    break;
                case '\\':
                    res.push_back('\\');
                    break;
                case '/':
                    res.push_back('/');
                    break;
                case 'f': {
                    res.push_back('\f');
                    break;
                }
                case 'b': {
                    res.push_back('\b');
                    break;
                }
                case 'r': {
                    res.push_back('\r');
                    break;
                }
                case 'n': {
                    res.push_back('\n');
                    break;
                }
                case 't': {
                    res.push_back('\t');
                    break;
                }
                Panic("invalid escaped char \\%c", escapeChar);
            }
        } else {
            res.push_back(curChar);
        }
    }
    return res;
}

std::string util::DoubleToString(double value)
{
    std::string res = std::to_string(value);
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

std::string util::LongIntToString(int64_t value)
{
    std::string res = std::to_string(value);
    return res;
}
