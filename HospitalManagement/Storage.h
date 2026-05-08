#pragma once
#include <iostream>

template <typename T>
class Storage 
{
private:
    // Project.pdf explicitly mandates an internal static array of size 100
    T data[100];
    int count;

public:
    // Constructor
    Storage() 
    {
        count = 0;
    }

    // --- 1. Add Method ---
    // Uses the Assignment Operator (Rule of Three) we built to safely copy the object
    bool add(const T& item) 
    {
        if (count < 100) 
        {
            data[count] = item;
            count++;
            return true;
        }
        return false; // Storage is full
    }

    // --- 2. Find By ID ---
    // Returns a pointer so we can modify the original object (e.g., updating a patient's balance)
    T* findByID(int id) 
    {
        for (int i = 0; i < count; i++) 
        {
            // This works perfectly because we overloaded the == operator in every class!
            if (data[i] == id) 
            {
                return &data[i];
            }
        }
        return nullptr; // Not found
    }

    // --- 3. Remove By ID ---[cite: 1]
    bool removeByID(int id) 
    {
        for (int i = 0; i < count; i++) 
        {
            if (data[i] == id) 
            {
                // If found, shift all subsequent elements one spot to the left to fill the gap
                for (int j = i; j < count - 1; j++) 
                {
                    data[j] = data[j + 1]; // Relies on our custom Assignment Operators
                }
                count--;
                return true; // Successfully removed
            }
        }
        return false; // ID not found
    }

    // --- 4. Get All ---
     // Returns a pointer to the start of the array so your GUI can loop through it
    T* getAll()
    {
        return data;
    }

    // --- 5. Size ---
    int size() const
    {
        return count;
    }

    int getSize() const {
        return count;
    }

    // --- 6. Get At Index ---
    // Returns a pointer to the object at a specific array index
    T* getAt(int index)
    {
        if (index >= 0 && index < count)
        {
            return &data[index];
        }
        return nullptr;
    }
};