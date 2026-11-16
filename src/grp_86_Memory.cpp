// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

Student::Student() : roll(""), name(""), regs(NULL), numRegs(0) {
    regs = new Registration[MAX_REGS];
}

Student::~Student() {
    delete[] regs;
    regs = NULL;
}

ERPSystem::ERPSystem()
    : courses(NULL), numCourses(0), students(NULL), numStudents(0),
      reqTotalCredits(160), reqElectiveCredits(24), minGpaHundredths(500),
      maxTermLoad(24), minThesisCredits(100) {
    courses = new Course[MAX_COURSES];
    students = new Student[MAX_STUDENTS];
}

ERPSystem::~ERPSystem() {
    delete[] courses;
    courses = NULL;
    delete[] students;
    students = NULL;
}

const char* errorToString(ErrorCode err) {
    switch (err) {
        case OK:                   return "OK";
        case E_PARSE:              return "E_PARSE";
        case E_FULL:               return "E_FULL";
        case E_DUPLICATE:          return "E_DUPLICATE";
        case E_UNKNOWN_STUDENT:    return "E_UNKNOWN_STUDENT";
        case E_UNKNOWN_COURSE:     return "E_UNKNOWN_COURSE";
        case E_BAD_COURSE:         return "E_BAD_COURSE";
        case E_BAD_GRADE:          return "E_BAD_GRADE";
        case E_GRADE_TYPE:         return "E_GRADE_TYPE";
        case E_NOT_REGISTERED:     return "E_NOT_REGISTERED";
        case E_ALREADY_GRADED:     return "E_ALREADY_GRADED";
        case E_THESIS_STATE:       return "E_THESIS_STATE";
        case E_ALREADY_PASSED:     return "E_ALREADY_PASSED";
        case E_ALREADY_REGISTERED: return "E_ALREADY_REGISTERED";
        case E_PREREQ:             return "E_PREREQ";
        case E_CREDIT_LIMIT:       return "E_CREDIT_LIMIT";
        case E_THESIS_NOT_ALLOWED: return "E_THESIS_NOT_ALLOWED";
        case E_NO_GPA:             return "E_NO_GPA";
    }
    return "E_PARSE";
}