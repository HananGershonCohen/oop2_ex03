#pragma once 
#include <exception>
#include <string>

class InputException : public std::exception 
{
public:
    InputException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;

};
