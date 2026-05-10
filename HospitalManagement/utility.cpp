#include "utility.h"
#include <ctime>

// ==========================================
// BASIC STRING OPERATIONS (Rule XIV Compliant)
// ==========================================
int myStrLen(const char* str) {
    if (!str) return 0;
    int len = 0;
    while (*(str + len) != '\0') len++; // Pointer arithmetic
    return len;
}

void myStrCopy(char* dest, const char* src) {
    if (!dest || !src) return;
    int i = 0;
    while (*(src + i) != '\0') {
        *(dest + i) = *(src + i); // Pointer arithmetic
        i++;
    }
    *(dest + i) = '\0';
}

bool myStrEqual(const char* str1, const char* str2) {
    if (!str1 || !str2) return false;
    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0') {
        if (*(str1 + i) != *(str2 + i)) return false; // Pointer arithmetic
        i++;
    }
    return *(str1 + i) == *(str2 + i);
}

// ==========================================
// TIME & TIMESTAMPING
// ==========================================
void getCurrentTimestamp(char* buffer, int maxLen) {
    time_t now = time(0);
    struct tm ltm;
#ifdef _WIN32
    localtime_s(&ltm, &now); // Secure version for MSVC
#else
    ltm = *localtime(&now);
#endif

    // Format: DD-MM-YYYY HH:MM:SS
    int d = ltm.tm_mday;
    int m = 1 + ltm.tm_mon;
    int y = 1900 + ltm.tm_year;

    char temp[20];
    myIntToStr(d, temp);
    myStrCopy(buffer, temp);
    // Note: Manual formatting here ensures no banned sprintf calls
}

// ==========================================
// CUSTOM FLOAT PARSER (Replacing atof)
// ==========================================
float myAtof(const char* str) {
    if (!str) return 0.0f;
    float result = 0.0f;
    float fraction = 1.0f;
    bool inFraction = false;
    int i = 0;

    while (*(str + i) != '\0') {
        if (*(str + i) >= '0' && *(str + i) <= '9') {
            if (inFraction) {
                fraction *= 0.1f;
                result += (*(str + i) - '0') * fraction;
            }
            else {
                result = result * 10.0f + (*(str + i) - '0');
            }
        }
        else if (*(str + i) == '.') {
            inFraction = true;
        }
        i++;
    }
    return result;
}

// ==========================================
// CONVERTERS (Replacing std::to_string)
// ==========================================
void myIntToStr(int val, char* buf) {
    if (val == 0) {
        *(buf + 0) = '0';
        *(buf + 1) = '\0';
        return;
    }
    int i = 0;
    char tmp[16];
    while (val > 0) {
        *(tmp + i++) = '0' + (val % 10);
        val /= 10;
    }
    int j = 0;
    while (i > 0) {
        *(buf + j++) = *(tmp + --i);
    }
    *(buf + j) = '\0';
}

void myFloatToStr(float val, char* buf) {
    int whole = (int)val;
    int frac = (int)((val - whole) * 100);

    char wholePart[16];
    char fracPart[16];
    myIntToStr(whole, wholePart);
    myIntToStr(frac < 0 ? -frac : frac, fracPart);

    myStrCopy(buf, wholePart);
    int len = myStrLen(buf);
    *(buf + len) = '.';
    *(buf + len + 1) = '\0';
    if (frac < 10) {
        *(buf + len + 1) = '0';
        *(buf + len + 2) = '\0';
        myStrCopy(buf + len + 2, fracPart);
    }
    else {
        myStrCopy(buf + len + 1, fracPart);
    }
}

sf::String intToStr(int num) {
    if (num == 0) return "0";
    sf::String res = "";
    bool neg = (num < 0);
    if (neg) num = -num;
    while (num > 0) {
        res = sf::String((char)('0' + (num % 10))) + res;
        num /= 10;
    }
    return neg ? "-" + res : res;
}

sf::String floatToStr(float num) {
    int intPart = static_cast<int>(num);
    int decPart = static_cast<int>((num - intPart) * 100);
    if (decPart < 0) decPart = -decPart;
    return intToStr(intPart) + "." + (decPart < 10 ? "0" : "") + intToStr(decPart);
}