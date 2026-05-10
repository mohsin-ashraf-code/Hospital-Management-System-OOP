#define _CRT_SECURE_NO_WARNINGS
#include "DoctorScreen.h"
#include "Validator.h"
#include "FileHandler.h"
#include "utility.h"
#include <ctime>

using namespace sf;

DoctorScreen::DoctorScreen() : state(nullptr), title(nullptr), specText(nullptr), buttons(nullptr), prescStep(PrescStep::None), activePrescApptId(-1) 
{

}

int DoctorScreen::parseDateToCompare(const char* dateStr) 
{
    if (!dateStr || Validator::myStrLen(dateStr) < 10)
    {
        return 0;
    }
    int d = (*(dateStr + 0) - '0') * 10 + (*(dateStr + 1) - '0');
    int m = (*(dateStr + 3) - '0') * 10 + (*(dateStr + 4) - '0');
    int y = (*(dateStr + 6) - '0') * 1000 + (*(dateStr + 7) - '0') * 100 + (*(dateStr + 8) - '0') * 10 + (*(dateStr + 9) - '0');
    return (y * 10000) + (m * 100) + d;
}

void DoctorScreen::init(const Font& font, AppState& appState) 
{
    state = &appState;

    title = new Text(font);
    title->setCharacterSize(52);
    title->setFillColor(Color::Black);

    specText = new Text(font);
    specText->setCharacterSize(26);
    specText->setFillColor(Color(70, 130, 180));

    const char* labels[] = 
    {
        "1. View Today's Appointments", 
        "2. Mark Complete", 
        "3. Mark No-Show",
        "4. Write Prescription", 
        "5. View Patient History", 
        "6. Logout"
    };

    float btnWidth = 500.f;
    float btnHeight = 75.f;
    float gapY = 40.f;

    buttons = new Button * [6];
    for (int i = 0; i < 6; i++) 
    {
        Color idle = (i == 5) ? Color(165, 55, 55) : Color(45, 95, 145);
        Color hover = (i == 5) ? Color(200, 75, 75) : Color(60, 120, 180);

        float xPos = (i < 3) ? 410.f : 1010.f;
        float yPos = 380.f + ((i % 3) * (btnHeight + gapY));

        *(buttons + i) = new Button({ btnWidth, btnHeight }, { xPos, yPos }, *(labels + i), font, idle, hover);
    }

    dataViewer.init(font, "Doctor Clinical Desk");

    const char* apptFields[] = { "Enter Appointment ID" };
    completeForm.init(font, "Conclude Active Case", apptFields, 1);
    noShowForm.init(font, "Register Attendance Failure (No-Show)", apptFields, 1);
    prescVerifyForm.init(font, "Step 1: Check Treatment ID", apptFields, 1);

    const char* rxFields[] = { "Medicines (Meds Dosage)", "Clinical Directives (Notes)" };
    prescDetailsForm.init(font, "Step 2: Generate Treatment Prescription", rxFields, 2);

    const char* historyFields[] = { "Enter Patient ID" };
    historyForm.init(font, "Access Medical File History", historyFields, 1);
}

void DoctorScreen::handleEvent(const Event& event, RenderWindow& window, AppState& appState) 
{
    if (dataViewer.isActive()) 
    {
        dataViewer.handleEvent(event, window);
        return;
    }

    char timeBuf[64];
    time_t raw = time(nullptr);
    struct tm* tinfo = localtime(&raw);
    strftime(timeBuf, sizeof(timeBuf), "%d-%m-%Y", tinfo);

    if (completeForm.isActive()) 
    {
        completeForm.handleEvent(event, window, [&](const char** formData) 
        {
            int aid = Validator::charToInt(*(formData + 0));
            Appointment* a = state->appointments.findByID(aid);

            if (!a || a->getDoctorId() != state->loggedInUserId || !Validator::myStrEqual(a->getStatus(), "pending") || !Validator::myStrEqual(a->getDate(), timeBuf)) 
            {
                dataViewer.show("Error: ID mismatch or appointment is not scheduled for today.");
                return;
            }

            a->setStatus("completed");
            FileHandler::saveAllAppointments(state->appointments);
            dataViewer.show("Clinical Session closed successfully. Status: [completed]");
        });
        return;
    }

    if (noShowForm.isActive()) 
    {
        noShowForm.handleEvent(event, window, [&](const char** formData) 
        {
            int aid = Validator::charToInt(*(formData + 0));
            Appointment* a = state->appointments.findByID(aid);

            if (!a || a->getDoctorId() != state->loggedInUserId || !Validator::myStrEqual(a->getStatus(), "pending") || !Validator::myStrEqual(a->getDate(), timeBuf)) 
            {
                dataViewer.show("Error: Appointment must belong to you, be pending, and dated today.");
                return;
            }

            a->setStatus("noshow");

            for (int i = 0; i < state->bills.getSize(); i++) 
            {
                Bill* b = state->bills.getAt(i);
                if (b->getAppointmentId() == aid)
                {
                    b->setStatus("cancelled");
                    break;
                }
            }

            FileHandler::saveAllAppointments(state->appointments);
            FileHandler::saveAllBills(state->bills);
            dataViewer.show("Marked as No-Show. Retention fee captured, invoice cancelled.");
        });
        return;
    }

    if (prescVerifyForm.isActive()) 
    {
        prescVerifyForm.handleEvent(event, window, [&](const char** formData) 
        {
            int aid = Validator::charToInt(*(formData + 0));
            Appointment* a = state->appointments.findByID(aid);

            if (!a || a->getDoctorId() != state->loggedInUserId || !Validator::myStrEqual(a->getStatus(), "completed")) 
            {
                dataViewer.show("Verification failed: session must be completed.");
                return;
            }

            for (int i = 0; i < state->prescriptions.getSize(); i++) 
            {
                if (state->prescriptions.getAt(i)->getAppointmentId() == aid) 
                {
                    dataViewer.show("Prescription already written for this appointment.");
                    return;
                }
            }

            activePrescApptId = aid;
            prescStep = PrescStep::Step2Form;
            prescDetailsForm.show();
        });
        return;
    }

    if (prescDetailsForm.isActive()) 
    {
        prescDetailsForm.handleEvent(event, window, [&](const char** formData) 
        {
            const char* medsStr = *(formData + 0);
            const char* notesStr = *(formData + 1);

            Appointment* a = state->appointments.findByID(activePrescApptId);
            int newPid = state->prescriptions.getSize() > 0 ? state->prescriptions.getAt(state->prescriptions.getSize() - 1)->getId() + 1 : 1;

            Prescription newRx(newPid, a->getId(), a->getPatientId(), a->getDoctorId(), a->getDate(), medsStr, notesStr);
            state->prescriptions.add(newRx);

            FileHandler::appendPrescription(newRx);
            dataViewer.show("Prescription saved successfully.");
            prescStep = PrescStep::None;
        });
        return;
    }

    if (historyForm.isActive()) 
    {
        historyForm.handleEvent(event, window, [&](const char** formData) 
        {
            int pid = Validator::charToInt(*(formData + 0));

            bool verified = false;
            for (int i = 0; i < state->appointments.getSize(); i++) 
            {
                Appointment* a = state->appointments.getAt(i);
                if (a->getPatientId() == pid && a->getDoctorId() == state->loggedInUserId && Validator::myStrEqual(a->getStatus(), "completed")) 
                {
                    verified = true;
                    break;
                }
            }

            if (!verified) 
            {
                dataViewer.show("Access denied. You can only view records of your own patients.");
                return;
            }

            int count = 0;
            Prescription** myRx = new Prescription * [state->prescriptions.getSize()];
            for (int i = 0; i < state->prescriptions.getSize(); i++) 
            {
                Prescription* p = state->prescriptions.getAt(i);
                if (p->getPatientId() == pid && p->getDoctorId() == state->loggedInUserId) 
                {
                    *(myRx + count) = p;
                    count++;
                }
            }

            if (count == 0) 
            {
                dataViewer.show("No records found.");
                delete[] myRx;
                return;
            }

            for (int j = 0; j < count - 1; j++) 
            {
                for (int k = 0; k < count - j - 1; k++) 
                {
                    if (parseDateToCompare((*(myRx + k))->getDate()) < parseDateToCompare((*(myRx + k + 1))->getDate())) 
                    {
                        Prescription* temp = *(myRx + k);
                        *(myRx + k) = *(myRx + k + 1);
                        *(myRx + k + 1) = temp;
                    }
                }
            }

            String display = "PATIENT CLINICAL DOSSIER\n";
            for (int j = 0; j < count; j++) 
            {
                display += "Date: " + String((*(myRx + j))->getDate()) + "\n" + "Meds: " + String((*(myRx + j))->getMedicines()) + "\n" + "Notes: " + String((*(myRx + j))->getNotes()) + "\n-----------------------------\n";
            }
            dataViewer.show(display);
            delete[] myRx;
        });
        return;
    }

    for (int i = 0; i < 6; i++) 
    {
        (*(buttons + i))->handleEvent(event, window, [&, i]() 
        {
            if (i == 0) {
                int count = 0;
                Appointment** myAppts = new Appointment * [state->appointments.getSize()];

                for (int j = 0; j < state->appointments.getSize(); j++) 
                {
                    Appointment* a = state->appointments.getAt(j);
                    if (a->getDoctorId() == state->loggedInUserId && Validator::myStrEqual(a->getDate(), timeBuf)) 
                    {
                        *(myAppts + count) = a;
                        count++;
                    }
                }

                if (count == 0)
                {
                    dataViewer.show("No appointments scheduled for today.");
                    delete[] myAppts;
                    return;
                }

                for (int j = 0; j < count - 1; j++) 
                {
                    for (int k = 0; k < count - j - 1; k++) 
                    {
                        const char* s1 = (*(myAppts + k))->getTimeSlot();
                        const char* s2 = (*(myAppts + k + 1))->getTimeSlot();
                        int c = 0;
                        bool swap = false;
                        while (*(s1 + c) != '\0' && *(s2 + c) != '\0') 
                        {
                            if (*(s1 + c) > *(s2 + c)) 
                            { 
                                swap = true; 
                                break; 
                            }
                            else if (*(s1 + c) < *(s2 + c)) 
                            { 
                                break; 
                            }
                            c++;
                        }

                        if (swap) 
                        {
                            Appointment* temp = *(myAppts + k);
                            *(myAppts + k) = *(myAppts + k + 1);
                            *(myAppts + k + 1) = temp;
                        }
                    }
                }

                String display = "SCHEDULED PATIENTS FOR TODAY\n";
                for (int j = 0; j < count; j++) 
                {
                    Patient* p = state->patients.findByID((*(myAppts + j))->getPatientId());
                    display += "ID: " + intToStr((*(myAppts + j))->getId()) + " | Name: " + String(p ? p->getName() : "Unknown") + " | Slot: " + String((*(myAppts + j))->getTimeSlot()) + " | Status: " + String((*(myAppts + j))->getStatus()) + "\n";
                }
                dataViewer.show(display);
                delete[] myAppts;
            }
            else if (i == 1) 
            {
                completeForm.show();
            }
            else if (i == 2) 
            {
                noShowForm.show();
            }
            else if (i == 3) 
            {
                prescStep = PrescStep::Step1Verify;
                prescVerifyForm.show();
            }
            else if (i == 4) 
            {
                historyForm.show();
            }
            else if (i == 5) 
            {
                appState.loggedInUserId = -1;
                appState.currentScreen = ScreenType::Login;
            }
        });
    }
}

void DoctorScreen::update(float dt, RenderWindow& window) 
{
    if (state && state->loggedInUserId != -1) {
        Doctor* d = state->doctors.findByID(state->loggedInUserId);
        if (d) 
        {
            if (title) 
            {
                title->setString("Doctor Hub - Welcome, Dr. " + String(d->getName()));
                // Calculate centering coordinates dynamically
                FloatRect titleBounds = title->getLocalBounds();
                title->setOrigin({ titleBounds.position.x + titleBounds.size.x / 2.0f, titleBounds.position.y + titleBounds.size.y / 2.0f });
                title->setPosition({ 1920.f / 2.0f, 150.f });
            }
            if (specText) 
            {
                specText->setString("Department: " + String(d->getSpecialization()));
                // Calculate centering coordinates dynamically
                FloatRect specBounds = specText->getLocalBounds();
                specText->setOrigin({ specBounds.position.x + specBounds.size.x / 2.0f, specBounds.position.y + specBounds.size.y / 2.0f });
                specText->setPosition({ 1920.f / 2.0f, 240.f });
            }
        }
    }

    if (dataViewer.isActive()) 
    {
        dataViewer.update(window);
    }
    else if (completeForm.isActive())
    {
        completeForm.update(window);
    } 
    else if (noShowForm.isActive())
    {
        noShowForm.update(window);
    } 
    else if (historyForm.isActive())
    {
        historyForm.update(window);
    } 
    else if (prescVerifyForm.isActive())
    {
        prescVerifyForm.update(window);
    } 
    else if (prescDetailsForm.isActive())
    {
        prescDetailsForm.update(window);
    }
    else 
    {
        if (buttons) 
        {
            for (int i = 0; i < 6; i++)
            {
                (*(buttons + i))->update(window);
            }
        }
    }
}

void DoctorScreen::draw(RenderWindow& window) 
{
    if (title) 
    {
        window.draw(*title);
    } 
    if (specText)
    {
        window.draw(*specText);
    }
    if (buttons) 
    {
        for (int i = 0; i < 6; i++)
        {
            (*(buttons + i))->draw(window);
        }
    }

    if (completeForm.isActive())
    {
        completeForm.draw(window);
    } 
    if (noShowForm.isActive())
    {
        noShowForm.draw(window);
    } 
    if (historyForm.isActive())
    {
        historyForm.draw(window);
    } 
    if (prescVerifyForm.isActive())
    {
        prescVerifyForm.draw(window);
    } 
    if (prescDetailsForm.isActive())
    {
        prescDetailsForm.draw(window);
    } 
    if (dataViewer.isActive())
    {
        dataViewer.draw(window);
    } 
}

DoctorScreen::~DoctorScreen()
{
    delete title;
    delete specText;
    if (buttons)
    {
        for (int i = 0; i < 6; i++)
        {
            delete* (buttons + i);
        }
        delete[] buttons;
    }
}