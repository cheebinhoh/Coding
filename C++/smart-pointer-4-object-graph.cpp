#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

private:
  std::string m_name{};
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

  void add(std::unique_ptr<Student> student) {
    // delete: we want to keep a list of students than one.
    // m_student = std::move(student);

    m_students.push_back(std::move(student));
  }

private:
  std::string m_name{};
  std::vector<std::unique_ptr<Student>> m_students{};

  // delete: we want to keep a list of students than one.
  // std::unique_ptr<Student> m_student;
};

int main(int argc, char *argv[]) {
  University uni{"University of southern Queensland"};

  std::cout << "\n"
            << "Enter inner block\n";
  {
    auto cb{std::make_unique<Student>("Chee Bin HOH")};
    auto cs{std::make_unique<Student>("Miow Miow")};

    uni.add(std::move(cb));
    std::cout << "After add student (cb)\n";

    uni.add(std::move(cs));
    std::cout << "After add student (cs)\n";
  }
  std::cout << "Exit inner block\n\n";

  return 0;
}
