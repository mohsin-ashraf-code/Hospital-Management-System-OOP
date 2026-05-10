#pragma once
#include <exception>

class HospitalException : public std::exception {
protected:
    char message[200];
public:
    HospitalException(const char* msg);
    virtual const char* what() const noexcept override;
};