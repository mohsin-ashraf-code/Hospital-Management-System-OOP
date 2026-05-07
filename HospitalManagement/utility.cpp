// #define _CRT_SECURE_NO_WARNINGS // MUST BE LINE 1! Shuts off MSVC security warnings.

#include "utility.h"
#include <ctime>

// ==========================================
// BASIC STRING OPERATIONS
// ==========================================
int myStrLen(const char* str) {
    if (!str) return 0;
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void myStrCopy(char* dest, const char* src) {
    if (!dest || !src) return;
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

bool myStrEqual(const char* str1, const char* str2) {
    if (!str1 || !str2) return false;
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return str1[i] == str2[i]; // True if both hit '\0' at the same time
}

// ==========================================
// TIME & LOGGING (No <cstdio> allowed)
// ==========================================
void getCurrentTimestamp(char* buffer, int maxLen) {
    time_t now = time(0);
    tm ltm;

    // Microsoft's secure version
    localtime_s(&ltm, &now);

    int index = 0;
    char temp[16];

    // Helper lambda-like logic to append numbers with leading zeros
    auto appendZeroPadded = [&](int value) {
        if (value < 10) buffer[index++] = '0';
        myIntToStr(value, temp);
        for (int i = 0; temp[i] != '\0'; i++) buffer[index++] = temp[i];
        };

    // 1. Day
    appendZeroPadded(ltm.tm_mday);
    buffer[index++] = '-';

    // 2. Month
    appendZeroPadded(1 + ltm.tm_mon);
    buffer[index++] = '-';

    // 3. Year (4 digits, no padding needed)
    myIntToStr(1900 + ltm.tm_year, temp);
    for (int i = 0; temp[i] != '\0'; i++) buffer[index++] = temp[i];
    buffer[index++] = ' ';

    // 4. Hour
    appendZeroPadded(ltm.tm_hour);
    buffer[index++] = ':';

    // 5. Minute
    appendZeroPadded(ltm.tm_min);
    buffer[index++] = ':';

    // 6. Second
    appendZeroPadded(ltm.tm_sec);

    buffer[index] = '\0'; // Null terminate the final string
}

// ==========================================
// CUSTOM STRING TOKENIZER (Replaces strtok)
// ==========================================
char* myStrtok(char* str, char delim, char** context) {
    if (str == nullptr) str = *context;
    if (str == nullptr || *str == '\0') return nullptr;

    char* start = str;

    while (*str != '\0' && *str != delim) {
        // KILL INVISIBLE CHARACTERS: \r or \n
        if (*str == '\r' || *str == '\n') {
            *str = '\0';
            *context = nullptr;
            return start;
        }
        str++;
    }

    if (*str == delim) {
        *str = '\0';
        *context = str + 1;
    }
    else {
        *context = nullptr;
    }

    return start;
}

// ==========================================
// CUSTOM FLOAT PARSER (Replaces atof)
// ==========================================
float myAtof(const char* str) {
    if (!str) return 0.0f;
    float result = 0.0f;
    float fraction = 1.0f;
    bool inFraction = false;
    bool isNegative = (*str == '-');

    if (isNegative) str++;

    while (*str != '\0') {
        if (*str >= '0' && *str <= '9') {
            if (inFraction) {
                fraction *= 0.1f;
                result += (*str - '0') * fraction;
            }
            else {
                result = result * 10.0f + (*str - '0');
            }
        }
        else if (*str == '.') {
            inFraction = true;
        }
        else {
            break;
        }
        str++;
    }
    return isNegative ? -result : result;
}

// ==========================================
// CUSTOM INT TO STRING
// ==========================================
void myIntToStr(int val, char* buf) {
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    int i = 0;
    bool isNegative = false;

    if (val < 0) {
        isNegative = true;
        val = -val;
    }

    char tmp[16];
    while (val > 0) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }

    int j = 0;
    if (isNegative) {
        buf[j++] = '-';
    }

    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

// ==========================================
// CUSTOM FLOAT TO STRING (2 Decimal Places)
// ==========================================
void myFloatToStr(float val, char* buf) {
    bool isNegative = (val < 0);
    if (isNegative) val = -val;

    int whole = (int)val;
    int frac = (int)((val - whole) * 100 + 0.5f);

    if (frac >= 100) {
        whole += 1;
        frac -= 100;
    }

    if (isNegative) {
        buf[0] = '-';
        myIntToStr(whole, buf + 1);
    }
    else {
        myIntToStr(whole, buf);
    }

    int i = 0;
    while (buf[i] != '\0') i++;

    buf[i++] = '.';

    if (frac < 10) {
        buf[i++] = '0';
    }

    char f[8];
    myIntToStr(frac, f);

    int j = 0;
    while (f[j] != '\0') {
        buf[i++] = f[j++];
    }
    buf[i] = '\0';
}