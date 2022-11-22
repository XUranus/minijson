/*================================================================
*   Copyright (C) 2022 XUranus All rights reserved.
*   
*   File:         Xml.h
*   Author:       XUranus
*   Date:         2022-11-21
*   Description:  
*
================================================================*/

#ifndef _XURANUS_JSON_CPP_
#define _XURANUS_JSON_CPP_

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <stdarg.h>

/**
 * https://www.json.org/json-en.html
 *
 *                JsonElement
 *                    |
 *   ------------------------------------------
 *   |      |         |           |           |
 * null  JsonNumber  bool   JsonObject  JsonArray
 *        
 **/
namespace xuranus {
namespace jsoncpp {

class JsonElement;
class JsonObject;
class JsonArray;

#define SERIALIZE_SECTION_BEGIN                                                                 \
public:                                                                                         \
  using __XURANUS_JSON_SERIALIZATION_MAGIC__ = void;                                            \
public:                                                                                         \
  void _XURANUS_JSON_CPP_SERIALIZE_METHOD_(xuranus::jsoncpp::JsonObject& object, bool toJson)   \
  {                                                                                             \

#define SERIALIZE_SECTION_END                                                                   \
  };                                                                                            \

#define SERIALIZE_FIELD(KEY_NAME, ATTR_NAME)                                                    \
  do {                                                                                          \
    if (toJson) {                                                                               \
      xuranus::jsoncpp::util::SerializeTo(object, #KEY_NAME, ATTR_NAME);                        \
    } else {                                                                                    \
      xuranus::jsoncpp::util::DeserializeFrom(object, #KEY_NAME, ATTR_NAME);                    \
    }                                                                                           \
  } while (0)                                                                                   \

#define Panic(str, ...) do {                                                                    \
  char message[100];                                                                            \
  ::sprintf(message, str"\n", ##__VA_ARGS__);                                                   \
  throw std::logic_error(message);                                                              \
} while(0);                                                                                     \

// inline void Panic(const char* str, ...)
// {
//   char message[100]; 
//   va_list args;
//   va_start(args, str);
//   ::sprintf(message, str, args);
//   ::va_end(args);
//   throw std::logic_error(message); 
// }

class Serializable {
  virtual std::string Serialize() const = 0;
};

class JsonElement: public Serializable {
  public:
    enum class Type {
      JSON_OBJECT,
      JSON_ARRAY,
      JSON_STRING,
      JSON_NUMBER,
      JSON_BOOL,
      JSON_NULL
    };

    union Value {
      JsonObject* objectValue;
      JsonArray* arrayValue;
      std::string* stringValue;
      double numberValue;
      bool boolValue;
    };

  public:
    JsonElement();
    JsonElement(JsonElement::Type type);
    JsonElement(bool value);
    JsonElement(double num);
    JsonElement(long num);
    JsonElement(const std::string &str);
    JsonElement(const JsonObject& object);
    JsonElement(const JsonArray& array);

    JsonElement(const JsonElement& ele);
    JsonElement(JsonElement&& ele);
    JsonElement& operator = (const JsonElement& ele);
    ~JsonElement();

    bool& AsBool();
    double& AsNumber();
    void* AsNull();
    std::string& AsString();
    JsonObject& AsJsonObject();
    JsonArray& AsJsonArray();

    bool ToBool() const;
    double ToNumber() const;
    void* ToNull() const;
    std::string ToString() const;
    JsonObject ToJsonObject() const;
    JsonArray ToJsonArray() const;
    
    bool IsNull() const;
    bool IsBool() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsJsonObject() const;
    bool IsJsonArray() const;

    std::string TypeName() const;
    std::string Serialize() const override;

  private:
    Type m_type = Type::JSON_NULL;
    Value m_value {};
};

class JsonObject: public std::map<std::string, JsonElement>, public Serializable {
public:
  std::string Serialize() const override;
};

class JsonArray: public std::vector<JsonElement>, public Serializable {
public:
  std::string Serialize() const override;
};



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
    double GetNumberValue();
    std::string GetStringValue();
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
    double m_tmpNumberValue {0};
};


class JsonParser {
  private:
    JsonScanner m_scanner;

  public:
    JsonParser(const std::string str);
    JsonElement Parse();
    bool IsValid();
  private:
    JsonElement ParseNext();
    JsonObject ParseJsonObject();
    JsonArray ParseJsonArray();
};

// cast between struct and JsonElement
template<typename T>
auto CastFromJsonElement(const JsonElement& ele, T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__()) {
  JsonObject object = ele.ToJsonObject();
  value._XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, false);
  return;
};

template<typename T>
auto CastToJsonElement(JsonElement& ele, const T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__()) {
  JsonObject object {};
  T* valueRef = reinterpret_cast<T*>((void*)&value);
  valueRef->_XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, true);
  ele = JsonElement(object);
  return;
};

// cast between std::string and JsonElement
template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void CastFromJsonElement(const JsonElement& ele, T& value) {
  value = ele.ToString();
  return;
}

template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  ele = JsonElement(value);
  return;
}

// cast between numeric and JsonElement
template<typename T, typename std::enable_if<(std::is_integral<T>::value || std::is_floating_point<T>::value) 
  && !std::is_same<T, bool>::value>::type* = nullptr>
void CastFromJsonElement(const JsonElement& ele, T& value) {
  value = static_cast<T>(ele.ToNumber());
  return;
}

template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  ele = JsonElement(static_cast<long>(value));
  return;
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  ele = JsonElement(static_cast<double>(value));
  return;
}

// cast between bool and JsonElement
template<typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
void CastFromJsonElement(const JsonElement& ele, T& value) {
  value = ele.ToBool();
  return;
}

template<typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  ele = JsonElement(value);
  return;
}

// cast between std::vector or std::list and JsonElement
template<typename T, typename std::enable_if<
  std::is_same<T, std::vector<typename T::value_type>>::value ||
  std::is_same<T, std::list<typename T::value_type>>::value
  >::type* = nullptr>
void CastFromJsonElement(const JsonElement& ele, T& value) {
  JsonArray array = ele.ToJsonArray();
  value.clear();
  for (const JsonElement& eleItem: array) {
    typename T::value_type t;
    CastFromJsonElement<typename T::value_type>(eleItem, t);
    value.push_back(t);
  }
  return;
}

template<typename T, typename std::enable_if<
  std::is_same<T, std::vector<typename T::value_type>>::value ||
  std::is_same<T, std::list<typename T::value_type>>::value
  >::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  JsonArray array;
  for (const typename T::value_type& item: value) {
    JsonElement itemElement;
    CastToJsonElement<typename T::value_type>(itemElement, item);
    array.push_back(itemElement);
  }
  ele = JsonElement(array);
  return;
}

// cast between std::map and JsonElement
template<typename T, typename std::enable_if<
  std::is_same<std::string, typename T::key_type>::value && (
  std::is_same<T, std::map<std::string, typename T::mapped_type>>::value ||
  std::is_same<T, std::unordered_map<std::string, typename T::mapped_type>>::value
  )>::type* = nullptr>
void CastFromJsonElement(const JsonElement& ele, T& value) {
  JsonObject object = ele.ToJsonObject();
  value.clear();
  for (const std::pair<std::string, JsonElement>& p: object) {
    typename T::mapped_type v;
    CastFromJsonElement<typename T::mapped_type>(p.second, v);
    value[p.first] = v;
  }
  return;
}

template<typename T, typename std::enable_if<
  std::is_same<std::string, typename T::key_type>::value && (
  std::is_same<T, std::map<std::string, typename T::mapped_type>>::value ||
  std::is_same<T, std::unordered_map<std::string, typename T::mapped_type>>::value
  )>::type* = nullptr>
void CastToJsonElement(JsonElement& ele, const T& value) {
  JsonObject object;
  for (const std::pair<std::string, typename T::mapped_type>& p: value) {
    JsonElement valueElement;
    CastToJsonElement<typename T::mapped_type>(valueElement, p.second);
    object[p.first] = valueElement;
  }
  ele = JsonElement(object);
  return;
}


namespace util {
  std::string EscapeString(const std::string& str);
  std::string DoubleToString(double value);

  template<typename T>
  auto Serialize(T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__(), std::string())
  {
    JsonObject object {};
    value._XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, true); 
    return object.Serialize();
  }

  template<typename T>
  auto Deserialize(const std::string& jsonStr, T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__())
  {
    JsonParser parser(jsonStr);
    JsonElement ele = parser.Parse();
    JsonObject object = ele.AsJsonObject();
    value._XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, false);
  }



  template<typename T>
  void SerializeTo(JsonObject &object, const std::string& key, const T& field)
  {
    JsonElement ele {};
    CastToJsonElement<T>(ele, field);
    object[key] = ele;
  }

  template<typename T>
  void DeserializeFrom(const JsonObject& object, const std::string& key, T& field)
  {
    JsonElement ele = object.find(key)->second;
    CastFromJsonElement<T>(ele, field);
  }
}

}
}

#endif