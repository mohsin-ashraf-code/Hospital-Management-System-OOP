#pragma once

// --- 1. Base Exception ---
class HospitalException {
protected:
    char message[200]; // Fixed array as requested
public:
    HospitalException(const char* msg);
    virtual const char* what() const;
    virtual ~HospitalException();
};

// --- 2. Derived: File Not Found ---
class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const char* msg);
};

// --- 3. Derived: Insufficient Funds ---
class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException(const char* msg);
};

// --- 4. Derived: Invalid Input ---
class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const char* msg);
};

// --- 5. Derived: Slot Unavailable ---
class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const char* msg);
};
