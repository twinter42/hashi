// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#ifndef FILEINTERPRETER_H_
#define FILEINTERPRETER_H_

#include <gtest/gtest.h>
#include "Hashi.h"

class Hashi;

class FileInterpreter {
 public:
  // constructor
  FileInterpreter();
  FRIEND_TEST(FileInterpreter, constructor);

  // Parse the command line options.
  void parseCommandLineArguments(int argc, char** argv);
  FRIEND_TEST(FileInterpreter, parseCommandLineArgumentsNoArguments);
  FRIEND_TEST(FileInterpreter, parseCommandLineArgumentsArguments);
  FRIEND_TEST(FileInterpreter, parseCommandLineArgumentsArgumentsSetUndos);

  // Process the command line arguments by calling the
  // matching private set-function below. If the input file is
  // invalid, the program stops and shows a error message.
  // If the solution file is invalid, the program continues
  // without it.
  void processFiles(Hashi* hashi) const;
  FRIEND_TEST(FileInterpreter, processFiles);

 private:
  // Name of the input file.
  const char* _inputFile;
  // Name of the solution file.
  const char* _solutionFile;

  // The allowed amount of undo operations
  int _undoOperations;

  // Print errors and usage information when the programm is called with
  // the wrong parameters
  void printUsageAndExit() const;
  FRIEND_TEST(FileInterpreter, printUsageAndExit);

  // Checks if the file ending is correct.
  // Arguments:
  //   const char* file - the full name of the file
  //   const char* ending - the requiered ending (including '.')
  // Returns:
  //   bool - true if the ending of the file matches the given ending
  bool checkFileEnding(const char* file, const char* ending) const;
  FRIEND_TEST(FileInterpreter, checkFileEnding);

  // Sets the initial array values for the game according to
  // a .xy input file.
  void setFieldxy(Hashi* hashi) const;
  FRIEND_TEST(FileInterpreter, setFieldxy);
  FRIEND_TEST(FileInterpreter, readInvalidFilexy);

  // Sets the initial array values for the game according to
  // a .plain input file.
  void setFieldPlain(Hashi* hashi) const;
  FRIEND_TEST(FileInterpreter, setFieldPlain);
  FRIEND_TEST(FileInterpreter, readInvalidFilePlain);

  // Sets the initial solution array values for the game according
  // to a .xy.solution input file.
  void setSolution(Hashi* hashi) const;
  FRIEND_TEST(FileInterpreter, setSolution);
};

#endif  // FILEINTERPRETER_H_
