#ifndef __StringHelper_h__
#define __StringHelper_h__
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class StringHelper
{
  public:
    static std::vector<std::string> split(const std::string &s, char delim);
  private: 
    static std::vector<std::string> splitHelper(const std::string &s, char delim, std::vector<std::string> &elems);
    
};

#endif // __StringHelper_h__

