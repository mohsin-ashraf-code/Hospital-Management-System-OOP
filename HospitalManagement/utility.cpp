#include "utility.h"
#include <ctime>

int myStrLen(const char* str)
{
    if (!str) 
    {
        return 0;
    } 
    int len = 0;
    while (*(str + len) != '\0')
    {
        len++;
    } 
    return len;
}

void myStrCopy(char* dest, const char* src)
{
    if (!dest || !src)
    {
        return;
    }

    int i = 0;
    while (*(src + i) != '\0')
    {
        *(dest + i) = *(src + i);
        i++;
    }
    *(dest + i) = '\0';
}

bool myStrEqual(const char* str1, const char* str2)
{
    if (!str1 || !str2)
    {
        return false;
    }

    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0')
    {
        if (*(str1 + i) != *(str2 + i))
        {
            return false;
        }
        i++;
    }
    return *(str1 + i) == *(str2 + i);
}

void getCurrentTimestamp(char* buffer, int maxLen)
{
    time_t now = time(0);
    struct tm ltm;
#ifdef _WIN32
    localtime_s(&ltm, &now);
#else
    ltm = *localtime(&now);
#endif

    int d = ltm.tm_mday;
    int m = 1 + ltm.tm_mon;
    int y = 1900 + ltm.tm_year;

    char* temp = new char[20];
    myIntToStr(d, temp);
    myStrCopy(buffer, temp);
    delete[] temp;
}

float myAtof(const char* str)
{
    if (!str)
    {
        return 0.0f;
    }
    float result = 0.0f;
    float fraction = 1.0f;
    bool inFraction = false;
    int i = 0;

    while (*(str + i) != '\0')
    {
        if (*(str + i) >= '0' && *(str + i) <= '9')
        {
            if (inFraction)
            {
                fraction *= 0.1f;
                result += (*(str + i) - '0') * fraction;
            }
            else
            {
                result = result * 10.0f + (*(str + i) - '0');
            }
        }
        else if (*(str + i) == '.')
        {
            inFraction = true;
        }
        i++;
    }
    return result;
}

void myIntToStr(int val, char* buf)
{
    if (val == 0)
    {
        *(buf + 0) = '0';
        *(buf + 1) = '\0';
        return;
    }
    int i = 0;
    char* tmp = new char[16];
    while (val > 0)
    {
        *(tmp + i++) = '0' + (val % 10);
        val /= 10;
    }
    int j = 0;
    while (i > 0)
    {
        *(buf + j++) = *(tmp + --i);
    }
    *(buf + j) = '\0';
    delete[] tmp;
}

void myFloatToStr(float val, char* buf)
{
    int whole = (int)val;
    int frac = (int)((val - whole) * 100);

    char* wholePart = new char[16];
    char* fracPart = new char[16];
    myIntToStr(whole, wholePart);
    myIntToStr(frac < 0 ? -frac : frac, fracPart);

    myStrCopy(buf, wholePart);
    int len = myStrLen(buf);
    *(buf + len) = '.';
    *(buf + len + 1) = '\0';
    if (frac < 10)
    {
        *(buf + len + 1) = '0';
        *(buf + len + 2) = '\0';
        myStrCopy(buf + len + 2, fracPart);
    }
    else
    {
        myStrCopy(buf + len + 1, fracPart);
    }
    delete[] wholePart;
    delete[] fracPart;
}

String intToStr(int num)
{
    if (num == 0) 
    {
        return "0";
    }
    String res = "";
    bool neg = (num < 0);
    if (neg) 
    {
        num = -num;
    }
    while (num > 0)
    {
        res = String((char)('0' + (num % 10))) + res;
        num /= 10;
    }
    return neg ? "-" + res : res;
}

String floatToStr(float num)
{
    int intPart = static_cast<int>(num);
    int decPart = static_cast<int>((num - intPart) * 100);
    if (decPart < 0)
    {
        decPart = -decPart;
    }
    return intToStr(intPart) + "." + (decPart < 10 ? "0" : "") + intToStr(decPart);
}
