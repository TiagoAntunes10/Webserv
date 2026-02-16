#ifndef MIME_HPP
#define MIME_HPP

#include <iostream>
#include <map>

class Mime {
public:
  typedef std::map<std::string, std::string>::iterator iterator;

  void initMime(void);
  static Mime &getMimeInfo(void);
  std::string getMimeType(std::string const &extension);
  std::string getExtension(std::string const &type);

private:
  Mime(void) {};
  Mime(Mime const &) {};
  Mime &operator=(Mime const &) { return (*this); };

  std::map<std::string, std::string> mime_types_;
};

#endif
