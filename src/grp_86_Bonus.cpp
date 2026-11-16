// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

ErrorCode ERPSystem::handleTranscript(const std::string tokens[], int count) const {
    if (count != 2 || tokens[1].empty() || tokens[1].size() > 10) return E_PARSE;
    int s = findStudent(tokens[1]);
    if (s == -1) return E_UNKNOWN_STUDENT;

    const Student& st = students[s];
    std::cout << "TRANSCRIPT " << st.roll << "\n";
    for (int t = 1; t <= 12; ++t) {
        for (int i = 0; i < st.numRegs; ++i) {
            if (st.regs[i].term != t) continue;
            int c = st.regs[i].course;
            std::string g = st.regs[i].grade.empty() ? "-" : st.regs[i].grade;
            std::cout << "TERM " << t << " " << courses[c].code << " "
                      << courses[c].credits << " " << g << "\n";
        }
    }
    return OK;
}

ErrorCode ERPSystem::handleSave(const std::string tokens[], int count) const {
    if (count != 2 || tokens[1].empty()) return E_PARSE;
    std::ofstream out(tokens[1].c_str());
    if (!out.is_open()) return E_PARSE;

    out << "ACADERP_SAVE_V1\n";
    out << reqTotalCredits << " " << reqElectiveCredits << " "
        << minGpaHundredths << " " << maxTermLoad << " " << minThesisCredits << "\n";
    out << numCourses << "\n";
    for (int i = 0; i < numCourses; ++i) {
        out << courses[i].code << " " << static_cast<int>(courses[i].type) << " "
            << courses[i].credits << " " << (courses[i].isCore ? 1 : 0) << " "
            << courses[i].prereq << "\n";
    }
    out << numStudents << "\n";
    for (int s = 0; s < numStudents; ++s) {
        out << students[s].roll << " " << students[s].name << " " << students[s].numRegs << "\n";
        for (int r = 0; r < students[s].numRegs; ++r) {
            std::string g = students[s].regs[r].grade.empty() ? "-" : students[s].regs[r].grade;
            out << students[s].regs[r].course << " " << students[s].regs[r].term << " "
                << g << " " << static_cast<int>(students[s].regs[r].thesis) << "\n";
        }
    }
    return OK;
}

ErrorCode ERPSystem::handleLoad(const std::string tokens[], int count) {
    if (count != 2 || tokens[1].empty()) return E_PARSE;
    std::ifstream in(tokens[1].c_str());
    if (!in.is_open()) return E_PARSE;

    std::string magic;
    if (!(in >> magic) || magic != "ACADERP_SAVE_V1") return E_PARSE;

    int tot = 0, elec = 0, minGpa = 0, maxLoad = 0, thesisCred = 0, nC = 0;
    if (!(in >> tot >> elec >> minGpa >> maxLoad >> thesisCred >> nC)) return E_PARSE;
    if (nC < 0 || nC > MAX_COURSES) return E_FULL;

    Course* tmpCourses = new Course[MAX_COURSES];
    for (int i = 0; i < nC; ++i) {
        int tInt = 0, coreInt = 0;
        if (!(in >> tmpCourses[i].code >> tInt >> tmpCourses[i].credits >> coreInt >> tmpCourses[i].prereq)) {
            delete[] tmpCourses;
            return E_PARSE;
        }
        tmpCourses[i].type = static_cast<CourseType>(tInt);
        tmpCourses[i].isCore = (coreInt != 0);
    }

    int nS = 0;
    if (!(in >> nS) || nS < 0 || nS > MAX_STUDENTS) {
        delete[] tmpCourses;
        return (nS > MAX_STUDENTS) ? E_FULL : E_PARSE;
    }

    Student* tmpStudents = new Student[MAX_STUDENTS];
    for (int s = 0; s < nS; ++s) {
        int nR = 0;
        if (!(in >> tmpStudents[s].roll >> tmpStudents[s].name >> nR) || nR < 0 || nR > MAX_REGS) {
            delete[] tmpCourses;
            delete[] tmpStudents;
            return (nR > MAX_REGS) ? E_FULL : E_PARSE;
        }
        tmpStudents[s].numRegs = nR;
        for (int r = 0; r < nR; ++r) {
            std::string g;
            int thInt = 0;
            if (!(in >> tmpStudents[s].regs[r].course >> tmpStudents[s].regs[r].term >> g >> thInt)) {
                delete[] tmpCourses;
                delete[] tmpStudents;
                return E_PARSE;
            }
            tmpStudents[s].regs[r].grade = (g == "-") ? "" : g;
            tmpStudents[s].regs[r].thesis = static_cast<ThesisState>(thInt);
        }
    }

    delete[] courses;
    delete[] students;
    courses = tmpCourses;
    students = tmpStudents;
    numCourses = nC;
    numStudents = nS;
    reqTotalCredits = tot;
    reqElectiveCredits = elec;
    minGpaHundredths = minGpa;
    maxTermLoad = maxLoad;
    minThesisCredits = thesisCred;
    return OK;
}