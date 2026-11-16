// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

ErrorCode ERPSystem::gpaHundredths(int s, int* result) const {
    long long num = 0, den = 0;
    for (int c = 0; c < numCourses; ++c) {
        if (courses[c].type == SU) continue;
        int idx = getCountingAttempt(s, c);
        if (idx == -1) continue;
        const Registration& reg = students[s].regs[idx];
        if (courses[c].type == THESIS && reg.thesis != FINAL) continue;
        int pts = gradePoints(reg.grade);
        if (pts >= 0) {
            num += static_cast<long long>(courses[c].credits) * pts;
            den += courses[c].credits;
        }
    }
    if (den == 0) return E_NO_GPA;
    *result = static_cast<int>((200LL * num + den) / (2LL * den));
    return OK;
}

static const char* hundredthsToAvgGrade(int h) {
    if (h >= 950) return "A";
    if (h >= 850) return "A-";
    if (h >= 750) return "B";
    if (h >= 650) return "B-";
    if (h >= 550) return "C";
    if (h >= 450) return "C-";
    if (h >= 200) return "D";
    return "F";
}

static void printGradStatus(const std::string& roll, bool r1, bool r2, bool r3, bool r4, bool r5) {
    if (r1 && r2 && r3 && r4 && r5) { std::cout << "GRAD " << roll << " YES\n"; return; }
    std::cout << "GRAD " << roll << " NO ";
    bool first = true;
    if (!r1) { std::cout << "R1"; first = false; }
    if (!r2) { if (!first) std::cout << ","; std::cout << "R2"; first = false; }
    if (!r3) { if (!first) std::cout << ","; std::cout << "R3"; first = false; }
    if (!r4) { if (!first) std::cout << ","; std::cout << "R4"; first = false; }
    if (!r5) { if (!first) std::cout << ","; std::cout << "R5"; }
    std::cout << "\n";
}

static bool isValidId(const std::string& id) {
    return !id.empty() && id.size() <= 10;
}

ErrorCode ERPSystem::handleRegister(const std::string tokens[], int count) {
    int term = 0;
    if (count != 4 || !isValidId(tokens[1]) || !isValidId(tokens[2]) ||
        !isValidNumber(tokens[3], term) || term < 1 || term > 12) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    int c = findCourse(tokens[2]);
    if (c == -1) return E_UNKNOWN_COURSE;
    return registerCourse(s, c, term);
}

ErrorCode ERPSystem::handleGrade(const std::string tokens[], int count) {
    int term = 0;
    if (count != 5 || !isValidId(tokens[1]) || !isValidId(tokens[2]) ||
        !isValidNumber(tokens[3], term) || term < 1 || term > 12) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    int c = findCourse(tokens[2]);
    if (c == -1) return E_UNKNOWN_COURSE;
    return submitGrade(s, c, term, tokens[4]);
}

ErrorCode ERPSystem::handleConvert(const std::string tokens[], int count) {
    if (count != 4 || !isValidId(tokens[1]) || !isValidId(tokens[2])) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    int c = findCourse(tokens[2]);
    if (c == -1) return E_UNKNOWN_COURSE;
    return convertThesis(s, c, tokens[3]);
}

ErrorCode ERPSystem::handleGpaOrAvg(const std::string tokens[], int count, bool isAvg) const {
    if (count != 2 || !isValidId(tokens[1])) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    int h = 0;
    ErrorCode err = gpaHundredths(s, &h);
    if (err != OK) return err;
    if (isAvg) {
        std::cout << "AVG " << tokens[1] << " " << hundredthsToAvgGrade(h) << "\n";
    } else {
        char buf[32];
        std::sprintf(buf, "GPA %s %d.%02d\n", tokens[1].c_str(), h / 100, h % 100);
        std::cout << buf;
    }
    return OK;
}

ErrorCode ERPSystem::handleEligible(const std::string tokens[], int count) const {
    int term = 0;
    if (count != 4 || !isValidId(tokens[1]) || !isValidId(tokens[2]) ||
        !isValidNumber(tokens[3], term) || term < 1 || term > 12) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    int c = findCourse(tokens[2]);
    if (c == -1) return E_UNKNOWN_COURSE;
    int ek = checkEligibility(s, c, term);
    if (ek == 0) std::cout << "ELIGIBLE " << tokens[1] << " " << tokens[2] << " YES\n";
    else std::cout << "ELIGIBLE " << tokens[1] << " " << tokens[2] << " NO E" << ek << "\n";
    return OK;
}

ErrorCode ERPSystem::handleGrad(const std::string tokens[], int count) const {
    if (count != 2 || !isValidId(tokens[1])) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;
    bool r1 = (getEarnedCredits(s) >= reqTotalCredits);
    bool r2 = true, r4 = false;
    int elecCredits = 0;
    for (int c = 0; c < numCourses; ++c) {
        bool passed = isCoursePassed(s, c);
        if (courses[c].isCore && !passed) r2 = false;
        if (!courses[c].isCore && passed) elecCredits += courses[c].credits;
        if (courses[c].type == THESIS) {
            int idx = getCountingAttempt(s, c);
            if (idx != -1 && students[s].regs[idx].thesis == FINAL &&
                gradePoints(students[s].regs[idx].grade) >= 6) r4 = true;
        }
    }
    int h = 0;
    bool r5 = (gpaHundredths(s, &h) == OK && h >= minGpaHundredths);
    printGradStatus(tokens[1], r1, r2, (elecCredits >= reqElectiveCredits), r4, r5);
    return OK;
}

void ERPSystem::processLine(const std::string& line, int lineNum, bool isFirstCommand) {
    std::string tokens[10];
    int count = splitTokens(line, tokens, 10);
    if (count == 0) return;
    const std::string& cmd = tokens[0];
    ErrorCode err = OK;
    if (cmd == "PROGRAM") err = isFirstCommand ? setProgram(line) : E_PARSE;
    else if (cmd == "COURSE") err = addCourse(line);
    else if (cmd == "STUDENT") err = addStudent(line);
    else if (cmd == "REGISTER") err = handleRegister(tokens, count);
    else if (cmd == "GRADE") err = handleGrade(tokens, count);
    else if (cmd == "CONVERT") err = handleConvert(tokens, count);
    else if (cmd == "GPA") err = handleGpaOrAvg(tokens, count, false);
    else if (cmd == "AVG") err = handleGpaOrAvg(tokens, count, true);
    else if (cmd == "ELIGIBLE") err = handleEligible(tokens, count);
    else if (cmd == "GRAD") err = handleGrad(tokens, count);
    else if (cmd == "TRANSCRIPT") err = handleTranscript(tokens, count);
    else if (cmd == "SAVE") err = handleSave(tokens, count);
    else if (cmd == "LOAD") err = handleLoad(tokens, count);
    else err = E_PARSE;
    if (err != OK) std::cout << "ERROR " << lineNum << " " << errorToString(err) << "\n";
}