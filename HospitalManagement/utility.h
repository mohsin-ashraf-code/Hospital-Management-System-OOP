#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

int myStrLen(const char* str);
void myStrCopy(char* dest, const char* src);
bool myStrEqual(const char* str1, const char* str2);

void getCurrentTimestamp(char* buffer, int maxLen);

float myAtof(const char* str);

void myIntToStr(int val, char* buf);

void myFloatToStr(float val, char* buf);

String intToStr(int num);
String floatToStr(float num);