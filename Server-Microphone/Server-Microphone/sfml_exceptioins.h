#pragma once
#include <exception>
#include <string>

class PortBindingException : public std::exception {
  unsigned port;
public:
  PortBindingException(unsigned port) : port(port) {}
  virtual const char* what() const throw () {
    return ("Cannot be binding to port" + std::to_string(port)).c_str();
  }
};