// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#include <gtest/gtest.h>
#include "./Hashi.h"

// _____________________________________________________________________________
TEST(Hashi, constructor) {
  Hashi gameTest0;
  ASSERT_EQ(0, gameTest0._max_x);
  ASSERT_EQ(0, gameTest0._max_y);
  ASSERT_EQ(-1, gameTest0._lastClicked_x);
  ASSERT_EQ(-1, gameTest0._lastClicked_y);
}

// _____________________________________________________________________________
TEST(Hashi, processUserInput) {
  Hashi gameTest1;
  ASSERT_EQ(-1, gameTest1.processUserInput(27));
  ASSERT_EQ(1, gameTest1.processUserInput('r'));
  ASSERT_EQ(2, gameTest1.processUserInput('s'));
  ASSERT_EQ(3, gameTest1.processUserInput('u'));
}

// _____________________________________________________________________________
TEST(Hashi, addBridge) {
  Hashi gameTest2;
  gameTest2._numbers = {{0, 0, 0, 3, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0},
                        {3, 0, 0, 5, 0, 0, 4},
                        {0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 2, 0, 0, 0}};
  gameTest2.addBridge(0, 2, 3, 2, false, false);
  gameTest2.addBridge(3, 0, 3, 2, false, true);
  gameTest2.addBridge(3, 2, 3, 5, false, false);
  gameTest2.addBridge(3, 2, 6, 2, false, true);

  ASSERT_EQ(10, gameTest2._numbers[2][1]);
  ASSERT_EQ(11, gameTest2._numbers[2][5]);
  ASSERT_EQ(11, gameTest2._numbers[2][4]);
  ASSERT_EQ(12, gameTest2._numbers[3][3]);
  ASSERT_EQ(13, gameTest2._numbers[1][3]);

  gameTest2.addBridge(0, 2, 3, 2, true, false);
  ASSERT_EQ(0, gameTest2._numbers[2][1]);
}

// _____________________________________________________________________________
TEST(Hashi, isBridgeValid) {
  Hashi gameTest3;
  gameTest3._max_x = 7;
  gameTest3._max_y = 5;
  gameTest3._numbers = {{2, 0, 0, 3, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0},
                        {3, 0, 0, 5, 0, 0, 4},
                        {0, 0, 1, 0, 1, 0, 0},
                        {0, 0, 0, 2, 0, 0, 0}};
  ASSERT_EQ(0, gameTest3.isBridgeValid(0, 0, 3, 0));
  ASSERT_EQ(1, gameTest3.isBridgeValid(0, 0, 4, 0));
  ASSERT_EQ(0, gameTest3.isBridgeValid(3, 0, 3, 2));
  ASSERT_EQ(0, gameTest3.isBridgeValid(3, 2, 3, 0));
  ASSERT_EQ(1, gameTest3.isBridgeValid(0, 0, 0, 0));

  gameTest3.addBridge(2, 3, 4, 3, false, false);
  ASSERT_EQ(10, gameTest3.isBridgeValid(2, 3, 4, 3));
  gameTest3.addBridge(2, 3, 4, 3, false, true);
  ASSERT_EQ(11, gameTest3.isBridgeValid(2, 3, 4, 3));
  gameTest3.addBridge(3, 2, 3, 4, false, false);
  ASSERT_EQ(12, gameTest3.isBridgeValid(3, 2, 3, 4));
  gameTest3.addBridge(3, 2, 3, 4, false, true);
  ASSERT_EQ(13, gameTest3.isBridgeValid(3, 2, 3, 4));
}

// _____________________________________________________________________________
TEST(Hashi, countBridges) {
  Hashi gameTest4;
  gameTest4._max_x = 7;
  gameTest4._max_y = 5;
  gameTest4._numbers = {{0, 0, 0, 3, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0},
                        {3, 0, 0, 5, 0, 0, 4},
                        {0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 2, 0, 0, 0}};
  gameTest4.addBridge(0, 2, 3, 2, false, true);
  ASSERT_EQ(2, gameTest4.countBridges(3, 2));
  ASSERT_EQ(0, gameTest4.countBridges(3, 0));
  gameTest4.addBridge(3, 2, 6, 2, false, false);
  ASSERT_EQ(1, gameTest4.countBridges(6, 2));
  ASSERT_EQ(3, gameTest4.countBridges(3, 2));
  ASSERT_EQ(2, gameTest4.countBridges(0, 2));
  ASSERT_EQ(0, gameTest4.countBridges(0, 0));
  ASSERT_EQ(0, gameTest4.countBridges(17, 17));
}

// _____________________________________________________________________________
TEST(Hashi, reset) {
  Hashi gameTest5;
  gameTest5._max_x = 6;
  gameTest5._max_y = 3;
  gameTest5._numbers = {{0, 0, 0, 3, 0, 0},
                        {0, 0, 0, 0, 0, 0},
                        {3, 0, 0, 5, 0, 0}};
  gameTest5._undos  = {{3, 0, 3, 2},
                       {0, 2, 3, 2}};
  gameTest5.addBridge(3, 0, 3, 2, false, false);
  gameTest5.addBridge(0, 2, 3, 2, false, false);
  ASSERT_EQ(12, gameTest5._numbers[1][3]);
  ASSERT_EQ(10, gameTest5._numbers[2][2]);
  gameTest5.reset();
  ASSERT_EQ(0, gameTest5._numbers[1][3]);
  ASSERT_EQ(0, gameTest5._numbers[2][2]);
  ASSERT_EQ(0, gameTest5._undos[0][0]);
  ASSERT_EQ(0, gameTest5._undos[1][2]);
}

// _____________________________________________________________________________
TEST(Hashi, undo) {
  Hashi gameTest6;
  gameTest6._max_x = 6;
  gameTest6._max_y = 3;
  gameTest6._numbers = {{0, 0, 0, 3, 0, 0},
                        {0, 0, 0, 0, 0, 0},
                        {3, 0, 0, 5, 0, 0}};
  gameTest6._undos  = {{3, 0, 3, 2},
                       {0, 2, 3, 2}};
  gameTest6.addBridge(3, 0, 3, 2, false, false);
  gameTest6.addBridge(0, 2, 3, 2, false, false);
  gameTest6.undo();
  ASSERT_EQ(0, gameTest6._undos[0][0]);
  ASSERT_EQ(3, gameTest6._undos[1][0]);
  ASSERT_EQ(2, gameTest6._undos[1][3]);
}

// _____________________________________________________________________________
TEST(Hashi, isSolved) {
  Hashi gameTest7;
  gameTest7._max_x = 6;
  gameTest7._max_y = 3;
  gameTest7._numbers = {{4, 0, 0, 3, 0, 0},
                        {0, 0, 0, 0, 0, 0},
                        {2, 0, 0, 1, 0, 0}};
  ASSERT_FALSE(gameTest7.isSolved());
  gameTest7.addBridge(0, 0, 3, 0, false, true);
  gameTest7.addBridge(0, 0, 0, 2, false, true);
  ASSERT_FALSE(gameTest7.isSolved());
  gameTest7.addBridge(3, 0, 3, 2, false, false);
  ASSERT_TRUE(gameTest7.isSolved());
}
