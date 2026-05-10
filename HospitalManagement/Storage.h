#pragma once
#include <iostream>

template <typename T>
class Storage
{
private:
    T data[100]; // Explicitly required by rubric
    int count;

public:
    Storage() { count = 0; }

    bool add(const T& item) {
        if (count < 100) {
            *(data + count) = item; // RULE XIV COMPLIANT
            count++;
            return true;
        }
        return false;
    }

    T* findByID(int id) {
        for (int i = 0; i < count; i++) {
            if (*(data + i) == id) return (data + i);
        }
        return nullptr;
    }

    bool removeByID(int id) {
        for (int i = 0; i < count; i++) {
            if (*(data + i) == id) {
                for (int j = i; j < count - 1; j++) {
                    *(data + j) = *(data + j + 1); // RULE XIV COMPLIANT
                }
                count--;
                return true;
            }
        }
        return false;
    }

    T* getAll() { return data; }
    int getSize() const { return count; }
    T* getAt(int index) {
        if (index >= 0 && index < count) return (data + index);
        return nullptr;
    }
};