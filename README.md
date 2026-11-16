<!-- group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani -->

# AcadERP-Lite — Academic Records & Graduation Audit System

**Group ID:** group-86  
**Members:**
- MT26150 — Abhishek Kumar Singh
- MT26203 — Abhay Sahani

---

## 1. How to Build and Run

- **Build the Binary:** Run `make -C src` from the project root directory to produce `src/acaderp`.
- **Run Automated Tests:** Run `./run_tests.sh` to compile and execute all 26 test suites (`T00` through `T25`).
- **Clean Build Artifacts:** Run `make -C src clean` to remove compiled `.o` files and the `src/acaderp` binary.

---

## 2. Data Layout & Modular Architecture

The program avoids all STL containers (`std::vector`, `std::map`, etc.), smart pointers, and exceptions, using dynamically allocated arrays and explicit `ErrorCode` return values across modular files:

- `src/grp_86_erp.h`: Defines enums (`ErrorCode`, `CourseType`, `ThesisState`) and classes (`Course`, `Registration`, `Student`, `ERPSystem`).
- `src/grp_86_main.cpp`: Reads `stdin` line-by-line, strips `\r`, preserves 1-based line numbers across blank/comment lines, and enforces `PROGRAM` as the first command.
- `src/grp_86_Memory.cpp`: Handles dynamic array allocation (`new[]`), deallocation (`delete[]`), and `ErrorCode` string mapping.
- `src/grp_86_Parser.cpp`: Handles tokenization, numeric/GPA parsing, linear lookups, and `PROGRAM`, `COURSE`, and `STUDENT` setup commands.
- `src/grp_86_Academic.cpp`: Handles `ELIGIBLE` (`E1`–`E6`), `REGISTER`, `GRADE`, and thesis `CONVERT` state transitions.
- `src/grp_86_Audit.cpp`: Handles integer-only `GPA`, `AVG` grade mapping, `GRAD` (`R1`–`R5`) graduation audit, and command dispatching.
- `src/grp_86_Bonus.cpp`: Handles the bonus commands `TRANSCRIPT`, `SAVE`, and `LOAD` (using `<fstream>` only for file persistence).

---

## 3. Dynamic Memory Management (`new[]` and `delete[]`)

- `ERPSystem::ERPSystem()` (`src/grp_86_Memory.cpp`): Allocates `courses = new Course[MAX_COURSES];` and `students = new Student[MAX_STUDENTS];` (each `Student` constructor allocates `regs = new Registration[MAX_REGS];`).
- `Student::~Student()` & `ERPSystem::~ERPSystem()` (`src/grp_86_Memory.cpp`): Frees `regs`, `students`, and `courses` via `delete[]` and sets pointers to `NULL`.
- `ERPSystem::handleLoad()` (`src/grp_86_Bonus.cpp`): Allocates temporary `Course[]` and `Student[]` buffers while parsing a `LOAD` file, freeing temporary buffers on error or replacing and freeing old arrays via `delete[]` on success.

---

## 4. Assumptions

1. **Re-registration After Failure:** A student may re-register for a failed course (`F`, `X`, or thesis converted to `F`) once no active (`REGISTERED` or `IN_PROGRESS`) attempt exists (`E3`).
2. **Thesis `CONVERT` Target:** `CONVERT` targets the latest registration of the thesis course; returns `E_NOT_REGISTERED` if never registered, `E_ALREADY_GRADED` if already `FINAL`, and `E_THESIS_STATE` if `REGISTERED` or `FAILED`.
3. **Multiple Thesis Courses:** Rule `R4` is satisfied if at least one `THESIS` course in the catalogue has a counting `FINAL` attempt with grade `>= C` (6 points).
4. **Term Load (`E5`):** All registrations in a given term count toward `maxTermLoad` regardless of grading status.
5. **Whitespace & Line Endings:** Leading/trailing spaces, multiple spaces, and `\r` are stripped cleanly while preserving exact 1-based line numbers for errors.
6. **Zero-Credit `SU` Courses:** A `0`-credit `SU` course graded `S` satisfies `R2` (if `CORE`) and prerequisites (`E4`) while contributing `0` credits to `R1`, `R3`, and `GPA`.
7. **Bonus `TRANSCRIPT`:** Prints `TRANSCRIPT <roll>` followed by `TERM <term> <course> <credits> <grade>` ordered by term (`1`–`12`), using `-` for ungraded courses.
8. **Bonus `SAVE` & `LOAD`:** Serializes and restores complete system state; returns `E_PARSE` on file or format errors without altering existing in-memory data.