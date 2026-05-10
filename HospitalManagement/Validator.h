#pragma once

class Validator 
{
public:
    static int myStrLen(const char* str);
    static bool myStrEqual(const char* str1, const char* str2);

    static bool isValidDate(const char* date);
    static bool isValidTimeSlot(const char* slot);
    static bool isValidContact(const char* contact);
    static bool isValidPassword(const char* pass);
    static bool isPositiveFloat(const char* str);
    static bool isValidMenuChoice(int choice, int minVal, int maxVal);

    static int charToInt(const char* str);
    static float charToFloat(const char* str);
};