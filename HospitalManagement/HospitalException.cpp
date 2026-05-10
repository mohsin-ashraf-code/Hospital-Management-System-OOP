#include "HospitalException.h"

HospitalException::HospitalException(const char* msg) {
    int i = 0;
    while (*(msg + i) != '\0' && i < 199) {
        *(message + i) = *(msg + i);
        i++;
    }
    *(message + i) = '\0';
}
const char* HospitalException::what() const noexcept { return message; }