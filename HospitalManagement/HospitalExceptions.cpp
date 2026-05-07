#include "HospitalExceptions.h"

using namespace std; // Included exactly as requested

// ==========================================
// Base Class Implementation
// ==========================================

HospitalException::HospitalException(const char* msg) {
    int i = 0;
    if (msg != nullptr) {
        // Safely copy up to 199 characters to leave room for the null terminator
        while (*(msg + i) != '\0' && i < 199) {
            *(message + i) = *(msg + i);
            i++;
        }
    }
    *(message + i) = '\0';
}

const char* HospitalException::what() const {
    return message;
}

HospitalException::~HospitalException() {
    // Nothing to delete dynamically since message[200] is a static array
}

// ==========================================
// Derived Classes Implementations
// ==========================================

// Simply pass the specific message up to the base constructor
FileNotFoundException::FileNotFoundException(const char* msg)
    : HospitalException(msg) {
}

InsufficientFundsException::InsufficientFundsException(const char* msg)
    : HospitalException(msg) {
}

InvalidInputException::InvalidInputException(const char* msg)
    : HospitalException(msg) {
}

SlotUnavailableException::SlotUnavailableException(const char* msg)
    : HospitalException(msg) {
}