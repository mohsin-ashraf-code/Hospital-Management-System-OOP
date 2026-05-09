#pragma once

class Validator {
public:
    // Core Conversions
    static int charToInt(const char* str);
    static float charToFloat(const char* str);

    // Rubric-Mandated Validations
    static bool isValidID(const char* id);
    static bool isValidDate(const char* date);           // Enforces DD-MM-YYYY
    static bool isValidTimeSlot(const char* slot);       // Enforces time formats (e.g., 10:00AM)
    static bool isValidContact(const char* contact);     // Enforces exactly 11 digits
    static bool isValidPassword(const char* password);   // Enforces minimum length
    static bool isPositiveFloat(const char* val);        // Validates floats/balances
    static bool isValidMenuChoice(const char* choice, int min, int max);
};