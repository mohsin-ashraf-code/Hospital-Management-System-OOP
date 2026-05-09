#pragma once

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "ScreenManager.h"
#include "STRING.h"

struct AppState {
    // 1. Data Storage (Loaded once in main.cpp)
    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Admin* admin = nullptr;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;

    // 2. Session Variables (Who is currently using the app?)
    ScreenType currentScreen = ScreenType::Login; // Always start at the Login screen
    int loggedInUserId = -1;                      // -1 means nobody is logged in yet
    STRING loggedInRole;
};