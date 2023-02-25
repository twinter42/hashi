// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#include "./FileInterpreter.h"
#include "./Hashi.h"

int main(int argc, char** argv) {
  Hashi game1;
  FileInterpreter fi;
  fi.parseCommandLineArguments(argc, argv);
  // Create new game object.
  fi.processFiles(&game1);
  // Initialize terminal and grid.
  game1.initializeGame();
  // Start the game.
  game1.play();
}
