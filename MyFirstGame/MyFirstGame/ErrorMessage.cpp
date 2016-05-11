#include "ErrorMessage.h"
#include <Windows.h>

static int numOfInstances = 0;

ErrorMessage::ErrorMessage()
{
    OutputDebugString("Creating Error Diagnostics logger\r\n");

    if (numOfInstances == 0)
        numOfInstances++;
    else
        throw "Error Diagnostics object already exists!";
}


ErrorMessage::~ErrorMessage()
{
    numOfInstances--;
    OutputDebugString("Destroying Error Diagnostics logger\r\n");
}

void ErrorMessage::LogMessage(std::string msg)
{
    std::string newMsg = "ErrorMessage::";
    newMsg.append(msg);
    OutputDebugString(newMsg.c_str());
}

void ErrorMessage::ThrowError(std::string msg)
{
    throw msg;
}
