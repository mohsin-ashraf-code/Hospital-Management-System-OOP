#include "Validator.h"

int Validator::myStrLen(const char* str) {
    if (!str) return 0;
    int len = 0;
    while (*(str + len) != '\0') len++;
    return len;
}

bool Validator::myStrEqual(const char* str1, const char* str2) {
    if (!str1 || !str2) return false;
    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0') {
        if (*(str1 + i) != *(str2 + i)) return false;
        i++;
    }
    return (*(str1 + i) == '\0' && *(str2 + i) == '\0');
}

bool Validator::isValidDate(const char* date) {
    if (!date || myStrLen(date) != 10) return false;
    if (*(date + 2) != '-' || *(date + 5) != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (*(date + i) < '0' || *(date + i) > '9') return false;
    }
    int dd = (*(date + 0) - '0') * 10 + (*(date + 1) - '0');
    int mm = (*(date + 3) - '0') * 10 + (*(date + 4) - '0');
    int yyyy = (*(date + 6) - '0') * 1000 + (*(date + 7) - '0') * 100 + (*(date + 8) - '0') * 10 + (*(date + 9) - '0');
    if (yyyy < 2026 || mm < 1 || mm > 12 || dd < 1 || dd > 31) return false;
    return true;
}

bool Validator::isValidTimeSlot(const char* slot) {
    if (!slot) return false;
    const char* validSlots[8] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
    for (int i = 0; i < 8; i++) {
        if (myStrEqual(slot, *(validSlots + i))) return true;
    }
    return false;
}

bool Validator::isValidContact(const char* contact) {
    if (!contact || myStrLen(contact) != 11) return false;
    for (int i = 0; i < 11; i++) {
        if (*(contact + i) < '0' || *(contact + i) > '9') return false;
    }
    return true;
}

bool Validator::isValidPassword(const char* pass) {
    return pass && myStrLen(pass) >= 6;
}

bool Validator::isPositiveFloat(const char* str) {
    if (!str || myStrLen(str) == 0) return false;
    int dots = 0;
    int i = 0;
    while (*(str + i) != '\0') {
        if (*(str + i) == '.') dots++;
        else if (*(str + i) < '0' || *(str + i) > '9') return false;
        i++;
    }
    return (dots <= 1 && charToFloat(str) > 0.0f);
}

bool Validator::isValidMenuChoice(int choice, int minVal, int maxVal) {
    return (choice >= minVal && choice <= maxVal);
}

int Validator::charToInt(const char* str) {
    if (!str) return 0;
    int res = 0, i = 0;
    while (*(str + i) != '\0') {
        if (*(str + i) >= '0' && *(str + i) <= '9') {
            res = res * 10 + (*(str + i) - '0');
        }
        i++;
    }
    return res;
}

float Validator::charToFloat(const char* str) {
    if (!str) return 0.0f;
    float res = 0.0f, fraction = 1.0f;
    bool decimal = false;
    int i = 0;
    while (*(str + i) != '\0') {
        if (*(str + i) == '.') decimal = true;
        else if (*(str + i) >= '0' && *(str + i) <= '9') {
            if (!decimal) res = res * 10.0f + (*(str + i) - '0');
            else {
                fraction *= 0.1f;
                res += (*(str + i) - '0') * fraction;
            }
        }
        i++;
    }
    return res;
}