/*  ============================================================
    STUDENT MANAGEMENT SYSTEM
    Console-based application using File Handling (C++)
    ------------------------------------------------------------
    Features : Add / Display / Search / Update / Delete students
    Storage  : students.txt  (persistent, pipe-delimited text)
    ============================================================ */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const string FILE_NAME = "students.txt";

/* ---------------- Data Model ---------------- */

struct Student {
    int    rollNo = 0;
    string name;
    string course;
    float  marks  = 0.0f;
};

/* ---------------- Input Helpers (validated) ---------------- */

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Please enter a valid integer.\n";
    }
}

float readFloat(const string& prompt) {
    float value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Please enter a valid number.\n";
    }
}

string readLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

/* ---------------- File Handling ---------------- */

// Reads every record from the file into a vector.
vector<Student> loadStudents() {
    vector<Student> list;
    ifstream fin(FILE_NAME);
    if (!fin.is_open()) return list;          // first run: file doesn't exist yet

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string roll, name, course, marks;

        getline(ss, roll,   '|');
        getline(ss, name,   '|');
        getline(ss, course, '|');
        getline(ss, marks,  '|');

        if (roll.empty() || name.empty()) continue;

        Student s;
        try {
            s.rollNo = stoi(roll);
            s.marks  = marks.empty() ? 0.0f : stof(marks);
        } catch (...) {
            continue;                          // skip a corrupted line safely
        }
        s.name   = name;
        s.course = course;
        list.push_back(s);
    }
    fin.close();
    return list;
}

// Overwrites the file with the current in-memory records.
void saveStudents(const vector<Student>& list) {
    ofstream fout(FILE_NAME, ios::trunc);
    for (const Student& s : list) {
        fout << s.rollNo << '|' << s.name << '|'
             << s.course << '|' << s.marks << '\n';
    }
    fout.close();
}

/* ---------------- Helpers ---------------- */

// Returns index of the student with this roll number, or -1.
int findStudent(const vector<Student>& list, int rollNo) {
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].rollNo == rollNo) return static_cast<int>(i);
    return -1;
}

/* ---------------- CRUD Operations ---------------- */

void addStudent(vector<Student>& list) {
    cout << "\n--- Add Student ---\n";
    int roll = readInt("Enter Roll Number : ");

    if (findStudent(list, roll) != -1) {
        cout << "  [!] A student with roll number " << roll << " already exists.\n";
        return;
    }

    Student s;
    s.rollNo = roll;
    s.name   = readLine("Enter Name         : ");
    s.course = readLine("Enter Course       : ");
    s.marks  = readFloat("Enter Marks        : ");

    list.push_back(s);
    saveStudents(list);                        // persist immediately
    cout << "  [OK] Student added successfully.\n";
}

void displayStudents(const vector<Student>& list) {
    cout << "\n--- All Student Records ---\n";
    if (list.empty()) {
        cout << "  No records found.\n";
        return;
    }

    cout << left
         << setw(10) << "Roll No"
         << setw(25) << "Name"
         << setw(20) << "Course"
         << setw(8)  << "Marks" << '\n';
    cout << string(63, '-') << '\n';

    for (const Student& s : list) {
        cout << left
             << setw(10) << s.rollNo
             << setw(25) << s.name
             << setw(20) << s.course
             << setw(8)  << fixed << setprecision(2) << s.marks << '\n';
    }
    cout << string(63, '-') << '\n';
    cout << "Total records: " << list.size() << '\n';
}

void searchStudent(const vector<Student>& list) {
    cout << "\n--- Search Student ---\n";
    int roll = readInt("Enter Roll Number to search : ");
    int idx  = findStudent(list, roll);

    if (idx == -1) {
        cout << "  [!] Student not found.\n";
        return;
    }

    const Student& s = list[idx];
    cout << "  Roll No : " << s.rollNo << '\n';
    cout << "  Name    : " << s.name   << '\n';
    cout << "  Course  : " << s.course << '\n';
    cout << "  Marks   : " << fixed << setprecision(2) << s.marks << '\n';
}

void updateStudent(vector<Student>& list) {
    cout << "\n--- Update Student ---\n";
    int roll = readInt("Enter Roll Number to update : ");
    int idx  = findStudent(list, roll);

    if (idx == -1) {
        cout << "  [!] Student not found.\n";
        return;
    }

    Student& s = list[idx];
    cout << "  (Press Enter to keep the current value.)\n";

    string name = readLine("New Name   [" + s.name + "] : ");
    if (!name.empty()) s.name = name;

    string course = readLine("New Course [" + s.course + "] : ");
    if (!course.empty()) s.course = course;

    ostringstream oldMarks;
    oldMarks << fixed << setprecision(2) << s.marks;
    string marksStr = readLine("New Marks  [" + oldMarks.str() + "] : ");
    if (!marksStr.empty()) {
        try {
            s.marks = stof(marksStr);
        } catch (...) {
            cout << "  [!] Invalid marks value. Old value kept.\n";
        }
    }

    saveStudents(list);
    cout << "  [OK] Record updated successfully.\n";
}

void deleteStudent(vector<Student>& list) {
    cout << "\n--- Delete Student ---\n";
    int roll = readInt("Enter Roll Number to delete : ");
    int idx  = findStudent(list, roll);

    if (idx == -1) {
        cout << "  [!] Student not found.\n";
        return;
    }

    cout << "  Found: " << list[idx].name
         << " (" << list[idx].course << ")\n";

    string confirm = readLine("Are you sure you want to delete? (y/n) : ");
    if (confirm != "y" && confirm != "Y") {
        cout << "  Deletion cancelled.\n";
        return;
    }

    list.erase(list.begin() + idx);
    saveStudents(list);
    cout << "  [OK] Record deleted successfully.\n";
}

/* ---------------- Menu ---------------- */

void showMenu() {
    cout << "\n=========================================\n";
    cout << "        STUDENT MANAGEMENT SYSTEM\n";
    cout << "=========================================\n";
    cout << "  1. Add Student\n";
    cout << "  2. Display All Students\n";
    cout << "  3. Search Student\n";
    cout << "  4. Update Student\n";
    cout << "  5. Delete Student\n";
    cout << "  6. Exit\n";
    cout << "=========================================\n";
}

int main() {
    vector<Student> students = loadStudents();

    cout << "Welcome to the Student Management System!\n";
    cout << "Loaded " << students.size()
         << " record(s) from '" << FILE_NAME << "'.\n";

    while (true) {
        showMenu();
        int choice = readInt("Enter your choice (1-6) : ");

        switch (choice) {
            case 1: addStudent(students);      break;
            case 2: displayStudents(students); break;
            case 3: searchStudent(students);   break;
            case 4: updateStudent(students);   break;
            case 5: deleteStudent(students);   break;
            case 6:
                cout << "\nData saved to '" << FILE_NAME << "'. Goodbye!\n";
                return 0;
            default:
                cout << "  [!] Invalid choice. Please enter 1-6.\n";
        }
    }
}
