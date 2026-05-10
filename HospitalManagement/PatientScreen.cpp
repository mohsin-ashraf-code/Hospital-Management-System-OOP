#include "PatientScreen.h"
#include "Validator.h"
#include "FileHandler.h"
#include "utility.h"
#include "SlotUnavailableException.h"
#include "InsufficientFundsException.h"
#include "InvalidInputException.h"

using namespace sf;

PatientScreen::PatientScreen() : state(nullptr), title(nullptr), balanceText(nullptr), buttons(nullptr), bookState(BookingState::None), selectedDocId(-1), sessionalTopUpAttempts(0)
{
}

bool PatientScreen::manualCaseInsensitiveMatch(const char* str1, const char* str2)
{
    if (!str1 || !str2)
    {
        return false;
    }
    int i = 0;
    while (*(str1 + i) != '\0' && *(str2 + i) != '\0')
    {
        char c1 = *(str1 + i);
        char c2 = *(str2 + i);
        if (c1 >= 'A' && c1 <= 'Z')
        {
            c1 += 32;
        }
        if (c2 >= 'A' && c2 <= 'Z')
        {
            c2 += 32;
        }
        if (c1 != c2)
        {
            return false;
        }
        i++;
    }
    return (*(str1 + i) == '\0' && *(str2 + i) == '\0');
}

int PatientScreen::parseDateToInt(const char* dateStr)
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

void PatientScreen::init(const Font& font, AppState& appState)
{
    state = &appState;

    title = new Text(font);
    title->setCharacterSize(52);
    title->setFillColor(Color::Black);

    balanceText = new Text(font);
    balanceText->setCharacterSize(26);
    balanceText->setFillColor(Color(0, 150, 0));

    const char* labels[] =
    {
        "1. Book Appointment",
        "2. Cancel Appointment",
        "3. View My Appointments",
        "4. View My Medical Records",
        "5. View My Bills",
        "6. Pay Bill",
        "7. Top Up Balance",
        "8. Logout"
    };

    float btnWidth = 500.f;
    float btnHeight = 75.f;
    float gapY = 35.f;

    buttons = new Button * [8];
    for (int i = 0; i < 8; i++)
    {
        Color idle = (i == 7) ? Color(165, 55, 55) : Color(45, 95, 145);
        Color hover = (i == 7) ? Color(200, 75, 75) : Color(60, 120, 180);

        float xPos = (i < 4) ? 410.f : 1010.f;
        float yPos = 350.f + ((i % 4) * (btnHeight + gapY));

        *(buttons + i) = new Button({ btnWidth, btnHeight }, { xPos, yPos }, *(labels + i), font, idle, hover);
    }

    dataViewer.init(font, "Patient Dashboard Services");

    const char* tFields[] = { "Enter amount (PKR)" };
    topUpForm.init(font, "Top Up Account Balance", tFields, 1);

    const char* pFields[] = { "Enter Bill ID to Pay" };
    payBillForm.init(font, "Settle Unpaid Invoice", pFields, 1);

    const char* cFields[] = { "Enter Appointment ID" };
    cancelApptForm.init(font, "Void Current Booking", cFields, 1);

    const char* s1[] = { "Enter Specialization" };
    searchSpecForm.init(font, "Step 1: Department Sorting", s1, 1);

    const char* s2[] = { "Enter Doctor ID" };
    selectDocForm.init(font, "Step 2: Medical Officer ID", s2, 1);

    const char* s3[] = { "Enter Date (DD-MM-YYYY)" };
    bookDateForm.init(font, "Step 3: Scheduling Calendar", s3, 1);

    const char* s4[] = { "Enter Time (HH:MM)" };
    bookTimeForm.init(font, "Step 4: Session Slot Selection", s4, 1);
}

void PatientScreen::handleEvent(const Event& event, RenderWindow& window, AppState& appState)
{
    if (dataViewer.isActive())
    {
        dataViewer.handleEvent(event, window);
        return;
    }

    if (topUpForm.isActive())
    {
        topUpForm.handleEvent(event, window, [&](const char** formData)
            {
                const char* amtStr = *(formData + 0);
                try
                {
                    if (!Validator::isPositiveFloat(amtStr))
                    {
                        sessionalTopUpAttempts++;
                        if (sessionalTopUpAttempts >= 3)
                        {
                            sessionalTopUpAttempts = 0;
                            throw InvalidInputException("Error: Excessive failed attempts. Session closed.");
                        }
                        throw InvalidInputException("Invalid amount. Please enter a positive number.");
                    }
                    float amt = Validator::charToFloat(amtStr);
                    Patient* p = state->patients.findByID(state->loggedInUserId);
                    if (p)
                    {
                        *p += amt;
                        FileHandler::saveAllPatients(state->patients);
                        sessionalTopUpAttempts = 0;
                        dataViewer.show("Top-Up successful. Balance updated: PKR " + floatToStr(p->getBalance()));
                    }
                }
                catch (const InvalidInputException& e)
                {
                    dataViewer.show(e.what());
                }
            });
        return;
    }

    if (payBillForm.isActive())
    {
        payBillForm.handleEvent(event, window, [&](const char** formData)
            {
                int bid = Validator::charToInt(*(formData + 0));
                Bill* b = state->bills.findByID(bid);
                Patient* p = state->patients.findByID(state->loggedInUserId);

                if (!b || b->getPatientId() != p->getId() || !Validator::myStrEqual(b->getStatus(), "unpaid"))
                {
                    dataViewer.show("Error: Active invoice ID mismatch.");
                    return;
                }
                if (p->getBalance() < b->getAmount())
                {
                    dataViewer.show("Settle invoice failed: Insufficient balance.");
                    return;
                }

                *p -= b->getAmount();
                b->setStatus("paid");

                FileHandler::saveAllPatients(state->patients);
                FileHandler::saveAllBills(state->bills);
                dataViewer.show("Payment complete. Available balance: PKR " + floatToStr(p->getBalance()));
            });
        return;
    }

    if (cancelApptForm.isActive())
    {
        cancelApptForm.handleEvent(event, window, [&](const char** formData)
            {
                int aid = Validator::charToInt(*(formData + 0));
                Appointment* a = state->appointments.findByID(aid);
                Patient* p = state->patients.findByID(state->loggedInUserId);

                if (!a || a->getPatientId() != p->getId() || !Validator::myStrEqual(a->getStatus(), "pending"))
                {
                    dataViewer.show("Error: Invalid active appointment ID mapping.");
                    return;
                }

                a->setStatus("cancelled");

                Doctor* d = state->doctors.findByID(a->getDoctorId());
                if (d)
                {
                    *p += d->getFee();
                }

                for (int i = 0; i < state->bills.getSize(); i++)
                {
                    Bill* b = state->bills.getAt(i);
                    if (b->getAppointmentId() == aid)
                    {
                        b->setStatus("cancelled");
                        break;
                    }
                }

                FileHandler::saveAllPatients(state->patients);
                FileHandler::saveAllAppointments(state->appointments);
                FileHandler::saveAllBills(state->bills);

                dataViewer.show("Session voided. Retainer fee of PKR " + floatToStr(d ? d->getFee() : 0) + " refunded.");
            });
        return;
    }

    if (searchSpecForm.isActive())
    {
        searchSpecForm.handleEvent(event, window, [&](const char** formData)
            {
                String display = " MATCHING SPECIALIST DIRECTORY \n";
                bool found = false;
                for (int i = 0; i < state->doctors.getSize(); i++)
                {
                    Doctor* d = state->doctors.getAt(i);
                    if (manualCaseInsensitiveMatch(d->getSpecialization(), *(formData + 0)))
                    {
                        display += "ID: " + intToStr(d->getId()) + " | Dr. " + String(d->getName()) + " | Fee: " + floatToStr(d->getFee()) + "\n";
                        found = true;
                    }
                }
                if (!found)
                {
                    dataViewer.show("No doctors available for that specialization.");
                }
                else
                {
                    dataViewer.show(display);
                    bookState = BookingState::SelectDoc;
                    selectDocForm.show();
                }
            });
        return;
    }

    if (selectDocForm.isActive())
    {
        selectDocForm.handleEvent(event, window, [&](const char** formData)
            {
                int did = Validator::charToInt(*(formData + 0));
                Doctor* d = state->doctors.findByID(did);
                if (!d)
                {
                    dataViewer.show("Doctor not found.");
                    bookState = BookingState::None;
                }
                else
                {
                    selectedDocId = did;
                    bookState = BookingState::EnterDate;
                    bookDateForm.show();
                }
            });
        return;
    }

    if (bookDateForm.isActive())
    {
        bookDateForm.handleEvent(event, window, [&](const char** formData)
            {
                const char* dStr = *(formData + 0);
                if (!Validator::isValidDate(dStr))
                {
                    dataViewer.show("Invalid date. Use format DD-MM-YYYY (2026 or later).");
                    bookState = BookingState::None;
                }
                else
                {
                    selectedDate.copy(dStr);
                    const char* slots[] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
                    String display = " AVAILABLE SLOTS ON " + String(selectedDate.get()) + " \n";

                    for (int i = 0; i < 8; i++)
                    {
                        bool taken = false;
                        for (int j = 0; j < state->appointments.getSize(); j++)
                        {
                            Appointment* a = state->appointments.getAt(j);
                            if (a->getDoctorId() == selectedDocId && Validator::myStrEqual(a->getDate(), selectedDate.get()) &&
                                Validator::myStrEqual(a->getTimeSlot(), *(slots + i)) && !Validator::myStrEqual(a->getStatus(), "cancelled"))
                            {
                                taken = true;
                                break;
                            }
                        }
                        if (!taken)
                        {
                            display += String(*(slots + i)) + "  ";
                        }
                    }
                    dataViewer.show(display);
                    bookState = BookingState::EnterTime;
                    bookTimeForm.show();
                }
            });
        return;
    }

    if (bookTimeForm.isActive())
    {
        bookTimeForm.handleEvent(event, window, [&](const char** formData)
            {
                const char* tStr = *(formData + 0);
                if (!Validator::isValidTimeSlot(tStr))
                {
                    dataViewer.show("Format mismatch: invalid block marker.");
                    bookState = BookingState::None;
                    return;
                }

                try
                {
                    for (int j = 0; j < state->appointments.getSize(); j++)
                    {
                        Appointment* a = state->appointments.getAt(j);
                        if (a->getDoctorId() == selectedDocId && Validator::myStrEqual(a->getDate(), selectedDate.get()) &&
                            Validator::myStrEqual(a->getTimeSlot(), tStr) && !Validator::myStrEqual(a->getStatus(), "cancelled"))
                        {
                            throw SlotUnavailableException("Slot Unavailable! Please select another time.");
                        }
                    }

                    Patient* p = state->patients.findByID(state->loggedInUserId);
                    Doctor* d = state->doctors.findByID(selectedDocId);

                    if (p->getBalance() < d->getFee())
                    {
                        throw InsufficientFundsException("Insufficient funds to book this appointment.");
                    }

                    *p -= d->getFee();
                    int newApptId = state->appointments.getSize() > 0 ? state->appointments.getAt(state->appointments.getSize() - 1)->getId() + 1 : 1;
                    int newBillId = state->bills.getSize() > 0 ? state->bills.getAt(state->bills.getSize() - 1)->getId() + 1 : 1;

                    Appointment newAppt(newApptId, p->getId(), d->getId(), selectedDate.get(), tStr, "pending");
                    Bill newBill(newBillId, p->getId(), newApptId, d->getFee(), "unpaid", selectedDate.get());

                    state->appointments.add(newAppt);
                    state->bills.add(newBill);

                    FileHandler::saveAllPatients(state->patients);
                    FileHandler::appendAppointment(newAppt);
                    FileHandler::appendBill(newBill);

                    dataViewer.show("Appointment booked successfully! ID: " + intToStr(newApptId));
                    bookState = BookingState::None;
                }
                catch (const SlotUnavailableException& e)
                {
                    dataViewer.show(e.what());
                    bookState = BookingState::None;
                }
                catch (const InsufficientFundsException& e)
                {
                    dataViewer.show(e.what());
                    bookState = BookingState::None;
                }
            });
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        (*(buttons + i))->handleEvent(event, window, [&, i]()
            {
                if (i == 0)
                {
                    bookState = BookingState::SearchSpec;
                    searchSpecForm.show();
                }
                else if (i == 1)
                {
                    String disp = " ACTIVE BOOKINGS \n";
                    int count = 0;
                    for (int j = 0; j < state->appointments.getSize(); j++)
                    {
                        Appointment* a = state->appointments.getAt(j);
                        if (a->getPatientId() == state->loggedInUserId && Validator::myStrEqual(a->getStatus(), "pending"))
                        {
                            Doctor* d = state->doctors.findByID(a->getDoctorId());
                            disp += "ID: " + intToStr(a->getId()) + " | Dr. " + String(d ? d->getName() : "Unknown") + " | " + String(a->getDate()) + " @ " + String(a->getTimeSlot()) + "\n";
                            count++;
                        }
                    }
                    if (count == 0)
                    {
                        dataViewer.show("You have no pending appointments.");
                    }
                    else
                    {
                        dataViewer.show(disp);
                        cancelApptForm.show();
                    }
                }
                else if (i == 2)
                {
                    int count = 0;
                    Appointment** myAppts = new Appointment * [state->appointments.getSize()];
                    for (int j = 0; j < state->appointments.getSize(); j++)
                    {
                        if (state->appointments.getAt(j)->getPatientId() == state->loggedInUserId)
                        {
                            *(myAppts + count) = state->appointments.getAt(j);
                            count++;
                        }
                    }

                    if (count == 0)
                    {
                        dataViewer.show("No appointments found.");
                        delete[] myAppts;
                        return;
                    }

                    for (int j = 0; j < count - 1; j++)
                    {
                        for (int k = 0; k < count - j - 1; k++)
                        {
                            if (parseDateToInt((*(myAppts + k))->getDate()) > parseDateToInt((*(myAppts + k + 1))->getDate()))
                            {
                                Appointment* temp = *(myAppts + k);
                                *(myAppts + k) = *(myAppts + k + 1);
                                *(myAppts + k + 1) = temp;
                            }
                        }
                    }

                    String display = " MY APPOINTMENTS \n";
                    for (int j = 0; j < count; j++)
                    {
                        Doctor* d = state->doctors.findByID((*(myAppts + j))->getDoctorId());
                        display += "ID: " + intToStr((*(myAppts + j))->getId()) + " | Dr. " + String(d ? d->getName() : "Unknown") +
                            " | " + String((*(myAppts + j))->getDate()) + " " + String((*(myAppts + j))->getTimeSlot()) + " [" + String((*(myAppts + j))->getStatus()) + "]\n";
                    }
                    dataViewer.show(display);
                    delete[] myAppts;
                }
                else if (i == 3)
                {
                    int count = 0;
                    Prescription** myPrescs = new Prescription * [state->prescriptions.getSize()];
                    for (int j = 0; j < state->prescriptions.getSize(); j++)
                    {
                        if (state->prescriptions.getAt(j)->getPatientId() == state->loggedInUserId)
                        {
                            *(myPrescs + count) = state->prescriptions.getAt(j);
                            count++;
                        }
                    }

                    if (count == 0)
                    {
                        dataViewer.show("No medical records found.");
                        delete[] myPrescs;
                        return;
                    }

                    for (int j = 0; j < count - 1; j++)
                    {
                        for (int k = 0; k < count - j - 1; k++)
                        {
                            if (parseDateToInt((*(myPrescs + k))->getDate()) < parseDateToInt((*(myPrescs + k + 1))->getDate()))
                            {
                                Prescription* temp = *(myPrescs + k);
                                *(myPrescs + k) = *(myPrescs + k + 1);
                                *(myPrescs + k + 1) = temp;
                            }
                        }
                    }

                    String display = " MEDICAL CASE RECORDS \n";
                    for (int j = 0; j < count; j++)
                    {
                        Doctor* d = state->doctors.findByID((*(myPrescs + j))->getDoctorId());
                        display += "Date: " + String((*(myPrescs + j))->getDate()) + " | Dr. " + String(d ? d->getName() : "Unknown") + "\n" +
                            "Meds: " + String((*(myPrescs + j))->getMedicines()) + "\n" +
                            "Notes: " + String((*(myPrescs + j))->getNotes()) + "\n-\n";
                    }
                    dataViewer.show(display);
                    delete[] myPrescs;
                }
                else if (i == 4)
                {
                    String display = " INVOICES RECORD \n";
                    float outstanding = 0.0f;
                    int count = 0;
                    for (int j = 0; j < state->bills.getSize(); j++)
                    {
                        Bill* b = state->bills.getAt(j);
                        if (b->getPatientId() == state->loggedInUserId)
                        {
                            display += "Bill ID: " + intToStr(b->getId()) + " | Appt ID: " + intToStr(b->getAppointmentId()) + " | Amt: " + floatToStr(b->getAmount()) + " | [" + String(b->getStatus()) + "] | Date: " + String(b->getDate()) + "\n";
                            if (Validator::myStrEqual(b->getStatus(), "unpaid"))
                            {
                                outstanding += b->getAmount();
                            }
                            count++;
                        }
                    }
                    if (count == 0)
                    {
                        dataViewer.show("No bills found.");
                    }
                    else
                    {
                        display += "\nTOTAL OUTSTANDING UNPAID AMOUNT: PKR " + floatToStr(outstanding);
                        dataViewer.show(display);
                    }
                }
                else if (i == 5)
                {
                    String display = " UNPAID INVOICES \n";
                    bool found = false;
                    for (int j = 0; j < state->bills.getSize(); j++) {
                        Bill* b = state->bills.getAt(j);
                        if (b->getPatientId() == state->loggedInUserId && Validator::myStrEqual(b->getStatus(), "unpaid"))
                        {
                            display += "ID: " + intToStr(b->getId()) + " | Amt: " + floatToStr(b->getAmount()) + "\n";
                            found = true;
                        }
                    }
                    if (!found)
                    {
                        dataViewer.show("No unpaid bills.");
                    }
                    else
                    {
                        dataViewer.show(display);
                        payBillForm.show();
                    }
                }
                else if (i == 6)
                {
                    topUpForm.show();
                }
                else if (i == 7)
                {
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
            });
    }
}

void PatientScreen::update(float dt, RenderWindow& window)
{
    if (state && state->loggedInUserId != -1)
    {
        Patient* p = state->patients.findByID(state->loggedInUserId);
        if (p)
        {
            if (title)
            {
                title->setString("Patient Board - Welcome, " + String(p->getName()));
                FloatRect titleBounds = title->getLocalBounds();
                title->setOrigin({ titleBounds.position.x + titleBounds.size.x / 2.0f, titleBounds.position.y + titleBounds.size.y / 2.0f });
                title->setPosition({ 1920.f / 2.0f, 150.f });
            }
            if (balanceText)
            {
                balanceText->setString("Cleared Account Balance: PKR " + floatToStr(p->getBalance()));
                FloatRect balBounds = balanceText->getLocalBounds();
                balanceText->setOrigin({ balBounds.position.x + balBounds.size.x / 2.0f, balBounds.position.y + balBounds.size.y / 2.0f });
                balanceText->setPosition({ 1920.f / 2.0f, 240.f });
            }
        }
    }

    if (dataViewer.isActive())
    {
        dataViewer.update(window);
    }
    else if (topUpForm.isActive())
    {
        topUpForm.update(window);
    }
    else if (payBillForm.isActive())
    {
        payBillForm.update(window);
    }
    else if (cancelApptForm.isActive())
    {
        cancelApptForm.update(window);
    }
    else if (searchSpecForm.isActive())
    {
        searchSpecForm.update(window);
    }
    else if (selectDocForm.isActive())
    {
        selectDocForm.update(window);
    }
    else if (bookDateForm.isActive())
    {
        bookDateForm.update(window);
    }
    else if (bookTimeForm.isActive())
    {
        bookTimeForm.update(window);
    }
    else
    {
        if (buttons)
        {
            for (int i = 0; i < 8; i++)
            {
                (*(buttons + i))->update(window);
            }
        }
    }
}

void PatientScreen::draw(RenderWindow& window)
{
    if (title)
    {
        window.draw(*title);
    }
    if (balanceText)
    {
        window.draw(*balanceText);
    }

    if (buttons)
    {
        for (int i = 0; i < 8; i++)
        {
            (*(buttons + i))->draw(window);
        }
    }

    if (topUpForm.isActive())
    {
        topUpForm.draw(window);
    }
    if (payBillForm.isActive())
    {
        payBillForm.draw(window);
    }
    if (cancelApptForm.isActive())
    {
        cancelApptForm.draw(window);
    }
    if (searchSpecForm.isActive())
    {
        searchSpecForm.draw(window);
    }
    if (selectDocForm.isActive())
    {
        selectDocForm.draw(window);
    }
    if (bookDateForm.isActive())
    {
        bookDateForm.draw(window);
    }
    if (bookTimeForm.isActive())
    {
        bookTimeForm.draw(window);
    }
    if (dataViewer.isActive())
    {
        dataViewer.draw(window);
    }
}

PatientScreen::~PatientScreen()
{
    delete title;
    delete balanceText;
    if (buttons)
    {
        for (int i = 0; i < 8; i++)
        {
            delete* (buttons + i);
        }
        delete[] buttons;
    }
}