#pragma once

// Basic String Operations
int myStrLen(const char* str);
void myStrCopy(char* dest, const char* src);
bool myStrEqual(const char* str1, const char* str2);

// Time
void getCurrentTimestamp(char* buffer, int maxLen);

// Custom Parsers (Replacing cstring/cstdlib)
char* myStrtok(char* str, char delim, char** context);
float myAtof(const char* str);

// Custom Converters (Replacing sprintf/to_string)
void myIntToStr(int val, char* buf);
void myFloatToStr(float val, char* buf);

bool myCaseInsensitiveEqual(const char* str1, const char* str2);