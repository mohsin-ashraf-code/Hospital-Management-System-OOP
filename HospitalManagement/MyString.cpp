#include "MyString.h"
#include "utility.h" 

MyString::MyString() 
{
    len = 0;
    arr = new char[1];
    *arr = '\0';
}

MyString::MyString(const char* str) 
{
    if (str == nullptr) 
    {
        len = 0;
        arr = new char[1];
        *arr = '\0';
        return;
    }

    len = myStrLen(str);
    arr = new char[len + 1];
    myStrCopy(arr, str);
}

MyString::MyString(const MyString& other) 
{
    len = other.len;
    arr = new char[len + 1];
    myStrCopy(arr, other.arr); 
}

MyString& MyString::operator=(const MyString& other) 
{
    if (this != &other) 
    {
        delete[] arr;
        len = other.len;
        arr = new char[len + 1];
        myStrCopy(arr, other.arr); 
    }
    return *this;
}

const char* MyString::getData() const 
{ 
    return arr; 
}

int MyString::getLength() const 
{
    return len;
}

void MyString::toLowerCase() 
{
    for (int i = 0; i < len; i++) 
    {
        char c = *(arr + i);
        if (c >= 'A' && c <= 'Z') 
        {
            *(arr + i) = c + 32;
        }
    }
}

bool MyString::isEqual(const char* other) const {
    return myStrEqual(arr, other); // Reduced 15 lines of code to ONE line using utils!
}

MyString::~MyString() {
    delete[] arr;
}