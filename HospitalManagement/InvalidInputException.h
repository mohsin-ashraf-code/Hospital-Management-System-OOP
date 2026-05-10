#pragma once
#include "HospitalException.h"

class InvalidInputException : public HospitalException 
{
public:
    InvalidInputException(const char* msg) : HospitalException(msg) 
    {
    }
};