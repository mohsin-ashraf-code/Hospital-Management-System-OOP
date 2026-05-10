#pragma once
#include "HospitalException.h"

class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const char* msg) : HospitalException(msg) {}
};