#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

///< 列表类的预先引用
class CourseList;
class StudentList;
class OfferingList;

///< 程序中用到的常量
const int name_len = 30;
const int desc_len = 128;
const int course_len = 30;
const int student_len = 50;
const int small_strlen = 15;

/* 科目有 名字, 描述, 课时长度, 先修科目*/
class Course {
private:
    char name[name_len];
    char description[desc_len];
    int duration;
    CourseList* prereq;
    int reference_count;

public:
    Course(char*, char*, int, int, ...);
    Course(const Course&);
    ~Course();

    ///< 增加计数器值
    int attach_object();
    /* 减少计数器值, 如果其返回值为0, 那么调用者就知道 
       它是这个科目对象的最后一个调用者, 调用科目对象的析构函数 */
    int detach_object();
    void add_prereq(Course&);
    int check_prereq(CourseList&);
    void print();
    void short_print();
    int are_you(char*);
};

/* 每个关键抽象都有一个对应的列表类来维护列表操作 */
class CourseList {
private:
    Course **courses;
    int size;
    int course_num;

public:
    CourseList(int);
    CourseList(CourseList&);
    ~CourseList();
    int add_item(Course&);
    Course* find_item(char*);
    int find_all(CourseList&);
    void print();
};

/* 科目的构造函数要求以下参数: 名字, 描述, 课时长度, 先修科目的可变长度列表 */
Course::Course(char* n, char* d, int len, int pnum, ...)
{
    int i;
    ///< 可变参数宏
    va_list ap;

    strncpy(name, n, name_len);
    strncpy(description, d, desc_len);

    duration = len;
    prereq = new CourseList(course_len);
    reference_count = 1;

    if (pnum) {
        va_start(ap, pnum);
        for (i = 0; i < pnum; ++i) {
            prereq->add_item(*va_arg(ap, Course*));
        }
        va_end(ap);
    }
}

/* 科目的拷贝构造函数拷贝所有的字符串, 并调用科目列表的拷贝构造函数*/
Course::Course(const Course& rhs) 
{
    strcpy(name, rhs.name);
    strcpy(description, rhs.description);
    duration = rhs.duration;
    prereq = new CourseList(*rhs.prereq);
    reference_count = rhs.reference_count;
}

/*科目的析构函数删除了它的所有先修科目, 
  并检查以确保调用 delete 删除科目对象的是科目最后一个使用者.*/
Course::~Course() 
{
    delete prereq;
    if (reference_count > 1) {
        std::cout << "Error> A course object destroyed with ";
        std::cout << reference_count << " other objects referencing it. \n";
    }
}

/* 每个应用科目对象的对象都必须调用 attach_objcet 来向该对象注册自身*/
int Course::attach_object()
{
    return (++reference_count);
}

/* 每个调用过 attach_objcet 的对象都必须在析构函数中调用 detach_objcet 来减少引用计数*/
int Course::detach_object()
{
    return (--reference_count);
}

/* 为了给科目增加一门选修科目, 我们调用科目列表的 add_item 方法,
   这个方法若无法增加这门科目(列表已满), 那么返回0*/
void Course::add_prereq(Course& new_prereq) 
{
    if (prereq->add_item(new_prereq) == 0) {
        std::cout << "Error: Cannot add any new prepequisites.\n";
    }
}

void Course::print()
{
    using std::cout;
    cout << "\n\nCourse: " << name << "\n";
    cout << "Description: " << description << "\n";
    cout << "Duration: " << duration << "\n";
    cout << "List of Prerequisites: ";
    prereq->print();
    cout << "\n\n";
}

/* short_print 方法用在我们指向看到科目的名字而不想看到科目的关联信息的场合*/
void Course::short_print()
{
    std::cout << name;
}

/* 科目对象收到一个科目列表, 并调用 CourseList::find_all 方法来检查先修科目. 
   这个方法检查是否参数列表中的所有科目都在消息所发送至的列表中*/
int Course::check_prereq(CourseList& courses_taken)
{
    return (courses_taken.find_all(*prereq));
}

/* 这个方法检查它的名字是否等于传递进来的名字,
   这是用来根据名字从一个科目列表中找到一门特定的科目*/
int Course::are_you(char* guess_name) 
{
    return (!strcmp(name, guess_name));
}

CourseList::CourseList(int sz)
{
    course_num = 0;
    courses = new Course*[size = sz];
}

/*每个科目只是被引用, 而不是被复制*/
CourseList::CourseList(CourseList& rhs)
{
    int i;
    courses = new Course*[size=rhs.size];
    for (i = 0; i < size; ++i) {
        courses[i] = rhs.courses[i];
        courses[i]->attach_object();
    }
    course_num = rhs.course_num;
}

/* 科目列表的析构函数释放先修科目列表中的每个对象, 如果哪次调用 detach_object 方法使得引用计数为0,
   那么这个科目列表就是使用该科目的最后一个对象, 应当调用科目的析构函数*/
CourseList::~CourseList()
{
    int i;
    for (i = 0; i < course_num; ++i) {
        if (courses[i]->detach_object() == 1) {
            delete courses[i];
        }
    }
    delete courses;
}

/* add_item 方法检查以确保列表还有空间*/
int CourseList::add_item(Course& new_item)
{
    if (course_num == size) {
        return 0;
    } else {
        courses[course_num++] = &new_item;
        new_item.attach_object();
    }

    return 1;
}

/* 在课程列表中找出匹配用户传递的名称的课程, 如果没有找到, 那么该方法返回空指针*/
Course* CourseList::find_item(char* guess_name)
{
    int i;
    for (i = 0; i < course_num; ++i) {
        if (courses[i]->are_you(guess_name)) {
            return courses[i];
        }
    }
    return NULL;
}

/* 该方法检查待查找列表中的所有科目对象是否都存在于消息所发送至的列表中, 
   因为这些列表中的科目都是前拷贝, 我们只需要检查科目对象的地址, 而不需要比较科目名称 */
int CourseList::find_all(CourseList& findlist)
{
    int i, j, found;

    for (i = 0; i < findlist.course_num; ++i) {
        found = 0;
        for (j = 0; j < course_num && !found; ++j) {
            if (findlist.courses[i] == courses[j]) {
                found = 1;
            }
        }
        if (!found) {
            return 0;
        }
    }
    return 1;
}

void CourseList::print()
{
    int i;
    std::cout << "\n\n";

    for (i = 0; i < course_num; ++i) {
        courses[i]->short_print();
        std::cout << " ";
    }
    std::cout << "\n\n";
}


/* 学生有姓名, 社保号码, 年龄, 类似于科目对象, 学生也有一个科目清单, 引用计数的工作方式同科目类一模一样 */
class Student {
private:
    char name[name_len];
    char ssn[small_strlen];
    int age;
    CourseList* courses;
    int reference_count;

public:
    Student(char*, char*, int, int, ...);
    Student(const Student&);
    ~Student();
    int attach_object();
    int detach_object();
    void add_course(Course&);
    CourseList& get_courses();
    void print();
    void short_print();
    int are_you(char*);
};

/* 学生列表同科目列表一样, 唯一不同之处是他用来处理学生对象, 而不是科目对象 */
class StudentList {
private:
    Student **students;
    int size;
    int student_num;

public:
    StudentList(int);
    StudentList(StudentList&);
    ~StudentList();
    int add_item(Student&);
    Student* find_item(char*);
    void print();
};

Student::Student(char* n, char* s, int a, int num, ...)
{
    int i;
    va_list ap;

    strncpy(name, n, name_len);
    strncpy(ssn, s, small_strlen);
    age = a;
    courses = new CourseList(course_len);
    reference_count = 1;
    if (num) {
        va_start(ap, num);
        for (i = 0; i < num; ++i) {
            courses->add_item(*va_arg(ap, Course*));
        }
        va_end(ap);
    }
}

Student::Student(const Student& rhs)
{
    strcpy(name, rhs.name);
    strcpy(ssn, rhs.ssn);
    age = rhs.age;
    courses = new CourseList(*rhs.courses);
    reference_count = rhs.reference_count;
}

Student::~Student()
{
    delete courses;
}

int Student::attach_object()
{
    return (++reference_count);
}

int Student::detach_object()
{
    return (--reference_count);
}

void Student::add_course(Course& c)
{
    if (courses->add_item(c) == 0) {
        std::cout << "Cannot add any new courses to the Sutdent.\n";
    }
}

/* 我们需要一个访问方法 */
CourseList& Student::get_courses()
{
    return *courses;
}

void Student::print()
{
    using std::cout;
    cout << "\n\nName: " << name << "\n";
    cout << "SSN: " << ssn << "\n";
    cout << "Age " << age << "\n";
    cout << "Prerequisites: ";
    courses->print();
    cout << "\n\n";
}

void Student::short_print()
{
    std::cout << name;
}

int Student::are_you(char* guess_name)
{
    return (!strcmp(name, guess_name));
}

StudentList::StudentList(int sz)
{
    student_num = 0;
    students = new Student*[size=sz];
}

StudentList::StudentList(StudentList& rhs)
{
    int i;
    students = new Student*[size=rhs.size];
    for (i = 0; i < size; ++i) {
        students[i] = rhs.students[i];
        students[i]->attach_object();
    }
    student_num = rhs.student_num;
}

StudentList::~StudentList()
{
    int i;
    for (i = 0; i < student_num; ++i) {
        if (students[i]->detach_object() == 1) {
            delete students[i];
        }
    }
    delete students;
}

int StudentList::add_item(Student& new_item)
{
    if (student_num == size) {
        return 0;
    } else {
        students[student_num++] = &new_item;
        new_item.attach_object();
    }
    return 1;
}

Student* StudentList::find_item(char* guess_name) 
{
    int i;
    for (i = 0; i < student_num; ++i) {
        if (students[i]->are_you(guess_name)) {
            return students[i];
        }
    }
    return NULL;
}

void StudentList::print()
{
    int i;
    for (i = 0; i < student_num; ++i) {
        students[i]->short_print();
        std::cout << " ";
    }
}

/* 课程类表示了这样的关系, 某个科目, 在某个教室中, 在某个特定的日期被讲授, 同一组特定的
   学生的关系, 这不是一个应用计数类, 因为我们从来不在多个列表中共享课程对象*/
class CourseOffering {
private:
    Course* course;
    char room[small_strlen];
    char date[small_strlen];
    StudentList* attendees;

public:
    CourseOffering(Course&, char*, char*);
    CourseOffering(const CourseOffering&);
    ~CourseOffering();
    void add_student(Student&);
    void print();
    void short_print();
    int are_you(char*, char*);
};

CourseOffering::CourseOffering(Course& c, char* r, char* d)
{
    course = &c;
    course->attach_object();
    strncpy(room, r, small_strlen);
    strncpy(date, d, small_strlen);
    attendees = new StudentList(student_len);
}

CourseOffering::CourseOffering(const CourseOffering& rhs)
{
    course = rhs.course;
    course->attach_object();
    strcpy(room, rhs.room);
    strcpy(date, rhs.date);
    attendees = new StudentList(*rhs.attendees);
}

CourseOffering::~CourseOffering()
{
    if (course->detach_object() == 1) {
        delete course;
    }
    delete attendees;
}

/* 课程确保选课的新生已经修过必要的先修课程, 这是通过获取该学生已经修过的科目清单并将之
   传递给 check_prereq 方法来实现的, 课程可以检查学生是否已经修过所有要求的先修科目, 
   因为课程已经有了先修科目列表, 并通过调用学生的 get_courses 方法来获得了科目列表*/
void CourseOffering::add_student(Student& new_student)
{
    if (course->check_prereq(new_student.get_courses())) {
        attendees->add_item(new_student);
        std::cout << "Student added to course.\n";
    } else {
        std::cout << "Admission refused: Student does not hava the ";
        std::cout << "necessary prerequisites\n";
    }
}

void CourseOffering::print()
{
    using std::cout;

    cout << "\n\nThe course offering for ";
    course->short_print();
    cout << " will be held in room " << room << " starting on ";
    cout << date << "\n";
    cout << "Current attendees include: ";
    attendees->print();
    cout << "\n\n";
}

void CourseOffering::short_print()
{
    course->short_print();
    std::cout << " (" << date << ") ";
}

/* 在比较课程时, 比较科目名还不够, 还需要比较日期 */
int CourseOffering::are_you(char* guess_name, char* guess_date)
{
    return (!strcmp(guess_date, date) && course->are_you(guess_name));
}

/* 课程列表类类似于学生列表和科目列表类 */
class OfferingList {
private:
    CourseOffering **offerings;
    int size;
    int offering_num;

public:
    OfferingList(int);
    OfferingList(OfferingList&);
    ~OfferingList();
    int add_item(CourseOffering&);
    CourseOffering* find_item(char*, char*);
    void print();
};

OfferingList::OfferingList(int sz)
{
    offering_num = 0;
    offerings = new CourseOffering*[size=sz];
}

OfferingList::OfferingList(OfferingList& rhs)
{
    int i;

    offerings = new CourseOffering*[size=rhs.size];
    for (i = 0; i < size; ++i) {
        offerings[i] = rhs.offerings[i];
    }
    offering_num = rhs.offering_num;
}

OfferingList::~OfferingList()
{
    int i;
    for (i = 0; i < offering_num; ++i) {
        delete offerings[i];
    }
    delete offerings;
}

int OfferingList::add_item(CourseOffering& new_item)
{
    if (offering_num == size) {
        return 0;
    } else {
        offerings[offering_num++] = &new_item;
    }
    return 1;
}

CourseOffering* OfferingList::find_item(char* guess_name, char* date)
{
    int i;
    for (i = 0; i < offering_num; ++i) {
        if (offerings[i]->are_you(guess_name, date)) {
            return offerings[i];
        }
    }
    return NULL;
}

void OfferingList::print()
{
    int i;
    for (i = 0; i < offering_num; ++i) {
        offerings[i]->short_print();
        std::cout << " ";
    }
}

/* 出程序是一个简单的菜单驱动系统 */
int main()
{
    CourseList courses(50);
    StudentList students(50);
    OfferingList offerings(50);

    Course *course1, *course2;
    Student *student;
    CourseOffering *offer1;
    int duration, age, choice;
    char answer[128], name[40], description[128], course_name[50];
    char ssn[20], date[20], room[20];
    char c;

    do {
        using std::cout;
       
        cout << "What would you like to do? \n";
        cout << "  1) Build a new course\n";
        cout << "  2) Bulid a new student\n";
        cout << "  3) Build a new course offering\n";
        cout << "  4) List courses\n";
        cout << "  5) List students\n";
        cout << "  6) List offerings\n";
        cout << "  7) Add a prerequisite to a course\n";
        cout << "  8) Add a course to a student\n";
        cout << "  9) Add a student to a course offering\n";
        cout << " 10) Detailed info on a course\n";
        cout << " 11) Detailed info on a student\n";
        cout << " 12) Detailed info on an offering\n";
        cout << "  q) Quit\n";
        cout << "\nYour Choice: ";

        using std::cin;
        cin.getline(answer, 128);
        choice = atoi(answer);
        switch (choice) {
        case 1:
            cout << "Enter Name: ";
            cin.getline(name, 40);
            cout << "Enter Description: ";
            cin.getline(description, 128);
            cout << "Enter Length of Course: ";
            cin >> duration;
            courses.add_item(*new Course(name, description, duration, 0));
            cin.get(c);
            break;
        case 2:
            cout << "Enter name: ";
            cin.getline(name, 40);
            cout << "Enter ssn: ";
            cin.getline(ssn, 20);
            cout << "Enter age: ";
            cin >> age;
            students.add_item(*new Student(name, ssn, age, 0));
            cin.get(c);
            break;
        case 3:
            cout << "Enter course: ";
            cin.getline(course_name, 50);
            course1 = courses.find_item(course_name);
            if (course1 == NULL) {
                cout << "Sorry, Cannot find that course.\n";
                break;
            }
            cout << "Enter room: ";
            cin.getline(room, 20);
            cout << "Enter date: ";
            cin.getline(date, 20);
            offerings.add_item(*new CourseOffering(*course1, room, date));
            break;
        case 4:
            cout << "\nList of courses: \n";
            courses.print();
            cout << "\n\n";
            break;
        case 5:
            cout << "\nList of students: \n";
            students.print();
            cout << "\n\n";
            break;
        case 6:
            cout << "\nList of Offerings: \n";
            offerings.print();
            cout << "\n\n";
            break;
        case 7:
            cout << "To which course? ";
            cin.getline(course_name, 50);
            course1 = courses.find_item(course_name);
            if (course1 == NULL) {
                cout << "Sorry, Cannot find that course.\n";
                break;
            }
            cout << "Which prerequisite? ";
            cin.getline(course_name, 50);
            course2 = courses.find_item(course_name);
            if (course2 == NULL) {
                cout << "Sorry, Cannot find that course.\n";
                break;
            }
            course1->add_prereq(*course2);
            break;
        case 8:
            cout << "To Which Student? ";
            cin.getline(name, 40);
            student = students.find_item(name);
            if (student == NULL) {
                cout << "Sorry, Cannot find that student.\n";
                break;
            }
            cout << " Which Course ? ";
            cin.getline(course_name, 50);
            course1 = courses.find_item(course_name);
            if (course1 == NULL) {
                cout << "Sorry, Cannot find that course.\n";
                break;
            }
            student->add_course(*course1);
            break;
        case 9:
            cout << " To which course ? ";
            cin.getline(course_name, 50);
            cout << " On which date? ";
            cin.getline(date, 20);
            offer1 = offerings.find_item(course_name, date);
            if (offer1 == NULL) {
                cout << " Sorry, Cannot find that course offering.\n";
                break;
            }
            cout << " Which Student? ";
            cin.getline(name, 40);
            student = students.find_item(name);
            if (student == NULL) {
                cout << "Sorry, Cannot find that student.\n";
                break;
            }
            offer1->add_student(*student);
            break;
        case 10:
            cout << " On Which Course ? ";
            cin.getline(course_name, 50);
            course1 = courses.find_item(course_name);
            if (course1 == NULL) {
                cout << "Sorry, Cannot find that course.\n";
                break;
            }
            course1->print();
            break;
        case 11:
            cout << "On Which Sutdent? ";
            cin.getline(name, 40);
            student = students.find_item(name);
            if (student == NULL) {
                cout << "Sorry, Cannot find that student.\n";
                break;
            }
            student->print();
            break;
        case 12:
            cout << " On Which Course ? ";
            cin.getline(course_name, 50);
            cout << " Which date? ";
            cin.getline(date, 20);
            offer1 = offerings.find_item(course_name, date);
            if (offer1 == NULL) {
                cout << "Sorry, Cannot find that course offering.\n";
                break;
            }
            offer1->print();
            break;
        }

    } while (answer[0] >= '1' && answer[0] <= '9');
}



