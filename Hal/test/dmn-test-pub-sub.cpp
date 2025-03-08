/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-pub-sub.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <thread>

class Dmn_Msg_Receiver : public Dmn::Dmn_Pub<std::string>::Dmn_Sub {
public:
  Dmn_Msg_Receiver(std::string_view name) : m_name{name} {}

  ~Dmn_Msg_Receiver() {}

  void notify(std::string item) override {
    std::cout << m_name << " is notified: " << item << "\n";

    m_notifiedList.push_back(item);
  }

  std::vector<std::string> m_notifiedList{};
  std::string m_name{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);


  Dmn::Dmn_Pub<std::string> pub{"radio", 2, [](const Dmn::Dmn_Pub<std::string>::Dmn_Sub *const sub,
                                          const std::string &data) -> bool 
                                          {
 					    auto *msgReceiver = dynamic_cast<const Dmn_Msg_Receiver * const>(sub);

                                            return nullptr != msgReceiver
                                                   && ("receiver 2" != msgReceiver->m_name ||
                                                       "filter from 2" != data);
                                          }};
  Dmn_Msg_Receiver rec1{"receiver 1"};
  Dmn_Msg_Receiver rec2{"receiver 2"};
  Dmn_Msg_Receiver rec3{"receiver 3"};

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
  EXPECT_TRUE(rec1.m_notifiedList.size() == 4);
  EXPECT_TRUE(rec1.m_notifiedList[0] == "hello pub sub");
  EXPECT_TRUE(rec1.m_notifiedList[1] == "hello world");
  EXPECT_TRUE(rec1.m_notifiedList[2] == "hello world 3");
  EXPECT_TRUE(rec1.m_notifiedList[3] == "string 1");

  EXPECT_TRUE(rec2.m_notifiedList.size() == 4);
  EXPECT_TRUE(rec2.m_notifiedList[0] == "hello pub sub");
  EXPECT_TRUE(rec2.m_notifiedList[1] == "hello world");
  EXPECT_TRUE(rec2.m_notifiedList[2] == "hello world 3");
  EXPECT_TRUE(rec2.m_notifiedList[3] == "string 1");

  pub.registerSubscriber(&rec3);
  pub.waitForEmpty();
  rec3.waitForEmpty();
  EXPECT_TRUE(rec3.m_notifiedList.size() == 2);
  EXPECT_TRUE(rec3.m_notifiedList[0] == "hello world 3");
  EXPECT_TRUE(rec3.m_notifiedList[1] == "string 1");

  pub.unregisterSubscriber(&rec1);
  pub.publish("hello world last");
  pub.waitForEmpty();
  rec1.waitForEmpty();
  rec2.waitForEmpty();
  rec3.waitForEmpty();
  EXPECT_TRUE(rec1.m_notifiedList.size() == 4);
  EXPECT_TRUE(rec1.m_notifiedList[3] == "string 1");

  EXPECT_TRUE(rec2.m_notifiedList.size() == 5);
  EXPECT_TRUE(rec2.m_notifiedList[4] == "hello world last");

  EXPECT_TRUE(rec3.m_notifiedList.size() == 3);
  EXPECT_TRUE(rec3.m_notifiedList[2] == "hello world last");

  {
    Dmn_Msg_Receiver rec4{"receiver 4"};
    pub.registerSubscriber(&rec4);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pub.publish("hello world last again");
    pub.waitForEmpty();
    rec4.waitForEmpty();

    EXPECT_TRUE(rec4.m_notifiedList.size() == 3);
    EXPECT_TRUE(rec4.m_notifiedList[2] == "hello world last again");
  }

  pub.publish("hello world last again again");
  pub.waitForEmpty();
  rec2.waitForEmpty();
  rec3.waitForEmpty();
  EXPECT_TRUE(rec2.m_notifiedList.size() == 7);
  EXPECT_TRUE(rec3.m_notifiedList.size() == 5);

  pub.publish("filter from 2");
  pub.waitForEmpty();
  rec2.waitForEmpty();
  rec3.waitForEmpty();
  EXPECT_TRUE(rec2.m_notifiedList.size() == 7);
  EXPECT_TRUE(rec3.m_notifiedList.size() == 6);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  return RUN_ALL_TESTS();
}
