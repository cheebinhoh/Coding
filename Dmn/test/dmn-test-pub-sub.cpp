/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that the Dmn_Pub and Dmn_Pub::Sub model.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "dmn-pub-sub.hpp"

class Dmn_Msg_Receiver : public dmn::Dmn_Pub<std::string>::Dmn_Sub {
public:
  Dmn_Msg_Receiver(std::string_view name) : m_name{name} {}

  ~Dmn_Msg_Receiver() {}

  void notify(const std::string &item) override {
    std::cout << m_name << " is notified: " << item << "\n";

    m_notifiedList.push_back(item);
  }

  std::vector<std::string> m_notifiedList{};
  std::string m_name{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_Pub<std::string> pub{"radio", 2};
  Dmn_Msg_Receiver rec1{"receiver 1"};

  pub.registerSubscriber(&rec1);
  pub.publish("hello pub sub");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  EXPECT_TRUE(rec1.m_notifiedList.size() == 1);
  EXPECT_TRUE(rec1.m_notifiedList[0] == "hello pub sub");
  /*
    pub.publish("hello world");
    pub.publish("hello world 3");
    std::string str1{"string 1"};
    pub.publish(str1);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    EXPECT_TRUE(rec1.m_notifiedList.size() == 4);
    EXPECT_TRUE(rec1.m_notifiedList[1] == "hello world");
    EXPECT_TRUE(rec1.m_notifiedList[2] == "hello world 3");
    EXPECT_TRUE(rec1.m_notifiedList[3] == "string 1");
  */
  std::this_thread::sleep_for(std::chrono::seconds(5));
  pub.unregisterSubscriber(&rec1);

  /*
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
  */

  return RUN_ALL_TESTS();
}
