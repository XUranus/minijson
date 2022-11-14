#include <iostream>
#include "Json.h"

using namespace xuranus::jsoncpp;

int main()
{
  //std::string str1 = R"({"name":"xuranus"})";
  
  //std::string str1 = R"(null)";
  //std::string str1 = R"(123.44)";
  //std::string str1 = R"("helloworld")";
  //std::string str1 = R"([1,2,3,4,5])";
  std::string str1 = R"(["one", "two", "three", "four"])";
  std::cout << str1 << std::endl;
  JsonParser parser(str1);
  JsonElement ele = parser.Parse();
  std::cout << ele.Serialize() << std::endl;
  return 0;
}