# MiniJson
> a mini Json library C++ implement

<div align="center">
<img src="https://github.com/XUranus/minijson/actions/workflows/cmake-multi-platform.yml/badge.svg" alt="minijson" title="minijson">&thinsp;
<img src="https://img.shields.io/badge/-C++11-3F63B3.svg?style=flat&logo=C%2B%2B&logoColor=white" alt="C++14" title="C++ Standards Used: C++11">&thinsp;
<img src="https://img.shields.io/badge/-Windows-6E46A2.svg?style=flat&logo=windows-11&logoColor=white" alt="Windows" title="Supported Platform: Windows">&thinsp;
<img src="https://img.shields.io/badge/-Linux-9C2A91.svg?style=flat&logo=linux&logoColor=white" alt="Linux" title="Supported Platform: Linux">&thinsp;
<img src="https://img.shields.io/badge/MSVC%202015+-flag.svg?color=555555&style=flat&logo=visual%20studio&logoColor=white" alt="MSVC 2015+" title="Supported Windows Compiler: MSVC 2015 or later">&thinsp;
<img src="https://img.shields.io/badge/GCC%204.9+-flag.svg?color=555555&style=flat&logo=gnu&logoColor=white" alt="GCC 4.9+" title="Supported Unix Compiler: GCC 4.9 or later">&thinsp;
</div>

## build & test
build static/dynamic library:
```
mkdir build && cd build
cmake .. && cmake --build .
```

build and run test coverage:
```
mkdir build && cd build
# use lcov
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=lcov
# or use gcovr
# cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=gcovr
cmake --build .
make minijson_coverage_test
```

## usage
1. Serialization/deserilization of basic type
```C++
#include <bits/stdc++.h>
#include "Json.h"
using namespace xuranus::minijson;

int main() {
  std::string jsonStr = R"(
    {
      "name" : "xuranus",
      "age" : 300,
      "skills" : ["C++", "Java", "Python"]
    }
  )";
  JsonElement element = JsonParser(jsonStr).Parse();
  JsonObject object = element.AsJsonObject();
  std::cout << object["name"].AsString() << std::endl; // xuranus
  std::cout << object["age"].AsNumber() << std::endl; // 300
  std::cout << object["skills"].AsJsonArray()[0].AsString() << std::endl; // C++
  std::cout << object["skills"].AsJsonArray()[1].AsString() << std::endl; // Java
  std::cout << object["skills"].AsJsonArray()[2].AsString() << std::endl; // Python

  std::cout << element.Serialize() << std::endl; 
  // {"name" : "xuranus", "age" : 300, "skills" : ["C++", "Java", "Python"]}
}
```

2. Serialization/deserilization of struct
``` C++
#include <bits/stdc++.h>
#include "Json.h"
using namespace xuranus::minijson;

struct Book {
  std::string m_name;
  int m_id;
  float m_price;
  bool m_soldOut;
  std::vector<std::string> m_tags;
  
  SERIALIZE_SECTION_BEGIN
  SERIALIZE_FIELD(name, m_name);
  SERIALIZE_FIELD(id, m_id);
  SERIALIZE_FIELD(price, m_price);
  SERIALIZE_FIELD(soldOut, m_soldOut);
  SERIALIZE_FIELD(tags, m_tags);
  SERIALIZE_SECTION_END
};

int main() {
  Book book1 {};
  book1.m_name = "C++ Primer";
  book1.m_id = 114514;
  book1.m_price = 114.5;
  book1.m_soldOut = true;
  book1.m_tags = {"C++", "Programming", "Language"};

  
  std::string jsonStr = util::Serialize(book1);
  std::cout << jsonStr << std::endl; 
  /**
  * output:
  * {"id":114514,"name":"C++ Primer","price":114.5,"soldOut":true,"tags":["C++","Programming",*"Language"]}
  **/

  Book book2 {};
  util::Deserialize(jsonStr, book2);
  std::cout << book2.m_name << " " << book2.m_id << " " << book2.m_price << std::endl;
  /**
  * output:
  * C++ Primer 114514 114.5
  **/
  return 0;
}
```
see more usage in test cases at `test/MiniJsonTest.cpp`