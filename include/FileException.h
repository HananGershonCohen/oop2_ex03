#pragma once 
#include <exception>
#include <string>

class FileException : public std::exception
{
public:
    FileException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;

};
