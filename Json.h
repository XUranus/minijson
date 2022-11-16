#ifndef _XURANUS_JSON_CPP_
#define _XURANUS_JSON_CPP_

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string>
#include <vector>
#include <map>

#define Panic(str, ...) do { \
  char message[100]; \
  ::sprintf(message, str"\n", ##__VA_ARGS__); \
  throw std::logic_error(message); \
} while(0); \

/**
 * https://www.json.org/json-en.html
 *
 *                JsonElement
 *                    |
 *   ------------------------------------------
 *   |      |         |           |           |
 * null  JsonNumber  bool   JsonObject  JsonArray
 *        
 *
 *
 *
 **/
namespace xuranus {
namespace jsoncpp {

class JsonElement;
class JsonObject;
class JsonArray;

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
    
    bool IsNull() const;
    bool IsBool() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsJsonObject() const;
    bool IsJsonArray() const;

    std::string TypeName() const;

    template<typename T> void Cast(T& value) const {
      //Panic("unsupport cast %s", typeid(T).name());
      value._json_field_map_(*(m_value.objectValue), false);
      return;
    }

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
    enum  Token {
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
    Token Next();
    double GetNumberValue();
    std::string GetStringValue();
    inline void RollBack() { m_pos = m_prevPos; }
    inline size_t Position() { return m_pos; }

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
        Panic("unknown literal token, position = %d, do you mean: %s ?", m_pos, literal.c_str());
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
  private:
    JsonObject ParseJsonObject();
    JsonArray ParseJsonArray();
};

namespace util {
  std::string EscapeString(const std::string& str);
  std::string DoubleToString(double value);// TODO

  template<typename T>
  std::string Serialize(const T& value)
  {
    JsonObject object {};
    value._json_field_map_(object, true); 
    return object.Serialize();
  }

  template<typename T>
  void Deserialize(const std::string& jsonStr, T& value)
  {
    JsonParser parser(jsonStr);
    JsonElement ele = parser.Parse();
    JsonObject object = ele.AsJsonObject();
    value._json_field_map_(object, false);
  }



  template<typename T>
  void SerializeTo(JsonObject &object, const std::string& key, const T& field)
  {
    object[key] = JsonElement(field);
  }

  template<typename T>
  void DeserializeFrom(const JsonObject& object, const std::string& key, T& field)
  {
    JsonElement ele = object.find(key)->second;
    std::cout << typeid(T).name() << std::endl;
    ele.Cast(field);
  }
}

}
}

#endif