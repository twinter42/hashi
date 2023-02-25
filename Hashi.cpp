// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#include <ncurses.h>
#include <vector>
#include "./Hashi.h"

// ____________________________________________________________________________
Hashi::Hashi() {
  _max_x = 0;
  _max_y = 0;
  _lastClicked_x = -1;
  _lastClicked_y = -1;
}

// ____________________________________________________________________________
Hashi::~Hashi() {
  // Clean up window.
  endwin();
}

// ____________________________________________________________________________
void Hashi::initializeGame() const {
  // prepare the terminal for drawing
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  nodelay(stdscr, true);
  keypad(stdscr, true);
  // Catch mouse events
  mousemask(ALL_MOUSE_EVENTS, NULL);
  start_color();

  // define colors
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  init_pair(3, COLOR_BLACK, COLOR_RED);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_BLACK, COLOR_BLACK);

  // Draw menu
  mvprintw(0, 3, "Hashiwokakero %d X %d", _max_x, _max_y);
  mvprintw((_max_y) * 3 + 4, 3, "**************************"
  "*******************************************************");
  attron(COLOR_PAIR(3));
  mvprintw((_max_y) * 3 + 6, 3, " press ESC to exit ");
  mvprintw((_max_y) * 3 + 6, 23, " press r to reset ");
  mvprintw((_max_y) * 3 + 6, 42, " press u to undo ");
  mvprintw((_max_y) * 3 + 6, 60, " press s for solve mode ");
  if (_sol.size() == 0) {
    mvprintw((_max_y) * 3 + 8, 3, " NO VALID SOLUTION FILE ");
  }
  attron(COLOR_PAIR(3));

  // draw the number field
  attron(COLOR_PAIR(1));
  for (int row = 0; row < _max_y; row++) {
    for (int col = 0; col < _max_x; col++) {
      if (_numbers[row][col] != 0) {
        mvprintw(3*row+2, 5*col+3, "     ");
        mvprintw(3*row+3, 5*col+3, "  %d  ", _numbers[row][col]);
        mvprintw(3*row+4, 5*col+3, "     ");
      }
    }
  }
  attroff(COLOR_PAIR(1));
}

// ____________________________________________________________________________
void Hashi::play() {
  while (true) {
    int key = getch();
    int input = processUserInput(key);
    // proceed according to user input
    if (input == -1) {
      break;
    }
    switch (input) {
      case 1:
        reset();
        updateMarkers();
        break;
      case 2:
        solve();
        break;
    }
    if (input ==  3 && _undos.size() != 0) {
      undo();
    }
    usleep(10);
  }
}

// ____________________________________________________________________________
int Hashi::processUserInput(const int key) {
  // check for mouse clicks
  MEVENT event;
  switch (key) {
    case 27:
      // exit game
      return -1;
    case 'r':
      // reset all bridges
      return 1;
    case 's':
      // solve mode
      return 2;
    case 'u':
      // undo
      return 3;
    case KEY_MOUSE:
      if (getmouse(&event) == OK) {
        if (event.bstate & BUTTON1_CLICKED) {
          if ((event.x-3)/5 < _max_x && (event.y-2)/3 < _max_y
               && event.x >= 0 && event.y >= 0) {
            if (_lastClicked_x < 0) {
              // mark the isle if it is the first click
              _lastClicked_x = (event.x-3)/5;
              _lastClicked_y = (event.y-2)/3;
              markIsle(_lastClicked_x, _lastClicked_y, 4);
            } else {
              // only add valid bridges to the undo list
              if (isBridgeValid(_lastClicked_x, _lastClicked_y,
               (event.x-3)/5, (event.y-2)/3) != 1) {
               // add bridge to the list of undo operaions
                _undos.push_back({_lastClicked_x, _lastClicked_y,
                (event.x-3)/5, (event.y-2)/3});
                // delete first row of _undos
                _undos.erase(_undos.begin());
              }

              // draw bridge
              drawBridge(_lastClicked_x, _lastClicked_y,
               (event.x-3)/5, (event.y-2)/3);
              // prepare for next bridge
              _lastClicked_x = -1;
              // update all markers
              updateMarkers();
            }
          }
        }
      }
  }
  return 0;
}

// ____________________________________________________________________________
void Hashi::drawBridge(int x1, int y1, int x2, int y2) {
  // do not proceed if the coordinates do not present a valid bridge
  if (isBridgeValid(x1, y1, x2, y2) == 1) {return;}
  // turn the coordinates around if necessary
  if (x2 < x1 && y1 == y2) {
    int temp;
    temp = x1;
    x1 = x2;
    x2 = temp;
  }
  if (y2 < y1 && x1 == x2) {
    int temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  // check for the bridge type
  int bridgeType = isBridgeValid(x1, y1, x2, y2);
  bool doubleBridge = (bridgeType == 10 || bridgeType == 12);
  bool del = (bridgeType == 11 || bridgeType == 13);

  if (del) {
    attron(COLOR_PAIR(5));
  } else {
    attron(A_NORMAL);
  }

  // horizontal bridge
  if (x1 != x2) {
    for (int i = 5*x1+8; i < 5*x2+3; i++) {
      if (doubleBridge) {
        mvprintw(3*y1+2, i, "-");
        mvprintw(3*y1+3, i, " ");
        mvprintw(3*y1+4, i, "-");
      } else {
        mvprintw(3*y1+2, i, " ");
        mvprintw(3*y1+3, i, "-");
        mvprintw(3*y1+4, i, " ");
      }
    }
  }

  // vertical bridge
  if (y1 != y2) {
    for (int i = 3*y1+5; i < 3*y2+2; i++) {
      if (doubleBridge) {
        mvprintw(i, 5*x1+3, "|");
        mvprintw(i, 5*x1+5, "   ");
        mvprintw(i, 5*x1+7, "|");
      } else {
        mvprintw(i, 5*x1+3, "  ");
        mvprintw(i, 5*x1+5, "|");
        mvprintw(i, 5*x1+6, "  ");
      }
    }
  }

  if (del) {
    attroff(COLOR_PAIR(5));
  } else {
    attroff(A_NORMAL);
  }

  // add the bridge to the _numbers matrix and update
  addBridge(x1, y1, x2, y2, del, doubleBridge);
  updateMarkers();
}

// ____________________________________________________________________________
void Hashi::addBridge(int x1, int y1, int x2, int y2, const bool del,
 const bool doubleBridge) {
  int bridge;
  // determine the bridge type and assign the corresponding encryption value
  if (del) {
    bridge = 0;
  } else if (doubleBridge && y1 == y2) {
    bridge = 11;
  } else if (doubleBridge && x1 == x2) {
    bridge = 13;
  } else if (doubleBridge == false && y1 == y2) {
    bridge = 10;
  } else {
    bridge = 12;
  }

  // update the _numbers matrix
  if (x1 == x2) {
    for (int i = y1 + 1; i < y2; i++) {
      _numbers[i][x1] = bridge;
    }
  } else {
    for (int i = x1 + 1; i < x2; i++) {
      _numbers[y1][i] = bridge;
    }
  }
}

// ____________________________________________________________________________
int Hashi::isBridgeValid(int x1, int y1, int x2, int y2) const {
  // turn the coordinates around if necessary
  if (x2 < x1 && y1 == y2) {
    int temp;
    temp = x1;
    x1 = x2;
    x2 = temp;
  }
  if (y2 < y1 && x1 == x2) {
    int temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  // check for coordinates that represent invalid bridges
  if ((x1 == x2 && y1 == y2) || (x1 != x2 && y1 != y2) || x1 > _max_x
  || x2 > _max_x || y1 > _max_y || y2 > _max_y || x1 < 0 || x2 < 0 || y1 < 0
  || y2 < 0 || _numbers[y1][x1] == 0 || _numbers[y2][x2] == 0
  || _numbers[y1][x1] > 8 || _numbers[y2][x2] > 8) {
    return 1;
  }

  // determine and return the bridge type
  if (x1 == x2) {
    for (int i = y1 + 1; i < y2-1; i++) {
      if (_numbers[i][x1] != _numbers[i+1][x1]) {
        return 1;
      }
    }
    if (_numbers[y1+1][x1] > 11 || _numbers[y1+1][x1] == 0) {
      return _numbers[y1+1][x1];
    }
  }
  if (y1 == y2) {
    for (int i = x1 + 1; i < x2-1; i++) {
      if (_numbers[y1][i] != _numbers[y1][i+1]) {
        return 1;
      }
    }
    if (_numbers[y1][x1+1] < 12 || _numbers[y1][x1+1] == 0) {
      return _numbers[y1][x1+1];
    }
  }
  return 1;
}

// ____________________________________________________________________________
int Hashi::countBridges(const int x, const int y) const {
  int bridgeCount = 0;

  // Check all 4 sides of the isle. The amount of bridges is determined
  // by using %2 +1 which returns the correct amount of bridges (single bridges
  // are represented by even and double bridges by odd numbers).
  if (x >= 0 && y >= 0 && x < _max_x && y < _max_y && _numbers[y][x] < 10
  && _numbers[y][x] > 0) {
    if (x > 0 && _numbers[y][x-1] > 8 && _numbers[y][x-1] < 12) {
      bridgeCount += _numbers[y][x-1] % 2 + 1;
    }
    if (x < _max_x-1 && _numbers[y][x+1] > 8 && _numbers[y][x+1] < 12) {
      bridgeCount += _numbers[y][x+1] % 2 + 1;
    }
    if (y > 0 && _numbers[y-1][x] > 8 && _numbers[y-1][x] > 11) {
      bridgeCount += _numbers[y-1][x] % 2 +1;
    }
    if (y < _max_y-1 && _numbers[y+1][x] > 8 && _numbers[y+1][x] > 11) {
      bridgeCount += _numbers[y+1][x] % 2 + 1;
    }
  }
  return bridgeCount;
}

// ____________________________________________________________________________
void Hashi::updateMarkers() {
  for (int col = 0; col < _max_y; col++) {
    for (int row = 0; row < _max_x; row++) {
      if (_numbers[col][row] == countBridges(row, col)) {
        markIsle(row, col, 2);
      } else if (_numbers[col][row] < countBridges(row, col)) {
        markIsle(row, col, 3);
      } else {
        markIsle(row, col, 1);
      }
    }
  }
  isSolved();
}

// ____________________________________________________________________________
void Hashi::markIsle(const int x, const int y, const int color) const {
  if (x >= 0 && y >= 0 && x < _max_x && y < _max_y && _numbers[y][x] < 10
  && _numbers[y][x] > 0) {
    attron(COLOR_PAIR(color));
    mvprintw(3*y+2, 5*x+3, "     ");
    mvprintw(3*y+3, 5*x+3, "  %d  ", _numbers[y][x]);
    mvprintw(3*y+4, 5*x+3, "     ");
    attroff(COLOR_PAIR(color));
  }
}

// ____________________________________________________________________________
void Hashi::reset() {
  // delete all bridges
  for (int row = 0; row < _max_y; row++) {
    for (int col = 0; col < _max_x; col++) {
      if (_numbers[row][col] > 9) {
        mvprintw(3*row+2, 5*col+3, "     ");
        mvprintw(3*row+3, 5*col+3, "     ", _numbers[row][col]);
        mvprintw(3*row+4, 5*col+3, "     ");
        _numbers[row][col] = 0;
      }
    }
  }
  // reset undo list
  for (unsigned int col = 0; col < _undos.size(); col++) {
    for (int row = 0; row < 4; row++) {
      _undos[col][row] = 0;
    }
  }
}

// ____________________________________________________________________________
void Hashi::undo() {
  int last_row = _undos.size() -1;
  // drawing a bridge two times equals an undo operation
  for (int i = 0; i < 2; i++) {
    drawBridge(_undos[last_row][0], _undos[last_row][1],
    _undos[last_row][2], _undos[last_row][3]);
  }
  // insert new empty row at the beginning and delete the last one
  _undos.insert(_undos.begin(), {0, 0, 0, 0});
  _undos.erase(_undos.end()-1);
}

// ____________________________________________________________________________
bool Hashi::isSolved() {
  for (int col = 0; col < _max_y; col++) {
    for (int row = 0; row < _max_x; row++) {
      if (_numbers[col][row] != countBridges(row, col)
      && _numbers[col][row] < 10) {
        solvedMessage(true);
        return false;
      }
    }
  }
  solvedMessage(false);
  return true;
}

// ____________________________________________________________________________
void Hashi::solve() {
  if (_sol.size() > 0) {
    reset();
    for (unsigned int i = 0; i < _sol.size(); i++) {
      drawBridge(_sol[i][0], _sol[i][1], _sol[i][2], _sol[i][3]);
    }
    if (isSolved() == false) {
      mvprintw((_max_y) * 3 + 8, 2, " The solution file does not solve the "
      "puzzle!");
    }
  }
}

// ____________________________________________________________________________
void Hashi::solvedMessage(const bool del) {
  if (del) {
    mvprintw((_max_y) * 3 + 5, 23,   "                                ");
  } else {
    attron(COLOR_PAIR(2));
    mvprintw((_max_y) * 3 + 5, 23,   " *SOLVED*  Press ESC to exit ...");
    attroff(COLOR_PAIR(2));
  }
}
