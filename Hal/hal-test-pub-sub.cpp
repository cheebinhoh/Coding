#include "hal-pub-sub.hpp"

#include <iostream>
#include <string>
#include <thread>

class Hal_Msg_Receiver : public Hal_Pub<std::string>::Hal_Sub {
public:
  Hal_Msg_Receiver(std::string_view name) : m_name{name} {}

  ~Hal_Msg_Receiver() {}

  void notify(std::string item) override {
    std::cout << m_name << " is notified: " << item << "\n";
  }

private:
  std::string m_name{};
};

int main(int argc, char *argv[]) {
  Hal_Pub<std::string> pub{};
  Hal_Msg_Receiver rec1{"receiver 1"};
  Hal_Msg_Receiver rec2{"receiver 2"};
  Hal_Msg_Receiver rec3{"receiver 3"};

  pub.registerSubscriber(&rec1);
  pub.registerSubscriber(&rec2);
  pub.publish("hello pub sub");
  pub.publish("hello world");
  pub.publish("hello world 3");
  std::string str1{"string 1"};
  pub.publish(str1);

  pub.waitForEmpty();
  rec1.waitForEmpty();
  rec2.waitForEmpty();

  pub.registerSubscriber(&rec3);
  rec3.waitForEmpty();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  return 0;
}
