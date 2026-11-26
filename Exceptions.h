#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include <string>

class CargoOverweightException : public std::exception {
    std::string msg;
public:
    explicit CargoOverweightException(const std::string& m): msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};
class FuelDepletionException : public std::exception {
    std::string msg;
public:
    explicit FuelDepletionException(const std::string& m): msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};
class DamagedCorridorException : public std::exception {
    std::string msg;
public:
    explicit DamagedCorridorException(const std::string& m): msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};
#endif
