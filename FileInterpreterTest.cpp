// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#include <gtest/gtest.h>
#include "./FileInterpreter.h"

// _____________________________________________________________________________
TEST(FileInterpreter, constructor) {
  FileInterpreter test1;
  ASSERT_STREQ("", test1._inputFile);
  ASSERT_STREQ("", test1._solutionFile);
  ASSERT_EQ(5, test1._undoOperations);
}

// _____________________________________________________________________________
TEST(FileInterpreter, parseCommandLineArgumentsNoArguments) {
  FileInterpreter test2;
  int argc = 1;
  char* argv[1] = {
    const_cast<char*>("")
  };
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(test2.parseCommandLineArguments(argc, argv), "Usage: .*");
}

// _____________________________________________________________________________
TEST(FileInterpreter, parseCommandLineArgumentsArguments) {
  FileInterpreter test3;
  int argc = 4;
  char* argv[4] = {
    const_cast<char*>(""),
    const_cast<char*>("--solution"),
    const_cast<char*>("mySolution"),
    const_cast<char*>("myInputFile")
  };
  test3.parseCommandLineArguments(argc, argv);
  ASSERT_STREQ("myInputFile", test3._inputFile);
  ASSERT_STREQ("mySolution", test3._solutionFile);
  ASSERT_EQ(5, test3._undoOperations);
}

// _____________________________________________________________________________
TEST(FileInterpreter, parseCommandLineArgumentsArgumentsSetUndos) {
  FileInterpreter test4;
  int argc = 6;
  char* argv[6] = {
    const_cast<char*>(""),
    const_cast<char*>("--solution"),
    const_cast<char*>("mySolution"),
    const_cast<char*>("--undos"),
    const_cast<char*>("17"),
    const_cast<char*>("myInputFile")
  };
  test4.parseCommandLineArguments(argc, argv);
  ASSERT_STREQ("myInputFile", test4._inputFile);
  ASSERT_STREQ("mySolution", test4._solutionFile);
  ASSERT_EQ(17, test4._undoOperations);
}

// _____________________________________________________________________________
TEST(FileInterpreter, printUsageAndExit) {
  FileInterpreter test5;
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(test5.printUsageAndExit(), "Usage: .*");
}

// _____________________________________________________________________________
TEST(FileInterpreter, checkFileEnding) {
  FileInterpreter test6;
  ASSERT_TRUE(test6.checkFileEnding("myFile.something", ".something"));
  ASSERT_FALSE(test6.checkFileEnding("myFile.something", ".someotherthing"));
}

// _____________________________________________________________________________
TEST(FileInterpreter, processFiles) {
  FileInterpreter test7;
  Hashi gametest7;
  int argc = 2;
  char* argv[2] = {
    const_cast<char*>(""),
    const_cast<char*>("myInputFile")
  };
  test7.parseCommandLineArguments(argc, argv);
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(test7.processFiles(&gametest7), "<inputfile> has to have .*");
}

// _____________________________________________________________________________
TEST(FileInterpreter, setFieldxy) {
  FileInterpreter test8;
  Hashi gametest8;
  FILE* input = fopen("thisIsATest.xy", "w");
  fprintf(input, "# 5:5 (xy)\n"
                 "0,0,1\n"
                 "0,4,3\n"
                 "4,4,2\n");
  fclose(input);
  test8._inputFile = "thisIsATest.xy";
  test8.setFieldxy(&gametest8);
  ASSERT_EQ(5, gametest8._max_x);
  ASSERT_EQ(5, gametest8._max_y);
  ASSERT_EQ(3, gametest8._numbers[4][0]);
  ASSERT_EQ(2, gametest8._numbers[4][4]);
  unlink("thisIsATest.xy");
}

// _____________________________________________________________________________
TEST(FileInterpreter, readInvalidFilexy) {
  FileInterpreter test9;
  Hashi gametest9;
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(test9.setFieldxy(&gametest9), "Error opening .*");
}

// _____________________________________________________________________________
TEST(FileInterpreter, setFieldPlain) {
  FileInterpreter test10;
  Hashi gametest10;
  FILE* input = fopen("thisIsATest.plain", "w");
  fprintf(input, "# 5:5 (plain)\n"
                 "1    \n"
                 "     \n"
                 "     \n"
                 "     \n"
                 "3   2");
  fclose(input);
  test10._inputFile = "thisIsATest.plain";
  test10.setFieldPlain(&gametest10);
  ASSERT_EQ(5, gametest10._max_x);
  ASSERT_EQ(5, gametest10._max_y);
  ASSERT_EQ(3, gametest10._numbers[4][0]);
  ASSERT_EQ(2, gametest10._numbers[4][4]);
  unlink("thisIsATest.plain");
}

// _____________________________________________________________________________
TEST(FileInterpreter, readInvalidFilePlain) {
  FileInterpreter test11;
  Hashi gametest11;
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(test11.setFieldxy(&gametest11), "Error opening .*");
}

// _____________________________________________________________________________
TEST(FileInterpreter, setSolution) {
  FileInterpreter test12;
  Hashi gametest12;
  FILE* solution = fopen("thisIsATest.xy.solution", "w");
  fprintf(solution, " 1,  2,  3,  4\n"
                    " 5,  6,  7,  8\n"
                    " 9, 10, 11, 12\n"
                    "13, 14, 15, 16");
  fclose(solution);
  test12._solutionFile = "thisIsATest.xy.solution";
  test12.setSolution(&gametest12);
  ASSERT_EQ(4, gametest12._sol.size());
  ASSERT_EQ(8, gametest12._sol[1][3]);
  ASSERT_EQ(11, gametest12._sol[2][2]);
  ASSERT_STREQ("thisIsATest.xy.solution", gametest12._solutionFile);
  unlink("thisIsATest.xy.solution");
}
