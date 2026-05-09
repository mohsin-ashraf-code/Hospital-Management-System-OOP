#pragma once

class STRING {
private:
    char* data;
    int length;
    int capacity;

    void resize(int newCapacity);

public:
    // Constructors & Destructor
    STRING();
    STRING(const char* str);
    STRING(const STRING& other); // Deep Copy Constructor
    ~STRING();

    // Assignment Operator
    STRING& operator=(const STRING& other); // Deep Copy Assignment

    // Core Functionality
    void copy(const char* str);
    void concatenate(const char* str);

    // Getters
    int getLength() const;
    const char* get() const;
    void clear();
};