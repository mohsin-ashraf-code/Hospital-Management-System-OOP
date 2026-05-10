#pragma once
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "ScreenManager.h"

struct AppState
{
    AppState()
    {
        loggedInRole = new char[20];
        *(loggedInRole + 0) = '\0';
    }

    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Admin* admin = nullptr;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;

    ScreenType currentScreen = ScreenType::Login;
    int loggedInUserId = -1;
    char* loggedInRole = nullptr;

    ~AppState()
    {
        delete[] loggedInRole;
    }
};