#pragma once
#include <string>

class ErrorMessage
{
public:
    ErrorMessage();
    ~ErrorMessage();

    void LogMessage(std::string msg);
    void ThrowError(std::string msg);
};

extern ErrorMessage globalErrorMessage;