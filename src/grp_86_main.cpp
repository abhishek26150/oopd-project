// group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

#include "grp_86_erp.h"

static bool isBlankLine(const std::string& line) {
    for (size_t i = 0; i < line.size(); ++i) {
        if (!std::isspace(static_cast<unsigned char>(line[i]))) {
            return false;
        }
    }
    return true;
}

int main() {
    ERPSystem erp;
    std::string line;
    int lineNum = 0;
    bool isFirstCommand = true;

    while (std::getline(std::cin, line)) {
        ++lineNum;

        // Strip trailing carriage return (\r) if present (Windows CRLF files)
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        if (isBlankLine(line)) {
            continue;
        }

        // Check first non-whitespace character for '#'
        size_t firstChar = 0;
        while (firstChar < line.size() && std::isspace(static_cast<unsigned char>(line[firstChar]))) {
            ++firstChar;
        }
        if (firstChar < line.size() && line[firstChar] == '#') {
            continue;
        }

        erp.processLine(line, lineNum, isFirstCommand);
        isFirstCommand = false;
    }

    return 0;
}