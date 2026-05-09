#pragma once
class HospitalException 
{
protected:
    char message[200];

public:
    HospitalException(const char* msg);
    virtual ~HospitalException();
    virtual const char* what() const;
};