// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

int ERPSystem::gradePoints(const std::string& symbol) const {
    if (symbol == "A")  return 10;
    if (symbol == "A-") return 9;
    if (symbol == "B")  return 8;
    if (symbol == "B-") return 7;
    if (symbol == "C")  return 6;
    if (symbol == "C-") return 5;
    if (symbol == "D")  return 4;
    if (symbol == "F")  return 0;
    return -1;
}

int ERPSystem::getCountingAttempt(int s, int c) const {
    const Student& st = students[s];
    int latestIdx = -1;
    for (int i = 0; i < st.numRegs; ++i) {
        if (st.regs[i].course != c) continue;
        if (courses[c].type == THESIS) {
            if (st.regs[i].thesis == FINAL || st.regs[i].thesis == FAILED) latestIdx = i;
        } else if (!st.regs[i].grade.empty()) {
            latestIdx = i;
        }
    }
    return latestIdx;
}

bool ERPSystem::isCoursePassed(int s, int c) const {
    int idx = getCountingAttempt(s, c);
    if (idx == -1) return false;
    const Registration& reg = students[s].regs[idx];
    if (courses[c].type == SU) return reg.grade == "S";
    if (courses[c].type == THESIS) return reg.thesis == FINAL && gradePoints(reg.grade) >= 4;
    return gradePoints(reg.grade) >= 4;
}

int ERPSystem::getEarnedCredits(int s) const {
    int total = 0;
    for (int c = 0; c < numCourses; ++c) {
        if (isCoursePassed(s, c)) total += courses[c].credits;
    }
    return total;
}

int ERPSystem::getTermCredits(int s, int term) const {
    const Student& st = students[s];
    int total = 0;
    for (int i = 0; i < st.numRegs; ++i) {
        if (st.regs[i].term == term) total += courses[st.regs[i].course].credits;
    }
    return total;
}

int ERPSystem::checkEligibility(int s, int c, int term) const {
    if (s < 0 || s >= numStudents || c < 0 || c >= numCourses) return 1;
    if (isCoursePassed(s, c)) return 2;
    const Student& st = students[s];
    const Course& crs = courses[c];
    for (int i = 0; i < st.numRegs; ++i) {
        if (st.regs[i].course != c) continue;
        if (crs.type == THESIS && (st.regs[i].thesis == REGISTERED || st.regs[i].thesis == IN_PROGRESS)) return 3;
        if (crs.type != THESIS && st.regs[i].grade.empty()) return 3;
    }
    if (crs.prereq != -1 && !isCoursePassed(s, crs.prereq)) return 4;
    if (getTermCredits(s, term) + crs.credits > maxTermLoad) return 5;
    if (crs.type == THESIS && getEarnedCredits(s) < minThesisCredits) return 6;
    return 0;
}

ErrorCode ERPSystem::registerCourse(int s, int c, int term) {
    int check = checkEligibility(s, c, term);
    if (check == 2) return E_ALREADY_PASSED;
    if (check == 3) return E_ALREADY_REGISTERED;
    if (check == 4) return E_PREREQ;
    if (check == 5) return E_CREDIT_LIMIT;
    if (check == 6) return E_THESIS_NOT_ALLOWED;
    Student& st = students[s];
    if (st.numRegs >= MAX_REGS) return E_FULL;
    Registration& r = st.regs[st.numRegs];
    r.course = c;
    r.term = term;
    r.grade = "";
    r.thesis = (courses[c].type == THESIS) ? REGISTERED : NOT_THESIS;
    ++st.numRegs;
    return OK;
}

ErrorCode ERPSystem::submitGrade(int s, int c, int term, const std::string& symbol) {
    bool isLetter = (gradePoints(symbol) != -1);
    bool isSX = (symbol == "S" || symbol == "X");
    if (!isLetter && !isSX) return E_BAD_GRADE;
    const Course& crs = courses[c];
    if (crs.type == REGULAR && !isLetter) return E_GRADE_TYPE;
    if ((crs.type == SU || crs.type == THESIS) && !isSX) return E_GRADE_TYPE;
    Student& st = students[s];
    int regIdx = -1;
    for (int i = 0; i < st.numRegs; ++i) {
        if (st.regs[i].course == c && st.regs[i].term == term) { regIdx = i; break; }
    }
    if (regIdx == -1) return E_NOT_REGISTERED;
    if (!st.regs[regIdx].grade.empty()) return E_ALREADY_GRADED;
    st.regs[regIdx].grade = symbol;
    if (crs.type == THESIS) st.regs[regIdx].thesis = (symbol == "S") ? IN_PROGRESS : FAILED;
    return OK;
}

ErrorCode ERPSystem::convertThesis(int s, int c, const std::string& letter) {
    if (gradePoints(letter) == -1) return E_BAD_GRADE;
    if (courses[c].type != THESIS) return E_GRADE_TYPE;
    Student& st = students[s];
    int regIdx = -1;
    for (int i = 0; i < st.numRegs; ++i) {
        if (st.regs[i].course == c) regIdx = i;
    }
    if (regIdx == -1) return E_NOT_REGISTERED;
    if (st.regs[regIdx].thesis == FINAL) return E_ALREADY_GRADED;
    if (st.regs[regIdx].thesis != IN_PROGRESS) return E_THESIS_STATE;
    st.regs[regIdx].grade = letter;
    st.regs[regIdx].thesis = FINAL;
    return OK;
}