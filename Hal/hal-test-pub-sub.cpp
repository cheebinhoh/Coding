#include "hal-pub-sub.hpp"

#include <iostream>
#include <string>
#include <thread>

class Hal_Msg_Receiver : public Hal_Pub<std::string>::Hal_Sub {
public:
  ~Hal_Msg_Receiver() {}

  void notify(std::string item) override {
    std::cout << "notify: " << item << "\n";
  }
};

int main(int argc, char *argv[]) {
  Hal_Pub<std::string> pub{};
  Hal_Msg_Receiver rec{};

  pub.registerSubscriber(&rec);
  pub.publish("hello");
  pub.waitForEmpty();
  rec.waitForEmpty();

  return 0;
}
