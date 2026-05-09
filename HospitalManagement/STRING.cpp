#include "STRING.h"

// Private Resize Helper
void STRING::resize(int newCapacity) {
    if (newCapacity > capacity) {
        char* newData = new char[newCapacity];

        int i = 0;
        while (i <= length) {
            *(newData + i) = *(data + i);
            i++;
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
}

// Default Constructor
STRING::STRING() {
    capacity = 16;
    length = 0;
    data = new char[capacity];
    *(data + 0) = '\0';
}

// Parameterized Constructor
STRING::STRING(const char* str) {
    capacity = 16;
    length = 0;

    if (str != nullptr) {
        int tempLen = 0;
        while (*(str + tempLen) != '\0') {
            tempLen++;
        }

        capacity = tempLen + 16;
        data = new char[capacity];

        int i = 0;
        while (i < tempLen) {
            *(data + i) = *(str + i);
            i++;
        }
        *(data + tempLen) = '\0';
        length = tempLen;
    }
    else {
        data = new char[capacity];
        *(data + 0) = '\0';
    }
}

// Copy Constructor (Deep Copy)
STRING::STRING(const STRING& other) {
    capacity = other.capacity;
    length = other.length;
    data = new char[capacity];

    int i = 0;
    while (i <= length) {
        *(data + i) = *(other.data + i);
        i++;
    }
}

// Destructor
STRING::~STRING() {
    if (data != nullptr) {
        delete[] data;
        data = nullptr;
    }
}

// Assignment Operator (Deep Copy)
STRING& STRING::operator=(const STRING& other) {
    if (this != &other) {
        delete[] data;

        capacity = other.capacity;
        length = other.length;
        data = new char[capacity];

        int i = 0;
        while (i <= length) {
            *(data + i) = *(other.data + i);
            i++;
        }
    }
    return *this;
}

// Copy from const char*
void STRING::copy(const char* str) {
    if (str == nullptr) {
        clear();
        return;
    }

    int strLen = 0;
    while (*(str + strLen) != '\0') {
        strLen++;
    }

    if (strLen + 1 > capacity) {
        resize(strLen + 16);
    }

    int i = 0;
    while (i < strLen) {
        *(data + i) = *(str + i);
        i++;
    }
    *(data + strLen) = '\0';
    length = strLen;
}

// Concatenate
void STRING::concatenate(const char* str) {
    if (str == nullptr) {
        return;
    }

    int strLen = 0;
    while (*(str + strLen) != '\0') {
        strLen++;
    }

    if (length + strLen + 1 > capacity) {
        resize(length + strLen + 16);
    }

    int i = 0;
    while (i < strLen) {
        *(data + length + i) = *(str + i);
        i++;
    }

    length = length + strLen;
    *(data + length) = '\0';
}

// Get Length
int STRING::getLength() const {
    return length;
}

// Get Raw Pointer
const char* STRING::get() const {
    return data;
}

// Clear String
void STRING::clear() {
    length = 0;
    *(data + 0) = '\0';
}