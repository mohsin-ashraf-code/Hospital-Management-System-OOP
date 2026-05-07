#pragma once

class MyString
{
private:
    char* arr;
    int len;

public:
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);
    ~MyString();

    MyString& operator=(const MyString& other);

    const char* getData() const;
    int getLength() const;

    void toLowerCase();
    bool isEqual(const char* other) const;
};