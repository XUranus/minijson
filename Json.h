/*================================================================
*   Copyright (C) 2022 XUranus All rights reserved.
*   
*   File:         Json.h
*   Author:       XUranus
*   Date:         2022-11-21
*   Description:  a tiny C++ Json library
*                 https://github.com/XUranus/minicpp
*
================================================================*/

#ifndef _XURANUS_MINI_JSON_HEADER_
#define _XURANUS_MINI_JSON_HEADER_

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string>
#include <type_traits>
#include <utility>
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
 *
 * @brief
 * add -DLIBRARY_EXPORT build param to export lib on Win32 MSVC
 * define LIBRARY_IMPORT before including Json.h to add __declspec(dllimport) to use dll library
 * libminijson is linked static by default
 */

#ifdef _WIN32
    #ifdef LIBRARY_EXPORT
        #define MINIJSON_API __declspec(dllexport)
    #else
        #ifdef LIBRARY_IMPORT
            #define MINIJSON_API __declspec(dllimport)
        #else
            #define MINIJSON_API
        #endif
    #endif
#else
    #define MINIJSON_API  __attribute__((__visibility__("default")))
#endif

#define SERIALIZE_SECTION_BEGIN                                                                 \
public:                                                                                         \
    using __XURANUS_JSON_SERIALIZATION_MAGIC__ = void;                                            \
public:                                                                                         \
    void _XURANUS_JSON_CPP_SERIALIZE_METHOD_(xuranus::minijson::JsonObject& object, bool toJson)   \
    {                                                                                             \

#define SERIALIZE_SECTION_END                                                                   \
    };                                                                                            \

#define SERIALIZE_FIELD(KEY_NAME, ATTR_NAME)                                                    \
    do {                                                                                          \
        if (toJson) {                                                                               \
            xuranus::minijson::rules::SerializeTo(object, #KEY_NAME, ATTR_NAME);                        \
        } else {                                                                                    \
            xuranus::minijson::rules::DeserializeFrom(object, #KEY_NAME, ATTR_NAME);                    \
        }                                                                                           \
    } while (0)                                                                                   \

namespace xuranus {
namespace minijson {

class JsonElement;
class JsonObject;
class JsonArray;
class JsonScanner;

inline void Panic(const char* str, ...)
{
    const int PANIC_MESSAGE_MAX = 512;
    char message[PANIC_MESSAGE_MAX]; 
    va_list args;
    va_start(args, str);
#ifdef _MSC_VER
    // supress MSVC warning
    vsprintf_s(message, PANIC_MESSAGE_MAX, str, args);
#else
    vsprintf(message, str, args);
#endif
    va_end(args);
    throw std::logic_error(message); 
}

// serializable interface
class MINIJSON_API Serializable {
    virtual std::string Serialize() const = 0;
};

// base class of json variant
class MINIJSON_API JsonElement: public Serializable {
    public:
        enum class Type {
            JSON_OBJECT,
            JSON_ARRAY,
            JSON_STRING,
            JSON_NUMBER_LONG,
            JSON_NUMBER_DOUBLE,
            JSON_BOOL,
            JSON_NULL
        };

        union Value {
            JsonObject* objectValue;
            JsonArray* arrayValue;
            std::string* stringValue;
            int64_t numberLongValue;
            double numberDoubleValue;
            bool boolValue;
        };

    public:
        JsonElement();
        explicit JsonElement(JsonElement::Type type);
        explicit JsonElement(bool value);
        explicit JsonElement(double num);
        explicit JsonElement(int64_t num);
        explicit JsonElement(const std::string &str);
        explicit JsonElement(char const *str);
        JsonElement(const JsonObject& object);
        JsonElement(const JsonArray& array);

        JsonElement(const JsonElement& ele);
        explicit JsonElement(JsonElement&& ele);
        JsonElement& operator = (const JsonElement& ele);
        ~JsonElement();

        bool& AsBool();
        double& AsDouble();
        int64_t& AsLongInt();
        void* AsNull() const;
        std::string& AsString();
        JsonObject& AsJsonObject();
        JsonArray& AsJsonArray();

        bool ToBool() const;
        double ToDouble() const;
        int64_t ToLongInt() const;
        void* ToNull() const;
        std::string ToString() const;
        JsonObject ToJsonObject() const;
        JsonArray ToJsonArray() const;
        
        bool IsNull() const;
        bool IsBool() const;
        bool IsLongInt() const;
        bool IsDouble() const;
        bool IsString() const;
        bool IsJsonObject() const;
        bool IsJsonArray() const;

        std::string TypeName() const;
        std::string Serialize() const override;

    private:
        Type m_type = Type::JSON_NULL;
        Value m_value {};
};

class MINIJSON_API JsonObject: public std::map<std::string, JsonElement>, public Serializable {
public:
    std::string Serialize() const override;
};

class MINIJSON_API JsonArray: public std::vector<JsonElement>, public Serializable {
public:
    std::string Serialize() const override;
};

class MINIJSON_API JsonParser {
    public:
        explicit JsonParser(const std::string& str);
        ~JsonParser();
        JsonElement Parse();
        bool IsValid();
    private:
        JsonElement ParseNext();
        JsonObject ParseJsonObject();
        JsonArray ParseJsonArray();
    private:
        JsonScanner* m_scanner { nullptr };
};

// use CastFromJsonElement & CastToJsonElement template methods to define some serialization/deserialzation rules
namespace rules {

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
    template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
    void CastFromJsonElement(const JsonElement& ele, T& value) {
        value = static_cast<T>(ele.ToLongInt());
        return;
    }

    template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    void CastFromJsonElement(const JsonElement& ele, T& value) {
        value = static_cast<T>(ele.ToDouble());
        return;
    }

    template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
    void CastToJsonElement(JsonElement& ele, const T& value) {
        ele = JsonElement(static_cast<int64_t>(value));
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

    // cast between std::pair and JsonElement
    template<typename T, typename std::enable_if<
        std::is_same<T, std::pair<typename T::first_type, typename T::second_type>>::value
        >::type* = nullptr>
    void CastFromJsonElement(const JsonElement& ele, T& value) {
        JsonArray array = ele.ToJsonArray();
        if (array.size() < 2) {
            return;
        }
        CastFromJsonElement<typename T::first_type>(array[0], value.first);
        CastFromJsonElement<typename T::second_type>(array[1], value.second);
        return;
    }

    template<typename T, typename std::enable_if<
        std::is_same<T, std::pair<typename T::first_type, typename T::second_type>>::value
        >::type* = nullptr>
    void CastToJsonElement(JsonElement& ele, const T& value) {
        JsonArray array;
        JsonElement firstItemElement;
        JsonElement secondItemElement;
        CastToJsonElement<typename T::first_type>(firstItemElement, value.first);
        CastToJsonElement<typename T::second_type>(secondItemElement, value.second);
        array.push_back(firstItemElement);
        array.push_back(secondItemElement);
        ele = JsonElement(array);
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

// utils used for user to do serialization and deserialzation
namespace util {
    template<typename T>
    auto Serialize(const T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__(), std::string());

    template<typename T>
    auto Deserialize(const std::string& jsonStr, T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__());
}

// util template function implement
template<typename T>
auto util::Serialize(const T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__(), std::string())
{
    JsonObject object {};
    auto valuePtr = const_cast<typename std::remove_const<T>::type*>(&value);
    valuePtr->_XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, true); 
    return object.Serialize();
}

template<typename T>
auto util::Deserialize(const std::string& jsonStr, T& value) -> decltype(typename T::__XURANUS_JSON_SERIALIZATION_MAGIC__())
{
    JsonParser parser(jsonStr);
    JsonElement ele = parser.Parse();
    JsonObject object = ele.AsJsonObject();
    value._XURANUS_JSON_CPP_SERIALIZE_METHOD_(object, false);
}

}
}

#endif
