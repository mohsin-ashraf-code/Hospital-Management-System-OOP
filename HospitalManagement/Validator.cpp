#include "Validator.h"
#include "utility.h"

using namespace std;

// --- 1. Date Validation (DD-MM-YYYY) ---
bool Validator::isValidDate(const char* date) {
    if (date == nullptr || myStrLen(date) != 10) return false;

    // Check dashes
    if (*(date + 2) != '-' || *(date + 5) != '-') return false;

    // Verify all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (*(date + i) < '0' || *(date + i) > '9') return false;
    }

    // Extract numbers manually
    int dd = (*(date + 0) - '0') * 10 + (*(date + 1) - '0');
    int mm = (*(date + 3) - '0') * 10 + (*(date + 4) - '0');
    int yyyy = (*(date + 6) - '0') * 1000 + (*(date + 7) - '0') * 100 + (*(date + 8) - '0') * 10 + (*(date + 9) - '0');

    // Basic range checks (Assuming current year is 2026 based on system time)
    if (yyyy < 2026) return false;
    if (mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > 31) return false;

    // Optional: Strict day checks for specific months
    if ((mm == 4 || mm == 6 || mm == 9 || mm == 11) && dd > 30) return false;
    if (mm == 2) {
        bool isLeap = (yyyy % 4 == 0 && yyyy % 100 != 0) || (yyyy % 400 == 0);
        if (dd > (isLeap ? 29 : 28)) return false;
    }

    return true;
}

// --- 2. Time Slot Validation ---
bool Validator::isValidTimeSlot(const char* slot) {
    if (slot == nullptr) return false;

    // Define the 8 fixed clinic slots (HH:MM format fits in 6 chars with \0)
    const char* validSlots[8] = {
        "09:00", "10:00", "11:00", "12:00",
        "14:00", "15:00", "16:00", "17:00"
    };

    for (int i = 0; i < 8; i++) {
        if (myStrEqual(slot, validSlots[i])) {
            return true;
        }
    }
    return false;
}

// --- 3. Contact Validation ---
bool Validator::isValidContact(const char* contact) {
    if (contact == nullptr || myStrLen(contact) != 11) return false;

    for (int i = 0; i < 11; i++) {
        if (*(contact + i) < '0' || *(contact + i) > '9') {
            return false;
        }
    }
    return true;
}

// --- 4. Password Validation ---
bool Validator::isValidPassword(const char* pass) {
    if (pass == nullptr) return false;
    return myStrLen(pass) >= 6;
}

// --- 5. Positive Float Validation (for fees and balances) ---
bool Validator::isPositiveFloat(const char* str) {
    if (str == nullptr || myStrLen(str) == 0) return false;

    int dots = 0;
    int i = 0;
    while (*(str + i) != '\0') {
        if (*(str + i) == '.') {
            dots++;
        }
        else if (*(str + i) < '0' || *(str + i) > '9') {
            return false; // Contains invalid characters (like letters)
        }
        i++;
    }

    if (dots > 1) return false; // e.g., "15.00.5" is invalid

    float val = charToFloat(str);
    return val > 0.0f;
}

// --- 6. Menu Choice Validation ---
bool Validator::isValidMenuChoice(int choice, int min, int max) {
    return (choice >= min && choice <= max);
}

// --- 7. Manual String to Integer ---
int Validator::charToInt(const char* str) {
    if (str == nullptr) return 0;
    int res = 0;
    int i = 0;
    while (*(str + i) != '\0') {
        if (*(str + i) >= '0' && *(str + i) <= '9') {
            res = res * 10 + (*(str + i) - '0');
        }
        i++;
    }
    return res;
}

// --- 8. Manual String to Float ---
float Validator::charToFloat(const char* str) {
    if (str == nullptr) return 0.0f;
    float res = 0.0f;
    float fraction = 1.0f;
    bool decimal = false;
    int i = 0;

    while (*(str + i) != '\0') {
        if (*(str + i) == '.') {
            decimal = true;
        }
        else if (*(str + i) >= '0' && *(str + i) <= '9') {
            if (!decimal) {
                res = res * 10.0f + (*(str + i) - '0');
            }
            else {
                fraction *= 0.1f;
                res = res + (*(str + i) - '0') * fraction;
            }
        }
        i++;
    }
    return res;
}