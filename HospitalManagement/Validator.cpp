#include "Validator.h"
#include "utility.h"

int Validator::charToInt(const char* str) 
{
    if (!str) 
    {
        return 0;
    }
    int res = 0;
    int i = 0;
    while (*(str + i) != '\0') 
    {
        if (*(str + i) >= '0' && *(str + i) <= '9') 
        {
            res = res * 10 + (*(str + i) - '0');
        }
        i++;
    }
    return res;
}

float Validator::charToFloat(const char* str) 
{
    if (!str) 
    {
        return 0.0f;
    }
    return myAtof(str);
}

bool Validator::isValidID(const char* id) {
    if (!id || myStrLen(id) == 0) return false;
    for (int i = 0; *(id + i) != '\0'; i++) {
        if (*(id + i) < '0' || *(id + i) > '9') return false;
    }
    return charToInt(id) > 0;
}

// Enforces exact DD-MM-YYYY format
bool Validator::isValidDate(const char* date) {
    if (!date || myStrLen(date) != 10) return false;
    if (*(date + 2) != '-' || *(date + 5) != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Skip the dashes
        if (*(date + i) < '0' || *(date + i) > '9') return false;
    }

    int dd = (*(date + 0) - '0') * 10 + (*(date + 1) - '0');
    int mm = (*(date + 3) - '0') * 10 + (*(date + 4) - '0');

    if (mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > 31) return false;
    return true;
}

// Basic check for time slot (e.g. "10:00AM")
bool Validator::isValidTimeSlot(const char* slot) {
    int len = myStrLen(slot);
    if (len < 6 || len > 7) return false;

    char secondToLast = *(slot + len - 2);
    char last = *(slot + len - 1);

    // Must end in AM or PM
    if ((secondToLast != 'A' && secondToLast != 'P') || last != 'M') return false;

    // Must contain a colon
    bool hasColon = false;
    for (int i = 0; i < len - 2; i++) {
        if (*(slot + i) == ':') {
            hasColon = true;
            break;
        }
    }
    return hasColon;
}

// Enforces exactly 11 numeric digits
bool Validator::isValidContact(const char* contact) {
    if (!contact || myStrLen(contact) != 11) return false;
    for (int i = 0; i < 11; i++) {
        if (*(contact + i) < '0' || *(contact + i) > '9') return false;
    }
    return true;
}

bool Validator::isValidPassword(const char* password) {
    if (!password || myStrLen(password) < 4) return false;
    return true;
}

bool Validator::isPositiveFloat(const char* val) {
    if (!val || myStrLen(val) == 0) return false;
    int dots = 0;
    for (int i = 0; *(val + i) != '\0'; i++) {
        if (*(val + i) == '.') {
            dots++;
            if (dots > 1) return false;
        }
        else if (*(val + i) < '0' || *(val + i) > '9') {
            return false;
        }
    }
    return charToFloat(val) >= 0.0f;
}

bool Validator::isValidMenuChoice(const char* choice, int min, int max) {
    if (!isValidID(choice)) return false;
    int c = charToInt(choice);
    return (c >= min && c <= max);
}