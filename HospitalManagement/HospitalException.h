#pragma once
#include <exception>

class HospitalException : public std::exception 
{
protected:
	char message[200]; //PDF demanded static array
public:
    HospitalException(const char* msg);
    virtual const char* what() const noexcept override;
};