#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cctype>
#include <iomanip>
using namespace std;
// ---------------------------------------------------------------- DEBUGGING
#ifdef DEBUG
#define LOG(x) cerr << "[DEBUG] " << x << endl
#else
#define LOG(x)
#endif
// ---------------------------------------------------------------- CONSTANTS
const int MAX_USERS = 100;
const int MAX_COURSES = 20;
const int SUBJECTS = 5;
const int MAX_ENROLL = 5;
// Global variable (accessed with the scope resolution operator ::)
string institutionName = "Sanjivani College of Engineering";
// Array of subject names
const string subjectNames[SUBJECTS] = {
 "Mathematics", "OOP with C++", "Data Structures",
 "Digital Electronics", "Communication Skills"};
// ---------------------------------------------------------------- ENUMS
enum Department { IT = 1, CS, ENTC, MECH };
enum Role { STUDENT = 1, TEACHER = 2 };
string deptName(int d)
{
 switch (d)
 {
 case IT: return "Information Technology";
 case CS: return "Computer Science";
 case ENTC: return "Electronics & Telecom";
 case MECH: return "Mechanical";
 default: return "Unknown";
 }
}
// ---------------------------------------------------------------- EXCEPTION
class LMSException
{
 string message;
public:
 LMSException(string m) : message(m) {}
 string what() const { return message; }
};
// ---------------------------------------------------------------- INLINE FUNCTIONS
inline char getGrade(float p)
{
 if (p >= 75) return 'A';
 else if (p >= 60) return 'B';
 else if (p >= 50) return 'C';
 else if (p >= 40) return 'D';
 return 'F';
}
inline float calcPercentage(float total, int subjects) { return total / subjects; }
// ---------------------------------------------------------------- REFERENCE VARIABLES
// total, percentage and grade are returned through reference parameters
void calculateResult(const float m[], float &total, float &percentage, char &grade)
{
 total = 0;
 for (int i = 0; i < SUBJECTS; i++)
 total += m[i];
 percentage = calcPercentage(total, SUBJECTS);
 grade = getGrade(percentage);
}
// ---------------------------------------------------------------- FUNCTION TEMPLATES
template <class T>
T maximum(T a, T b) { return (a > b) ? a : b; }
template <class T>
T minimum(T a, T b) { return (a < b) ? a : b; }
// Generic input function: works for int, float, double ...
template <class T>
T readValue(const string &prompt)
{
 T v;
 while (true)
 {
 cout << prompt;
 if (cin >> v)
 {
 cin.ignore(10000, '\n');
 return v;
 }
 if (cin.eof())
 throw LMSException("Input stream closed.");
 cout << " Invalid input. Please try again.\n";
 cin.clear();
 cin.ignore(10000, '\n');
 }
}
string readLine(const string &prompt)
{
 string s;
 while (true)
 {
 cout << prompt;
 if (!getline(cin, s))
 throw LMSException("Input stream closed.");
 if (!s.empty())
 return s;
 cout << " This field cannot be empty.\n";
 }
}
// ---------------------------------------------------------------- CLASS TEMPLATE
template <class T>
class Statistics
{
 T data[MAX_USERS];
 int n;
public:
 Statistics() : n(0) {}
 void add(T v) { if (n < MAX_USERS) data[n++] = v; }
 int count() const { return n; }
 T highest() const
 {
 T h = data[0];
 for (int i = 1; i < n; i++) h = maximum(h, data[i]);
 return h;
 }
 T lowest() const
 {
 T l = data[0];
 for (int i = 1; i < n; i++) l = minimum(l, data[i]);
 return l;
 }
 double average() const
 {
 double s = 0;
 for (int i = 0; i < n; i++) s += data[i];
 return n ? s / n : 0;
 }
};
// ---------------------------------------------------------------- ABSTRACT CLASS
class User
{
protected:
 int id;
 string name;
 Department dept;
 static int totalUsers; // static data member
public:
 User() : id(0), name("Unknown"), dept(IT) { totalUsers++; }
 User(int i, string n, Department d) : id(i), name(n), dept(d) { totalUsers++; }
 virtual ~User() { totalUsers--; LOG("User " << id << " destroyed"); }
 virtual void display() const = 0; // pure virtual functions
 virtual string getRole() const = 0;
 virtual void saveToFile(ofstream &fo) const = 0;
 int getId() const { return id; }
 string getName() const { return name; }
 Department getDept() const { return dept; }
 // String handling: username = first 4 letters of name (lower case) + id
 string username() const
 {
 string u = name.substr(0, 4);
 for (size_t i = 0; i < u.length(); i++)
 u[i] = tolower(u[i]);
 u.append(to_string(id));
 return u;
 }
 string email() const { return username() + "@lms.edu"; }
 static int getTotalUsers() { return totalUsers; }
};
int User::totalUsers = 0; // static member initialisation
// ---------------------------------------------------------------- STUDENT (inheritance)
class Student : public User
{
 float marks[SUBJECTS];
 float total, percentage;
 char grade;
 bool marksEntered;
 int enrolled[MAX_ENROLL];
 int enrolledCount;
public:
 Student(int i, string n, Department d) : User(i, n, d)
 {
 total = percentage = 0;
 grade = '-';
 marksEntered = false;
 enrolledCount = 0;
 for (int k = 0; k < SUBJECTS; k++) marks[k] = 0;
 }
 void setMarks(const float m[])
 {
 for (int i = 0; i < SUBJECTS; i++)
 if (m[i] < 0 || m[i] > 100)
 throw LMSException("Marks must be between 0 and 100.");
 for (int i = 0; i < SUBJECTS; i++) marks[i] = m[i];
 calculateResult(marks, total, percentage, grade); // reference variables
 assert(percentage >= 0 && percentage <= 100); // assertion
 marksEntered = true;
 }
 void enroll(int courseId)
 {
 if (enrolledCount >= MAX_ENROLL)
 throw LMSException("Student cannot enroll in more than 5 courses.");
 for (int i = 0; i < enrolledCount; i++)
 if (enrolled[i] == courseId)
 throw LMSException("Student is already enrolled in this course.");
 enrolled[enrolledCount++] = courseId;
 }
 bool hasMarks() const { return marksEntered; }
 float getPercentage() const { return percentage; }
 int getEnrolledCount() const { return enrolledCount; }
 // Operator overloading
 bool operator==(const Student &o) const { return id == o.id; }
 bool operator>(const Student &o) const { return percentage > o.percentage; }
 // Function overloading: display() and display(bool)
 void display() const override
 {
 cout << "\n[Student] ID: " << id << " | Name: " << name
 << " | Dept: " << deptName(dept) << " | Username: " << username();
 }
 void display(bool withMarks) const
 {
 display();
 if (withMarks)
 {
 if (marksEntered)
 cout << " | Total: " << total << " | %: " << fixed << setprecision(2)
 << percentage << " | Grade: " << grade;
 else
 cout << " | Marks: not entered";
 }
 }
 string getRole() const override { return "Student"; }
 void saveToFile(ofstream &fo) const override
 {
 fo << STUDENT << "\n" << id << "\n" << dept << "\n" << name << "\n";
 for (int i = 0; i < SUBJECTS; i++) fo << marks[i] << " ";
 fo << "\n" << marksEntered << "\n" << enrolledCount << "\n";
 for (int i = 0; i < enrolledCount; i++) fo << enrolled[i] << " ";
 fo << "\n";
 }
 friend void printReportCard(const Student &s); // friend function
};
// ---------------------------------------------------------------- TEACHER (inheritance)
class Teacher : public User
{
 string subject;
 int experience;
public:
 Teacher(int i, string n, Department d, string sub, int exp)
 : User(i, n, d), subject(sub), experience(exp) {}
 void display() const override
 {
 cout << "\n[Teacher] ID: " << id << " | Name: " << name
 << " | Dept: " << deptName(dept) << " | Subject: " << subject
 << " | Experience: " << experience << " yrs";
 }
 string getRole() const override { return "Teacher"; }
 void saveToFile(ofstream &fo) const override
 {
 fo << TEACHER << "\n" << id << "\n" << dept << "\n" << name << "\n"
 << subject << "\n" << experience << "\n";
 }
};
// ---------------------------------------------------------------- COURSE
class Course
{
 int courseId;
 string courseName;
 int credits;
 int teacherId;
 static int totalCourses;
public:
 Course() : courseId(0), courseName("Unknown"), credits(0), teacherId(0) {}
 Course(int id, string n, int c, int t)
 : courseId(id), courseName(n), credits(c), teacherId(t) { totalCourses++; }
 int getId() const { return courseId; }
 string getName() const { return courseName; }
 static int getTotalCourses() { return totalCourses; }
 void save(ofstream &fo) const
 {
 fo << courseId << "\n" << courseName << "\n" << credits << "\n" << teacherId << "\n";
 }
 friend void printCourse(const Course &c); // friend function
};
int Course::totalCourses = 0;
// ---------------------------------------------------------------- FRIEND FUNCTIONS
void printCourse(const Course &c)
{
 cout << "\n[Course] ID: " << c.courseId << " | " << c.courseName
 << " | Credits: " << c.credits << " | Teacher ID: " << c.teacherId;
}
void printReportCard(const Student &s)
{
 cout << "\n=============== REPORT CARD ===============";
 cout << "\nInstitution : " << ::institutionName; // scope resolution
 cout << "\nStudent : " << s.name << " (ID " << s.id << ")";
 cout << "\nDepartment : " << deptName(s.dept);
 cout << "\nEmail : " << s.email();
 cout << "\n-------------------------------------------";
 if (!s.marksEntered)
 {
 cout << "\nMarks have not been entered yet.\n";
 return;
 }
 for (int i = 0; i < SUBJECTS; i++)
 cout << "\n" << left << setw(24) << subjectNames[i] << right << setw(6) << s.marks[i];
 cout << "\n-------------------------------------------";
 cout << "\nTotal : " << s.total << " / " << SUBJECTS * 100;
 cout << "\nPercentage : " << fixed << setprecision(2) << s.percentage << " %";
 cout << "\nGrade : " << s.grade;
 cout << "\nResult : " << (s.percentage >= 40 ? "PASS" : "FAIL");
 cout << "\nEnrolled Courses (IDs): ";
 if (s.enrolledCount == 0) cout << "None";
 for (int i = 0; i < s.enrolledCount; i++) cout << s.enrolled[i] << " ";
 cout << "\n===========================================\n";
}
// ---------------------------------------------------------------- LMS MANAGER
class LMS
{
 User *users[MAX_USERS]; // array of base-class pointers (polymorphism)
 int userCount;
 Course *courses; // dynamic array
 int courseCount;
 Department readDepartment();
public:
 LMS();
 ~LMS();
 // Function overloading
 User *findUser(int id) const;
 User *findUser(const string &namePart) const;
 void addStudent();
 void addTeacher();
 void addCourse();
 void enrollStudent();
 void enterMarks();
 void showAllUsers() const;
 void showCourses() const;
 void searchUser() const;
 void reportCard() const;
 void compareStudents() const;
 void statistics() const;
 void saveData() const;
 void loadData();
};
// --- constructor / destructor (dynamic memory) ---
LMS::LMS() : userCount(0), courseCount(0)
{
 courses = new Course[MAX_COURSES];
 LOG("LMS created, course array allocated");
}
LMS::~LMS()
{
 for (int i = 0; i < userCount; i++)
 delete users[i]; // virtual destructor called
 delete[] courses;
 LOG("LMS destroyed, memory released");
}
Department LMS::readDepartment()
{
 int d = readValue<int>("Department (1-IT 2-CS 3-ENTC 4-MECH): ");
 if (d < IT || d > MECH)
 throw LMSException("Invalid department choice.");
 return Department(d);
}
User *LMS::findUser(int id) const
{
 for (int i = 0; i < userCount; i++)
 if (users[i]->getId() == id) return users[i];
 return NULL;
}
User *LMS::findUser(const string &namePart) const
{
 for (int i = 0; i < userCount; i++)
 if (users[i]->getName().find(namePart) != string::npos) return users[i];
 return NULL;
}
void LMS::addStudent()
{
 if (userCount >= MAX_USERS) throw LMSException("User limit reached.");
 int id = readValue<int>("Enter Student ID: ");
 if (id <= 0) throw LMSException("ID must be positive.");
 assert(id > 0);
 if (findUser(id) != NULL) throw LMSException("A user with this ID already exists.");
 string name = readLine("Enter Student Name: ");
 Department d = readDepartment();
 users[userCount++] = new Student(id, name, d); // dynamic allocation
 cout << "\nStudent added. Username: " << users[userCount - 1]->username()
 << " | Email: " << users[userCount - 1]->email();
}
void LMS::addTeacher()
{
 if (userCount >= MAX_USERS) throw LMSException("User limit reached.");
 int id = readValue<int>("Enter Teacher ID: ");
 if (id <= 0) throw LMSException("ID must be positive.");
 if (findUser(id) != NULL) throw LMSException("A user with this ID already exists.");
 string name = readLine("Enter Teacher Name: ");
 Department d = readDepartment();
 string subject = readLine("Enter Subject Taught: ");
 int exp = readValue<int>("Enter Experience (years): ");
 if (exp < 0) throw LMSException("Experience cannot be negative.");
 users[userCount++] = new Teacher(id, name, d, subject, exp);
 cout << "\nTeacher added successfully.";
}
void LMS::addCourse()
{
 if (courseCount >= MAX_COURSES) throw LMSException("Course limit reached.");
 int id = readValue<int>("Enter Course ID: ");
 for (int i = 0; i < courseCount; i++)
 if (courses[i].getId() == id) throw LMSException("Course ID already exists.");
 string name = readLine("Enter Course Name: ");
 int credits = readValue<int>("Enter Credits (1-6): ");
 if (credits < 1 || credits > 6) throw LMSException("Credits must be between 1 and 6.");
 int tid = readValue<int>("Enter Teacher ID: ");
 User *u = findUser(tid);
 if (u == NULL || dynamic_cast<Teacher *>(u) == NULL)
 throw LMSException("No teacher found with this ID.");
 courses[courseCount++] = Course(id, name, credits, tid);
 cout << "\nCourse added successfully.";
}
void LMS::enrollStudent()
{
 int sid = readValue<int>("Enter Student ID: ");
 Student *s = dynamic_cast<Student *>(findUser(sid));
 if (s == NULL) throw LMSException("Student not found.");
 int cid = readValue<int>("Enter Course ID: ");
 bool found = false;
 for (int i = 0; i < courseCount; i++)
 if (courses[i].getId() == cid) found = true;
 if (!found) throw LMSException("Course not found.");
 s->enroll(cid);
 cout << "\nEnrollment successful.";
}
void LMS::enterMarks()
{
 int sid = readValue<int>("Enter Student ID: ");
 Student *s = dynamic_cast<Student *>(findUser(sid));
 if (s == NULL) throw LMSException("Student not found.");
 float m[SUBJECTS];
 for (int i = 0; i < SUBJECTS; i++)
 m[i] = readValue<float>("Marks in " + subjectNames[i] + " (out of 100): ");
 s->setMarks(m);
 cout << "\nMarks recorded successfully.";
}
void LMS::showAllUsers() const
{
 if (userCount == 0) { cout << "\nNo users registered yet."; return; }
 int ch = readValue<int>("Show student marks as well? (1-Yes 0-No): ");
 cout << "\n------------- ALL USERS -------------";
 for (int i = 0; i < userCount; i++)
 {
 Student *s = dynamic_cast<Student *>(users[i]);
 if (s != NULL) s->display(ch == 1); // overloaded display(bool)
 else users[i]->display(); // runtime polymorphism
 }
 cout << "\n\nTotal users in system: " << User::getTotalUsers();
}
void LMS::showCourses() const
{
 if (courseCount == 0) { cout << "\nNo courses available."; return; }
 cout << "\n------------- COURSES -------------";
 for (int i = 0; i < courseCount; i++)
 printCourse(courses[i]);
 cout << "\n\nTotal courses: " << Course::getTotalCourses();
}
void LMS::searchUser() const
{
 int ch = readValue<int>("Search by 1-ID 2-Name: ");
 User *u = NULL;
 if (ch == 1) u = findUser(readValue<int>("Enter ID: "));
 else if (ch == 2) u = findUser(readLine("Enter name (or part of it): "));
 else throw LMSException("Invalid search option.");
 if (u == NULL) cout << "\nNo matching user found.";
 else { cout << "\nRole: " << u->getRole(); u->display(); }
}
void LMS::reportCard() const
{
 int sid = readValue<int>("Enter Student ID: ");
 Student *s = dynamic_cast<Student *>(findUser(sid));
 if (s == NULL) throw LMSException("Student not found.");
 printReportCard(*s);
}
void LMS::compareStudents() const
{
 Student *a = dynamic_cast<Student *>(findUser(readValue<int>("Enter first Student ID: ")));
 Student *b = dynamic_cast<Student *>(findUser(readValue<int>("Enter second Student ID:")));
 if (a == NULL || b == NULL) throw LMSException("One or both students not found.");
 if (!a->hasMarks() || !b->hasMarks()) throw LMSException("Marks missing for one of the students.");
 if (*a == *b) cout << "\nBoth IDs refer to the same student."; // operator==
 else if (*a > *b) cout << "\n" << a->getName() << " scored higher than " << b->getName(); //operator>
 else if (*b > *a) cout << "\n" << b->getName() << " scored higher than " << a->getName();
 else cout << "\nBoth students have the same percentage.";
}
void LMS::statistics() const
{
 Statistics<float> pct; // class template with float
 Statistics<int> enr; // class template with int
 Student *topper = NULL;
 for (int i = 0; i < userCount; i++)
 {
 Student *s = dynamic_cast<Student *>(users[i]);
 if (s == NULL) continue;
 enr.add(s->getEnrolledCount());
 if (s->hasMarks())
 {
 pct.add(s->getPercentage());
 if (topper == NULL || *s > *topper) topper = s;
 }
 }
 if (pct.count() == 0) { cout << "\nNo marks available for statistics."; return; }
 cout << fixed << setprecision(2);
 cout << "\n------------- CLASS STATISTICS -------------";
 cout << "\nStudents with marks : " << pct.count();
 cout << "\nHighest percentage : " << pct.highest();
 cout << "\nLowest percentage : " << pct.lowest();
 cout << "\nAverage percentage : " << pct.average();
 cout << "\nTopper : " << topper->getName() << " (ID " << topper->getId() << ")";
 cout << "\nMax enrollments : " << enr.highest();
 cout << "\nAvg enrollments : " << enr.average();
}
// --- File handling ---
void LMS::saveData() const
{
 ofstream fu("users.txt");
 if (!fu) throw LMSException("Cannot open users.txt for writing.");
 for (int i = 0; i < userCount; i++) users[i]->saveToFile(fu);
 fu.close();
 ofstream fc("courses.txt");
 if (!fc) throw LMSException("Cannot open courses.txt for writing.");
 for (int i = 0; i < courseCount; i++) courses[i].save(fc);
 fc.close();
 LOG("Data saved: " << userCount << " users, " << courseCount << " courses");
 cout << "\nData saved to users.txt and courses.txt";
}
void LMS::loadData()
{
 ifstream fu("users.txt");
 if (fu)
 {
 int role;
 while (userCount < MAX_USERS && fu >> role)
 {
 int id, d;
 fu >> id >> d;
 fu.ignore(10000, '\n');
 string name;
 getline(fu, name);
 if (role == STUDENT)
 {
 float m[SUBJECTS];
 int me, cnt;
 for (int i = 0; i < SUBJECTS; i++) fu >> m[i];
 fu >> me >> cnt;
 Student *s = new Student(id, name, Department(d));
 if (me == 1) s->setMarks(m);
 for (int i = 0; i < cnt; i++)
 {
 int cid;
 fu >> cid;
 s->enroll(cid);
 }
 users[userCount++] = s;
 }
 else
 {
 string subject;
 int exp;
 getline(fu, subject);
 fu >> exp;
 users[userCount++] = new Teacher(id, name, Department(d), subject, exp);
 }
 }
 fu.close();
 }
 ifstream fc("courses.txt");
 if (fc)
 {
 int id, cr, tid;
 while (courseCount < MAX_COURSES && fc >> id)
 {
 fc.ignore(10000, '\n');
 string name;
 getline(fc, name);
 fc >> cr >> tid;
 courses[courseCount++] = Course(id, name, cr, tid);
 }
 fc.close();
 }
 cout << "Loaded " << userCount << " user(s) and " << courseCount << " course(s) from file.\n";
}
// ---------------------------------------------------------------- UI HELPERS
void printBanner()
{
 string title = "EDUCATION & LEARNING MANAGEMENT SYSTEM";
 cout << "\n=============================================";
 cout << "\n " << title;
 cout << "\n " << ::institutionName; // global via scope resolution
 cout << "\n=============================================\n";
}
void printMenu()
{
 cout << "\n\n------------------- MENU -------------------";
 cout << "\n 1. Add Student 2. Add Teacher";
 cout << "\n 3. Add Course 4. Enroll Student in Course";
 cout << "\n 5. Enter Student Marks 6. Display All Users";
 cout << "\n 7. Display Courses 8. Search User";
 cout << "\n 9. Student Report Card 10. Compare Two Students";
 cout << "\n11. Class Statistics 12. Save Data";
 cout << "\n 0. Save & Exit";
 cout << "\n--------------------------------------------\n";
}
bool authenticate(const string &correct)
{
 for (int attempt = 1; attempt <= 3; attempt++)
 {
 string p = readLine("Enter admin password: ");
 if (p.compare(correct) == 0) return true;
 cout << "Wrong password. Attempts left: " << 3 - attempt << "\n";
 }
 return false;
}
// ---------------------------------------------------------------- MAIN (command-line arguments)
int main(int argc, char *argv[])
{
 string adminPassword = "admin123";
 if (argc > 1) ::institutionName = argv[1]; // argv[1] -> institution name
 if (argc > 2) adminPassword = argv[2]; // argv[2] -> admin password
 printBanner();
 LMS lms; // object created; destructor frees memory
 try
 {
 if (!authenticate(adminPassword))
 {
 cout << "Too many wrong attempts. Exiting.\n";
 return 1;
 }
 lms.loadData();
 }
 catch (LMSException &e)
 {
 cout << "\n[Error] " << e.what() << "\n";
 return 1;
 }
 int choice = -1;
 do
 {
 try
 {
 printMenu();
 choice = readValue<int>("Enter your choice: ");
 LOG("Menu choice = " << choice);
 switch (choice)
 {
 case 1: lms.addStudent(); break;
 case 2: lms.addTeacher(); break;
 case 3: lms.addCourse(); break;
 case 4: lms.enrollStudent(); break;
 case 5: lms.enterMarks(); break;
 case 6: lms.showAllUsers(); break;
 case 7: lms.showCourses(); break;
 case 8: lms.searchUser(); break;
 case 9: lms.reportCard(); break;
 case 10: lms.compareStudents(); break;
 case 11: lms.statistics(); break;
 case 12: lms.saveData(); break;
 case 0: lms.saveData(); cout << "\nThank you for using the LMS. Goodbye!\n"; break;
 default: cout << "\nInvalid choice. Please select 0-12.";
 }
 }
 catch (LMSException &e)
 {
 cout << "\n[Error] " << e.what();
 if (e.what() == "Input stream closed.") break;
 }
 catch (...)
 {
 cout << "\n[Error] Unexpected error occurred.";
 }
 } while (choice != 0);
 return 0;
}
