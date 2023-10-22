#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_
#include "HashMap.hpp"

#define ERROR_MSG "Error: Invalid key"

class InvalidKey : public std::invalid_argument
{
  public:
   InvalidKey() : std::invalid_argument(ERROR_MSG) {} // default constructor
   explicit InvalidKey(const std::string& msg) : std::invalid_argument(msg) {}
};

class Dictionary : public HashMap<std::string, std::string>
{
 public:
  // constructors:
  Dictionary() = default;
  Dictionary(const std::vector<std::string> &keys, const
  std::vector<std::string> &values) : HashMap<std::string, std::string>
      (keys, values) {}

  virtual bool erase(const std::string& key)
  {
    if (!HashMap::erase(key))
    {
      throw InvalidKey(ERROR_MSG);
    }
    return true;
  }

  template<class InputIterator>
  void update(InputIterator first, InputIterator last)
  {
    while (first != last)
    {
      if (contains_key (first->first))
      {
        HashMap::erase(first->first);
      }
      insert (first->first, first->second);
    }
    first++;
  }
};

#endif //_DICTIONARY_HPP_
