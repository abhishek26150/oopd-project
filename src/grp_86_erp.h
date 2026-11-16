// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#ifndef GRP_86_ERP_H
#define GRP_86_ERP_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

const int MAX_STUDENTS = 100;
const int MAX_COURSES = 50;
const int MAX_REGS = 40;

enum ErrorCode {
    OK,
    E_PARSE,
    E_FULL,
    E_DUPLICATE,
    E_UNKNOWN_STUDENT,
    E_UNKNOWN_COURSE,
    E_BAD_COURSE,
    E_BAD_GRADE,
    E_GRADE_TYPE,
    E_NOT_REGISTERED,
    E_ALREADY_GRADED,
    E_THESIS_STATE,
    E_ALREADY_PASSED,
    E_ALREADY_REGISTERED,
    E_PREREQ,
    E_CREDIT_LIMIT,
    E_THESIS_NOT_ALLOWED,
    E_NO_GPA
};

enum CourseType { REGULAR, SU, THESIS };
enum ThesisState { NOT_THESIS, REGISTERED, IN_PROGRESS, FINAL, FAILED };

class Course {
public:
    std::string code;
    CourseType type;
    int credits;
    bool isCore;
    int prereq;

    Course() : code(""), type(REGULAR), credits(0), isCore(true), prereq(-1) {}
};

class Registration {
public:
    int course;
    int term;
    std::string grade;
    ThesisState thesis;

    Registration() : course(-1), term(0), grade(""), thesis(NOT_THESIS) {}
};

class Student {
public:
    std::string roll;
    std::string name;
    Registration* regs;
    int numRegs;

    Student();
    ~Student();
};

class ERPSystem {
private:
    Course* courses;
    int numCourses;
    Student* students;
    int numStudents;

    int reqTotalCredits;
    int reqElectiveCredits;
    int minGpaHundredths;
    int maxTermLoad;
    int minThesisCredits;

    int findCourse(const std::string& code) const;
    int findStudent(const std::string& roll) const;
    bool isValidNumber(const std::string& s, int& outVal) const;
    bool parseGpaString(const std::string& s, int& outHundredths) const;
    int splitTokens(const std::string& line, std::string tokens[], int maxTokens) const;

    bool isCoursePassed(int s, int c) const;
    int getEarnedCredits(int s) const;
    int getTermCredits(int s, int term) const;
    int getCountingAttempt(int s, int c) const;

    ErrorCode handleRegister(const std::string tokens[], int count);
    ErrorCode handleGrade(const std::string tokens[], int count);
    ErrorCode handleConvert(const std::string tokens[], int count);
    ErrorCode handleGpaOrAvg(const std::string tokens[], int count, bool isAvg) const;
    ErrorCode handleEligible(const std::string tokens[], int count) const;
    ErrorCode handleGrad(const std::string tokens[], int count) const;
    ErrorCode handleTranscript(const std::string tokens[], int count) const;
    ErrorCode handleSave(const std::string tokens[], int count) const;
    ErrorCode handleLoad(const std::string tokens[], int count);

public:
    ERPSystem();
    ~ERPSystem();

    ErrorCode setProgram(const std::string& line);
    ErrorCode addCourse(const std::string& line);
    ErrorCode addStudent(const std::string& line);
    ErrorCode registerCourse(int s, int c, int term);
    ErrorCode submitGrade(int s, int c, int term, const std::string& symbol);
    ErrorCode convertThesis(int s, int c, const std::string& letter);

    ErrorCode gpaHundredths(int s, int* result) const;
    int checkEligibility(int s, int c, int term) const;
    int gradePoints(const std::string& symbol) const;

    void processLine(const std::string& line, int lineNum, bool isFirstCommand);
};

const char* errorToString(ErrorCode err);

#endif