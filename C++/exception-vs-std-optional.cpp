/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * The purpose of this program is to demonstrate the use of exception
 * vs std::optional to indicate run time error.
 *
 * The example extends the University object graph we have and add the
 * following two methods:
 *
 * - std::unique_ptr<Student>
 *     University.delStudentWithException(std::string_view name);
 *   where it throws exception UniversityException_StudentNotExist() which is
 * inheritted from std::runtime_error when student is not found.
 *
 * - std::optional<std::unique_ptr<Student>>
 *     University.delStudentWithOptional(std::string_view name);
 *   where it deletes student from Univeristy object graph and return its
 * pointer reference (smart pointer) via std::optional which is empty if such
 * student is not found.
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
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
    std::cout << "Deinitializwe Student object with name: " << m_name;
    for (auto course : m_courses) {
      if (!course.expired()) {
        std::cout << ", course: " << course.lock()->getName();
      } else {
        std::cout << ", course: unknown";
      }
    }

    std::cout << "\n";
  }

  std::string_view getName() const { return m_name; }

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

class UniversityException_StudentNotExist : public std::runtime_error {
public:
  UniversityException_StudentNotExist(std::string_view student_name)
      : std::runtime_error{""}, m_student_name{student_name} {

    m_message = m_student_name + " is not found";
  }

  const char *what() const noexcept override { return m_message.c_str(); }

private:
  std::string m_student_name;
  std::string m_message;
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

  void delStudent(std::string_view student_name) {
    std::cout << "Before deleting, # of students: " << m_students.size()
              << "\n";

    auto found =
        std::find_if(m_students.begin(), m_students.end(),
                     [student_name](std::unique_ptr<Student> &student) {
                       return (student->getName() == student_name);
                     });
    if (found != m_students.end()) {
      m_students.erase(found);
    }

    std::cout << "After deleting, # of students: " << m_students.size() << "\n";
  }

  std::unique_ptr<Student>
  delStudentWithException(std::string_view student_name) {
    std::unique_ptr<Student> ret{};

    std::cout << "Before deleting, # of students: " << m_students.size()
              << "\n";

    auto found =
        std::find_if(m_students.begin(), m_students.end(),
                     [student_name](std::unique_ptr<Student> &student) {
                       return (student->getName() == student_name);
                     });
    if (found == m_students.end()) {
      throw UniversityException_StudentNotExist{student_name};
    } else {
      ret = std::move(*found);
      m_students.erase(found);
    }

    std::cout << "After deleting, # of students: " << m_students.size() << "\n";

    return ret;
  }

  std::optional<std::unique_ptr<Student>>
  delStudentWithOptional(std::string_view student_name) {
    std::unique_ptr<Student> ret{};

    std::cout << "Before deleting, # of students: " << m_students.size()
              << "\n";

    auto found =
        std::find_if(m_students.begin(), m_students.end(),
                     [student_name](std::unique_ptr<Student> &student) {
                       return (student->getName() == student_name);
                     });
    if (found == m_students.end()) {
      return {};
    } else {
      ret = std::move(*found);
      m_students.erase(found);
    }

    std::cout << "After deleting, # of students: " << m_students.size() << "\n";

    return std::move(ret);
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
  auto compscience{std::make_shared<Course>("compscience")};
  auto education{std::make_shared<Course>("education")};

  auto cb{std::make_unique<Student>("Chee Bin HOH")};
  auto cs{std::make_unique<Student>("Miow Miow")};
  auto other{std::make_unique<Student>("Miss X")};

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

  std::cout << "After add student (Miss X) with education\n";
  uni.add(std::move(other), "education");

  std::cout << "\n";
  std::cout << "Before enter nested block to delete student\n";
  {
    std::unique_ptr<Student> delStudent{};

    std::cout << "deleting student \"Miss X\"\n";
    delStudent = uni.delStudentWithException("Miss X");
    if (delStudent) {
      std::cout << "Deleted student name (" << delStudent->getName() << ")\n";
    }

    std::cout << "\n";
    std::cout << "deleting student \"Mr X\" which does not exist\n";
    try {
      std::unique_ptr<Student> delStudent{};

      delStudent = uni.delStudentWithException("Mr X");
    } catch (const UniversityException_StudentNotExist &except) {
      std::cout << "Exception catch: " << except.what() << "\n";
    } catch (...) {
      std::cout << "other exception catch\n";
    }

    {
      std::cout << "\n";
      std::cout << "deleting student \"Mr Y\" which does not exist\n";

      if (auto delStudent{uni.delStudentWithOptional("Mr Y")}) {

      } else {
        std::cout << "Mr Y is not found\n";
      }
    }

    {
      std::cout << "\n";
      std::cout << "deleting student \"Miow Miow\" which does exist\n";

      if (auto delStudent{uni.delStudentWithOptional("Miow Miow")}) {
        std::cout << "Miow Miow is found and will be deleted\n";
      } else {
        std::cout << "Mr Y is not found\n";
      }
    }

    std::cout << "\n";
  }
  std::cout << "After exit nested block to delete student\n\n";

  return 0;
}
