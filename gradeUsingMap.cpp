/*✨practice for polishing my c++ coding skill ✨*/
#include <iostream>
#include <map>
#include <string>
#include <limits>
#include <memory>   // unique_ptr

class Student {
private:
    std::string studentID;
    std::string studentName;
    std::map<std::string, int> studentGrade;

public:
    Student();
    Student(std::string, std::string);
    Student(std::string, std::string, std::map<std::string, int>);

    std::string getStudentID() const;
    std::string getStudentName() const;
    const std::map<std::string, int>& getStudentGrade() const;

    void setStudentID(std::string);
    void setStudentName(std::string);
    void setStudentGrade(std::map<std::string, int>);

    bool addOrUpdateGrade(const std::string&, int);

    void showGradeInformation() const;
    void filterByMinGrade(int min) const;
    void filterByRange(int min, int max) const;

    ~Student();
};

// ===== Student Implementation =====
Student::Student() : studentID(""), studentName(""), studentGrade() {}

Student::Student(std::string ID, std::string Name)
    : studentID(std::move(ID)), studentName(std::move(Name)), studentGrade() {}

Student::Student(std::string ID, std::string Name, std::map<std::string, int> Grade)
    : studentID(std::move(ID)), studentName(std::move(Name)), studentGrade(std::move(Grade)) {}

std::string Student::getStudentID() const { return studentID; }
std::string Student::getStudentName() const { return studentName; }
const std::map<std::string, int>& Student::getStudentGrade() const { return studentGrade; }

void Student::setStudentID(std::string studentID) { this->studentID = std::move(studentID); }
void Student::setStudentName(std::string studentName) { this->studentName = std::move(studentName); }
void Student::setStudentGrade(std::map<std::string, int> studentGrade) { this->studentGrade = std::move(studentGrade); }

bool Student::addOrUpdateGrade(const std::string& subjectCode, int subjectGrade) {
    if (subjectCode.empty() || subjectGrade < 0 || subjectGrade > 100) return false;
    studentGrade[subjectCode] = subjectGrade;
    return true;
}

void Student::showGradeInformation() const {
    if (studentGrade.empty()) {
        std::cout << "No grades yet.\n";
        return;
    }
    std::cout << "Course Grades:\n";
    for (const auto& [code, grade] : studentGrade) {
        std::cout << code << ": " << grade << '\n';
    }
}

void Student::filterByMinGrade(int min) const {
    if (min < 0) min = 0;
    if (min > 100) min = 100;

    bool found = false;
    std::cout << "Grades >= " << min << ":\n";
    for (const auto& [code, grade] : studentGrade) {
        if (grade >= min) {
            std::cout << code << ": " << grade << '\n';
            found = true;
        }
    }
    if (!found) std::cout << "(none)\n";
}

void Student::filterByRange(int min, int max) const {
    if (min > max) std::swap(min, max);
    if (min < 0) min = 0;
    if (max > 100) max = 100;

    bool found = false;
    std::cout << "Grades in [" << min << ", " << max << "]:\n";
    for (const auto& [code, grade] : studentGrade) {
        if (grade >= min && grade <= max) {
            std::cout << code << ": " << grade << '\n';
            found = true;
        }
    }
    if (!found) std::cout << "(none)\n";
}

Student::~Student() {}

// ===== Helper Input Utilities =====
static void clearBadInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int x;
        if (std::cin >> x) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        clearBadInput();
        std::cout << "Invalid input. Please enter an integer.\n";
    }
}

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin >> std::ws, s);
    return s;
}

// ===== Console Functions =====
Student createStudentConsole() {
    std::string id = readLine("Enter Student ID: ");
    std::string name = readLine("Enter Student Name: ");
    return Student(id, name);
}

// ✅ Reference version (best for this program)
void runStudentConsole(Student& s) {
    while (true) {
        std::cout << "====================================\n";
        std::cout << " Welcome to UOW Course Grade System\n";
        std::cout << "====================================\n";
        std::cout << "Student ID: " << s.getStudentID() << '\n';
        std::cout << "Student Name: " << s.getStudentName() << '\n';
        std::cout << "+++++++++++ Student Menu +++++++++++\n";
        std::cout << "1. Add / Update Grade\n";
        std::cout << "2. Show All Grades\n";
        std::cout << "3. Filter Grades (>= min)\n";
        std::cout << "4. Filter Grades (range)\n";
        std::cout << "0. Exit\n";
        std::cout << "====================================\n";

        int choice = readInt("Choice: ");
        if (choice == 0) break;

        if (choice == 1) {
            std::string code = readLine("Subject code: ");
            int grade = readInt("Grade (0-100): ");

            if (s.addOrUpdateGrade(code, grade)) std::cout << "Saved!\n";
            else std::cout << "Invalid input.\n";
        }
        else if (choice == 2) {
            s.showGradeInformation();
        }
        else if (choice == 3) {
            int min = readInt("Min grade: ");
            s.filterByMinGrade(min);
        }
        else if (choice == 4) {
            int min = readInt("Min grade: ");
            int max = readInt("Max grade: ");
            s.filterByRange(min, max);
        }
        else {
            std::cout << "Invalid choice.\n";
        }

        std::cout << '\n';
    }
}

// ✅ Pointer version (practice)
void runStudentConsolePtr(Student* s) {
    if (s == nullptr) return;

    std::cout << "\n[Pointer Console Demo]\n";
    s->addOrUpdateGrade("CSIT128", 90);
    s->addOrUpdateGrade("CSIT121", 85);
    s->showGradeInformation();
    std::cout << "[End Pointer Demo]\n\n";
}

// ===== main =====
int main() {
    // Create student (returned by value, RVO makes it efficient)
    Student s = createStudentConsole();

    // Pointer practice: pass address of stack object
    runStudentConsolePtr(&s);

    // Real console: pass by reference so changes persist
    runStudentConsole(s);

    // Bonus: smart pointer demo (optional style)
    // auto sp = std::make_unique<Student>(createStudentConsole());
    // runStudentConsolePtr(sp.get());
    // runStudentConsole(*sp);

    return 0;
}
