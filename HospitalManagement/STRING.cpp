#include "STRING.h"

void STRING::resize(int newCapacity) 
{
    char* newData = new char[newCapacity];
    for (int i = 0; i < length; i++) 
    {
        *(newData + i) = *(data + i);
    }
    *(newData + length) = '\0';
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

STRING::STRING() 
{
    capacity = 16;
    length = 0;
    data = new char[capacity];
    *(data + 0) = '\0';
}

STRING::STRING(const char* str) 
{
    if (!str) 
    {
        capacity = 16; length = 0; data = new char[capacity]; *(data + 0) = '\0';
        return;
    }
    length = 0;
    while (*(str + length) != '\0') 
    {
        length++;
    }
    capacity = length + 1;
    data = new char[capacity];
    for (int i = 0; i < length; i++) 
    {
        *(data + i) = *(str + i);
    } 
    *(data + length) = '\0';
}

STRING::STRING(const STRING& other) 
{
    length = other.length;
    capacity = other.capacity;
    data = new char[capacity];
    for (int i = 0; i <= length; i++) 
    {
        *(data + i) = *(other.data + i);
    }
}

STRING::~STRING() 
{
    delete[] data;
}

STRING& STRING::operator=(const STRING& other) 
{
    if (this != &other) 
    {
        delete[] data;
        length = other.length;
        capacity = other.capacity;
        data = new char[capacity];
        for (int i = 0; i <= length; i++) 
        {
            *(data + i) = *(other.data + i);
        } 
    }
    return *this;
}

void STRING::copy(const char* str) {
    if (!str) { clear(); return; }
    int newLen = 0;
    while (*(str + newLen) != '\0') newLen++;
    if (newLen >= capacity) resize(newLen + 1);
    length = newLen;
    for (int i = 0; i < length; i++) 
    {
        *(data + i) = *(str + i);
    }
    *(data + length) = '\0';
}

void STRING::concatenate(const char* str) 
{
    if (!str) 
    {
        return;
    }
    int strLen = 0;
    while (*(str + strLen) != '\0') 
    {
        strLen++;
    } 
    if (length + strLen >= capacity) 
    {
        resize(length + strLen + 16);
    }
    for (int i = 0; i < strLen; i++) 
    {
        *(data + length + i) = *(str + i);
    }
    length += strLen;
    *(data + length) = '\0';
}

void STRING::concatenate(char c) 
{
    if (length + 1 >= capacity)   
    {
        resize(capacity + 16);
    }
    *(data + length) = c;
    length++;
    *(data + length) = '\0';
}

int STRING::getLength() const 
{ 
    return length; 
}
const char* STRING::get() const 
{ 
    return data; 
}

void STRING::clear() 
{ 
    length = 0; 
    *(data + 0) = '\0'; 
}