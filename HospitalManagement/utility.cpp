#define _CRT_SECURE_NO_WARNINGS //Needed for Visual Studio to not give LocalTime Error
#include "utility.h"
#include <ctime>

// ==========================================
// BASIC STRING OPERATIONS (Pointer Arithmetic)
// ==========================================
int myStrLen(const char* str) {
    if (!str) return 0;
    int len = 0;
    while (*(str + len) != '\0') len++;
    return len;
}

void myStrCopy(char* dest, const char* src) {
    if (!dest || !src) return;
    int i = 0;
    while (*(src + i) != '\0') {
        *(dest + i) = *(src + i);
        i++;
    }
    *(dest + i) = '\0';
}

bool myStrEqual(const char* str1, const char* str2) {
    if (!str1 || !str2) return false;
    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0') {
        if (*(str1 + i) != *(str2 + i)) return false;
        i++;
    }
    return *(str1 + i) == *(str2 + i);
}

// ==========================================
// CUSTOM PARSERS
// ==========================================
char* myStrtok(char* str, char delim, char** context) {
    if (!str) str = *context;
    if (!str) return nullptr;

    while (*str == delim) str++;
    if (*str == '\0') return nullptr;

    char* tokenStart = str;
    while (*str != '\0' && *str != delim) str++;

    if (*str == delim) {
        *str = '\0';
        *context = str + 1;
    }
    else {
        *context = nullptr;
    }
    return tokenStart;
}

float myAtof(const char* str) {
    if (!str) return 0.0f;
    float res = 0.0f;
    float fraction = 1.0f;
    bool isNegative = false;
    bool hasDecimal = false;
    int i = 0;

    if (*(str + i) == '-') {
        isNegative = true;
        i++;
    }

    while (*(str + i) != '\0') {
        if (*(str + i) == '.') {
            hasDecimal = true;
        }
        else if (*(str + i) >= '0' && *(str + i) <= '9') {
            if (hasDecimal) {
                fraction *= 0.1f;
                res += (*(str + i) - '0') * fraction;
            }
            else {
                res = res * 10.0f + (*(str + i) - '0');
            }
        }
        i++;
    }
    return isNegative ? -res : res;
}

bool myCaseInsensitiveEqual(const char* str1, const char* str2)
{
    if (!str1 || !str2) return false;

    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0') 
    {
        char c1 = *(str1 + i);
        char c2 = *(str2 + i);

        if (c1 >= 'A' && c1 <= 'Z') 
        {
            c1 += 32;
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 32;
        }

        if (c1 != c2) {
            return false;
        }
        i++;
    }

    // Ensure both strings end at the exact same length
    return *(str1 + i) == '\0' && *(str2 + i) == '\0';
}

// ==========================================
// CUSTOM CONVERTERS
// ==========================================
void myIntToStr(int val, char* buf) {
    if (val == 0) {
        *(buf + 0) = '0';
        *(buf + 1) = '\0';
        return;
    }

    int i = 0;
    bool isNegative = false;

    if (val < 0) {
        isNegative = true;
        val = -val;
    }

    char* tmp = new char[16];
    while (val > 0) {
        *(tmp + i++) = '0' + (val % 10);
        val /= 10;
    }

    int j = 0;
    if (isNegative) {
        *(buf + j++) = '-';
    }

    while (i > 0) {
        *(buf + j++) = *(tmp + --i);
    }
    *(buf + j) = '\0';
    delete[] tmp;
}

void myFloatToStr(float val, char* buf) {
    bool isNegative = (val < 0);
    if (isNegative) val = -val;

    int whole = (int)val;
    int frac = (int)((val - whole) * 100 + 0.5f);

    char* tmpWhole = new char[16];
    myIntToStr(whole, tmpWhole);

    int i = 0;
    if (isNegative) *(buf + i++) = '-';

    int j = 0;
    while (*(tmpWhole + j) != '\0') {
        *(buf + i++) = *(tmpWhole + j++);
    }

    *(buf + i++) = '.';

    if (frac < 10) {
        *(buf + i++) = '0';
        *(buf + i++) = '0' + frac;
    }
    else {
        *(buf + i++) = '0' + (frac / 10);
        *(buf + i++) = '0' + (frac % 10);
    }
    *(buf + i) = '\0';
    delete[] tmpWhole;
}

// ==========================================
// TIME & LOGGING
// ==========================================
void getCurrentTimestamp(char* buffer, int maxLen) {
    time_t now = time(0);

    // Clean, standard, universal C++. No #if or #else needed!
    struct tm* tstruct = localtime(&now);

    int year = tstruct->tm_year + 1900;
    int month = tstruct->tm_mon + 1;
    int day = tstruct->tm_mday;
    int hour = tstruct->tm_hour;
    int min = tstruct->tm_min;
    int sec = tstruct->tm_sec;

    // (The rest of the pointer arithmetic formatting remains exactly the same!)
    int i = 0;
    *(buffer + i++) = '0' + (year / 1000);
    *(buffer + i++) = '0' + ((year / 100) % 10);
    *(buffer + i++) = '0' + ((year / 10) % 10);
    *(buffer + i++) = '0' + (year % 10);
    *(buffer + i++) = '-';

    *(buffer + i++) = '0' + (month / 10);
    *(buffer + i++) = '0' + (month % 10);
    *(buffer + i++) = '-';

    *(buffer + i++) = '0' + (day / 10);
    *(buffer + i++) = '0' + (day % 10);
    *(buffer + i++) = ' ';

    *(buffer + i++) = '0' + (hour / 10);
    *(buffer + i++) = '0' + (hour % 10);
    *(buffer + i++) = ':';

    *(buffer + i++) = '0' + (min / 10);
    *(buffer + i++) = '0' + (min % 10);
    *(buffer + i++) = ':';

    *(buffer + i++) = '0' + (sec / 10);
    *(buffer + i++) = '0' + (sec % 10);

    *(buffer + i) = '\0';
}