#pragma once

class Validator {
public:
    static bool isValidDate(const char* date);       // DD-MM-YYYY, year >= current
    static bool isValidTimeSlot(const char* slot);   // one of 8 fixed slots
    static bool isValidContact(const char* contact); // exactly 11 digits
    static bool isValidPassword(const char* pass);   // min 6 chars
    static bool isPositiveFloat(const char* str);    // > 0
    static bool isValidMenuChoice(int choice, int min, int max);

    static int  charToInt(const char* str);          // manual atoi
    static float charToFloat(const char* str);       // manual atof
};
