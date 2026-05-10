#pragma once
// SFML String Helpers
#include <SFML/Graphics.hpp>

// Basic String Operations (Rule XIV: Pointer Arithmetic Only)
int myStrLen(const char* str); //
void myStrCopy(char* dest, const char* src); //
bool myStrEqual(const char* str1, const char* str2); //

// Time & Timestamping
void getCurrentTimestamp(char* buffer, int maxLen); //

// Custom Parsers (Replacing cstring/cstdlib)
float myAtof(const char* str); //

// Custom Converters (Replacing sprintf/to_string)
void myIntToStr(int val, char* buf); //
void myFloatToStr(float val, char* buf); //

sf::String intToStr(int num);
sf::String floatToStr(float num);