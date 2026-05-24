/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * The purpose of this program is to demonstrate the use of std-reference
 * in std container class (e.g std::vector) vs copy each element into
 * std container class.
 *
 * An efficient object graph is that there is a parent child ownership
 * reference that a parent object owns a set of child objects through
 * raw pointer or smart pointer, and then if different objects under the
 * same object graph will have to reference to each through std container
 * class, we can store the reference to avoid copying of object or store
 * id that can reference the object through root object in the graph.
 *
 * The alternatives are to use:
 * - smart pointer (std::weak_ptr or std::shared_ptr) with std container
 *   class.
 *
 * - object will store id of another object under the same object graph,
 *   so any reference to the detail of the referenced object will be
 *   resolved through the object graph' root.
 *
 * A lot of mechanism needed to add to object graph management code to
 * maintain integrity of the reference, example if object A references
 * object B under the same object graph, if B is removed, how do we reset
 * the reference from A? do we do it actively or only when the reference
 * is accessed.
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Course {
public:
  Course(std::string_view name) : m_name{name} {
    std::cout << "Initialize Course object with name: " << m_name << "\n";
  }

  Course(const Course &copy) {
    m_name = copy.m_name;

    std::cout << "Initialize Course object (copy) with name: " << m_name
              << "\n";
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

int main(int argc, char *argv[]) {
  Course compscience{"Computer Science"};
  Course education{"Education"};
  Course dup_compscience{compscience};

  std::cout << "\n";
  std::cout << "Before enter nested block and create std::vector of Course\n";
  {
    std::vector<Course> course_list{};

    std::cout << "Add course \"Computer Science\" to std::vector\n";
    course_list.push_back(compscience);

    // if we see two copy-constructor, one for computer science and
    // another for education, it is because c++ lbrary resize the
    // vector memory and move previously added computer science course
    // to new list and then add education course, hence two move copied.
    std::cout << "Add course \"Education\" to std::vector\n";
    course_list.push_back(education);

    std::cout << "\n";
    std::cout << "iterate the list of course\n";
    for (const auto c : course_list) {
      std::cout << "  " << c.getName() << "\n";
    }
    std::cout << "End of iteration\n";

    std::cout << "\n";
  }
  std::cout << "After exit nested block and destroy std::vector of Course\n";

  std::cout << "\n";
  std::cout << "Before enter nested block and create std::vector of "
               "std::reference_wrapper<Course>\n";
  {
    std::vector<std::reference_wrapper<Course>> course_list{};

    std::cout << "Add course \"Computer Science\" to std::vector\n";
    course_list.push_back(compscience);

    // if we see two copy-constructor, one for computer science and
    // another for education, it is because c++ lbrary resize the
    // vector memory and move previously added computer science course
    // to new list and then add education course, hence two move copied.
    std::cout << "Add course \"Education\" to std::vector\n";
    course_list.push_back(education);

    std::cout << "\n";
    std::cout << "iterate the list of course\n";
    for (const auto c : course_list) {
      std::cout << "  " << c.get().getName() << "\n";
    }
    std::cout << "End of iteration\n";

    std::cout << "\n";
  }
  std::cout << "After exit nested block and destroy std::vector of "
               "std::reference_wrapper<Course>\n";

  std::cout << "\n";
  std::cout << "Before exit main()\n";

  return 0;
}
