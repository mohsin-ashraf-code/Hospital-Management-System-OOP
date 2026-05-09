#include "HospitalException.h"

// Constructor using strict pointer arithmetic
HospitalException::HospitalException(const char* msg) 
{
    int i = 0;
    if (msg != nullptr)
    {
        while (*(msg + i) != '\0' && i < 199)
        {
            *(message + i) = *(msg + i);
            i++;
        }
    }
    *(message + i) = '\0';
}

// Virtual what() method
const char* HospitalException::what() const 
{
    return message;
}

// Destructor
HospitalException::~HospitalException()
{
}