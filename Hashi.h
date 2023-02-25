// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#ifndef HASHI_H_
#define HASHI_H_

#include <gtest/gtest.h>
#include <vector>
#include "./FileInterpreter.h"

class Hashi {
  // Allow the FileInterpreter class to initialize the private array
  // values for the number field according to the given input file.
  friend class FileInterpreter;

 public:
  // Constructor - sets the important member variables for a new object
  Hashi();
  FRIEND_TEST(Hashi, constructor);
  // Destructor
  ~Hashi();

  // Prepare the terminal for drawing with ncurses, draw the number
  // field and the menu.
  void initializeGame() const;

  // plays the game in a while loop
  void play();

 private:
  // name of the solution file
  const char* _solutionFile;

  // vector of vectors that store the field numbers
  std::vector< std::vector<int> > _numbers;
  // proportions of the _numbers matrix
  int _max_x;
  int _max_y;

  // coordinates of the selected isle
  int _lastClicked_x;
  int _lastClicked_y;

  // matrix that stores the solution coordinates
  std::vector< std::vector<int> > _sol;

  // matrix that stores the coordinates of the last drawn valid bridges
  std::vector< std::vector<int> > _undos;

  // Proccesses the user input (keyboard and mouse).
  // Arguments:
  //   const int key - the last pressed key
  // Returns: int - a specific feedback depending on the pressed key:
  //   ESC -1
  //   'r'  1
  //   's'  2
  //   'u'  3
  //   (Returns 0 in any other case)
  int processUserInput(const int key);
  FRIEND_TEST(Hashi, processUserInput);

  // Draw a bridge if possible (calls the isBridgeValid() function).
  // Automatically draws the correct bridge type by using the return value
  // of isBridgeValid()
  // Arguments:
  //   int x1 - the x coordinate of the first isle
  //   int y1 - the y coordinate of the first isle
  //   int x2 - the x coordinate of the second isle
  //   int y2 - the y coordinate of the second isle
  void drawBridge(int x1, int y1, int x2, int y2);

  // Adds a bridge to the _numbers matrix according
  // Arguments:
  //   const bool del - if del is true, the bridge will be deleted in the
  //                    _numbers matrix
  //   const bool doubleBridge - if true, a double-bridge will be added
  //   (for the other arguments, see drawBridge())
  void addBridge(int x1, int y1, int x2, int y2, const bool del,
   const bool doubleBridge);
  FRIEND_TEST(Hashi, addBridge);

  // Method that checks if a bridge with the given coordinates is valid.
  // Arguments:
  //   the coordinates for the two isles (same as drawBridge())
  // Returns: int - the current bridge type according to the following code:
  //    0 - no bridge (but also no other obstacles)
  //    1 - no bridge (but there are either obstacles such as crossing
  //        bridges / other isles or the coordinates can't represent a bridge)
  //   10 - horizontal single bridge
  //   11 - horizontal double bridge
  //   12 - vertical single bridge
  //   13 - vertical double bridge
  int isBridgeValid(int x1, int y1, int x2, int y2) const;
  FRIEND_TEST(Hashi, isBridgeValid);

  // Count the bridges on a island at the position (x,y) on the number field.
  // Arguments:
  //   const int x - the x coordinate of the isle
  //   const int y - the y coordinate of the isle
  // Returns:
  //   int bridgeCount - the amount of bridges connected to the isle at the
  //                     given position
  int countBridges(const int x, const int y) const;
  FRIEND_TEST(Hashi, countBridges);

  // Redraws all markers on the number field by using the countBridge()
  // and the markIsle() method.
  void updateMarkers();

  // Method that marks a isle at given coordinates.
  // Arguments:
  //   const int x - the x coordinate of the isle
  //   const int y - the y coordinate of the isle
  //   const int color - the marking color
  //     available colors: white(1), green(2), red(3), yellow(4), black(5)
  void markIsle(const int x, const int y, const int color) const;

  // Delete all bridges and reset the undo list.
  void reset();
  FRIEND_TEST(Hashi, reset);

  // Undoes the last drawn (valid) bridge.
  void undo();
  FRIEND_TEST(Hashi, undo);

  // Checks if every isle has the correct amount of bridges. If solved,
  // solvedMessage() is called.
  // Returns:
  //   bool - the solve state of the number field (solved(1) or not solved(0))
  bool isSolved();
  FRIEND_TEST(Hashi, isSolved);

  // If a valid solution file is given, reset() will be called and
  // all valid bridges will be drawn. If the solution is valid but does not
  // solve the puzzle, a message will be printed below the menu.
  void solve();

  // Prints a message on green background.
  // Arguments:
  //   const bool del - overwrite (delete) the message (e.g. if the player
  //                    decides to build new bridges after the puzzle is
  //                    already solved)
  void solvedMessage(const bool del);

  // Allow tests to access private member variables
  FRIEND_TEST(FileInterpreter, processFiles);
  FRIEND_TEST(FileInterpreter, setFieldxy);
  FRIEND_TEST(FileInterpreter, readInvalidFilexy);
  FRIEND_TEST(FileInterpreter, setFieldPlain);
  FRIEND_TEST(FileInterpreter, readInvalidFilePlain);
  FRIEND_TEST(FileInterpreter, setSolution);
};

#endif  // HASHI_H_
