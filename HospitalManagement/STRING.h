#pragma once

class STRING
{
private:
    char* data;
    int length;
    int capacity;

    void resize(int newCapacity);

public:
    STRING();
    STRING(const char* str);
    STRING(const STRING& other);
    ~STRING();

    STRING& operator=(const STRING& other);

    void copy(const char* str);
    void concatenate(const char* str);
    void concatenate(char c);

    int getLength() const;
    const char* get() const;
    void clear();
};