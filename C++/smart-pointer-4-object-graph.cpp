#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Course {
public:
  Course(std::string_view name) : m_name{name} {
    std::cout << "Initialize Course object with name: " << m_name << "\n";
  }

  Course() : Course("unknown") {
    std::cout << "Initialize (default) Course object with name: unknown\n";
  }

  ~Course() {
    std::cout << "Deinitializwe Course object with name: " << m_name << "\n";
  }

  std::string_view getName() const { return m_name; }

private:
  std::string m_name{};
};

class Student {
public:
  Student(std::string_view name) : m_name{name} {
    std::cout << "Initialize Student object with name: " << m_name << "\n";
  }

  Student() : Student{"unknown"} {
    std::cout << "Initialize (default) Student object with name: unknown\n";
  }

  Student(const Student &student) : m_name{student.m_name} {
    std::cout << "Initialize (copy) Student object with name: " << m_name
              << "\n";
  }

  ~Student() {
    std::cout << "Deinitializwe Student object with name: " << m_name << "\n";
  }

  void add(std::shared_ptr<Course> course) {
    auto course_found = std::find_if(
        m_courses.begin(), m_courses.end(),
        [course_name = course->getName()](std::weak_ptr<Course> course) {
          return (!course.expired()) && course.lock()->getName() == course_name;
        });

    if (course_found == m_courses.end()) {
      std::cout << "add Course (" << course->getName() << ") to student ("
                << m_name << ")\n";

      std::weak_ptr<Course> new_course{course};
      m_courses.push_back(new_course);
    } else {
      std::cout << "Course (" << course->getName() << ") already taken\n";
    }
  }

private:
  std::string m_name{};
  std::vector<std::weak_ptr<Course>> m_courses{};
};

class University {
public:
  University(std::string_view name) : m_name{name} {
    std::cout << "Initialize University object with name: " << m_name << "\n";
  }

  University() : University{"unknown"} {}

  ~University() {
    std::cout << "Deinitialize University object with name: " << m_name << "\n";
  }

  void add(std::unique_ptr<Student> student, std::string_view course_name) {
    // delete: we want to keep a list of students than one.
    // m_student = std::move(student);

    auto course_found =
        std::find_if(m_courses.begin(), m_courses.end(),
                     [course_name](std::shared_ptr<Course> course) {
                       return course->getName() == course_name;
                     });

    if (course_found != m_courses.end()) {
      std::cout << "Course (" << (*course_found)->getName() << ") found\n";
      student->add(*course_found);
    } else {
      std::cout << "Course (" << course_name << ") not found\n";
    }

    m_students.push_back(std::move(student));
  }

  void add(std::shared_ptr<Course> course) {
    m_courses.push_back(std::move(course));
  }

private:
  std::string m_name{};
  std::vector<std::unique_ptr<Student>> m_students{};
  std::vector<std::shared_ptr<Course>> m_courses{};

  // delete: we want to keep a list of students than one.
  // std::unique_ptr<Student> m_student;
};

int main(int argc, char *argv[]) {
  University uni{"University of southern Queensland"};

  std::cout << "\n"
            << "Enter inner block\n";
  {
    auto compscience{std::make_shared<Course>("compscience")};
    auto education{std::make_shared<Course>("education")};

    auto cb{std::make_unique<Student>("Chee Bin HOH")};
    auto cs{std::make_unique<Student>("Miow Miow")};

    std::cout << "\n";
    std::cout << "After add course (" << compscience->getName() << ")\n";
    uni.add(std::move(compscience));
    std::cout << "After add course (" << education->getName() << ")\n";
    uni.add(std::move(education));

    std::cout << "\n";
    std::cout << "After add student (Chee Bin HOH) with compscience\n";
    uni.add(std::move(cb), "compscience");

    std::cout << "After add student (Miow Miow) with compscience\n";
    uni.add(std::move(cs), "compscience");
  }
  std::cout << "Exit inner block\n\n";

  return 0;
}
