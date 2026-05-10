#pragma once
#include "HospitalException.h"

class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
};