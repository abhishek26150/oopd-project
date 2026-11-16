// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

int ERPSystem::splitTokens(const std::string& line, std::string tokens[], int maxTokens) const {
    int count = 0;
    size_t i = 0, n = line.size();
    while (i < n) {
        while (i < n && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i >= n) break;
        size_t start = i;
        while (i < n && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (count < maxTokens) tokens[count] = line.substr(start, i - start);
        ++count;
    }
    return count;
}

bool ERPSystem::isValidNumber(const std::string& s, int& outVal) const {
    if (s.empty() || s.size() > 9) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    outVal = std::atoi(s.c_str());
    return true;
}

bool ERPSystem::parseGpaString(const std::string& s, int& outHundredths) const {
    size_t dotPos = s.find('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos + 3 != s.size()) return false;
    int whole = 0, frac = 0;
    if (!isValidNumber(s.substr(0, dotPos), whole)) return false;
    if (!isValidNumber(s.substr(dotPos + 1, 2), frac)) return false;
    if (whole > 10 || (whole == 10 && frac > 0)) return false;
    outHundredths = whole * 100 + frac;
    return true;
}

int ERPSystem::findCourse(const std::string& code) const {
    for (int i = 0; i < numCourses; ++i) {
        if (courses[i].code == code) return i;
    }
    return -1;
}

int ERPSystem::findStudent(const std::string& roll) const {
    for (int i = 0; i < numStudents; ++i) {
        if (students[i].roll == roll) return i;
    }
    return -1;
}

ErrorCode ERPSystem::setProgram(const std::string& line) {
    std::string tokens[8];
    if (splitTokens(line, tokens, 8) != 6) return E_PARSE;
    int tot = 0, elec = 0, minGpa = 0, maxLoad = 0, thesisCred = 0;
    if (!isValidNumber(tokens[1], tot) || !isValidNumber(tokens[2], elec) ||
        !parseGpaString(tokens[3], minGpa) || !isValidNumber(tokens[4], maxLoad) ||
        !isValidNumber(tokens[5], thesisCred)) return E_PARSE;
    if (tot <= 0 || elec < 0 || elec > tot || maxLoad <= 0 || thesisCred < 0) return E_PARSE;
    reqTotalCredits = tot;
    reqElectiveCredits = elec;
    minGpaHundredths = minGpa;
    maxTermLoad = maxLoad;
    minThesisCredits = thesisCred;
    return OK;
}

ErrorCode ERPSystem::addCourse(const std::string& line) {
    std::string tokens[8];
    if (splitTokens(line, tokens, 8) != 6) return E_PARSE;
    if (tokens[1].empty() || tokens[1].size() > 10) return E_PARSE;
    CourseType ctype;
    if (tokens[2] == "REGULAR") ctype = REGULAR;
    else if (tokens[2] == "SU") ctype = SU;
    else if (tokens[2] == "THESIS") ctype = THESIS;
    else return E_PARSE;
    int creds = 0;
    if (!isValidNumber(tokens[3], creds)) return E_PARSE;
    if (tokens[4] != "CORE" && tokens[4] != "ELECTIVE") return E_PARSE;
    if (tokens[5].empty() || tokens[5].size() > 10) return E_PARSE;
    if (numCourses >= MAX_COURSES) return E_FULL;
    if (findCourse(tokens[1]) != -1) return E_DUPLICATE;
    if (creds < 0 || creds > 24 || (creds == 0 && ctype != SU)) return E_BAD_COURSE;
    int prereqIdx = -1;
    if (tokens[5] != "-") {
        if (tokens[5] == tokens[1]) return E_BAD_COURSE;
        prereqIdx = findCourse(tokens[5]);
        if (prereqIdx == -1) return E_BAD_COURSE;
    }
    courses[numCourses].code = tokens[1];
    courses[numCourses].type = ctype;
    courses[numCourses].credits = creds;
    courses[numCourses].isCore = (tokens[4] == "CORE");
    courses[numCourses].prereq = prereqIdx;
    ++numCourses;
    return OK;
}

ErrorCode ERPSystem::addStudent(const std::string& line) {
    std::string tokens[5];
    if (splitTokens(line, tokens, 5) != 3) return E_PARSE;
    if (tokens[1].empty() || tokens[1].size() > 10) return E_PARSE;
    if (tokens[2].empty() || tokens[2].size() > 30) return E_PARSE;
    if (numStudents >= MAX_STUDENTS) return E_FULL;
    if (findStudent(tokens[1]) != -1) return E_DUPLICATE;
    students[numStudents].roll = tokens[1];
    students[numStudents].name = tokens[2];
    students[numStudents].numRegs = 0;
    ++numStudents;
    return OK;
}