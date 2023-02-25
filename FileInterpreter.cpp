// Copyright 2018 Tim Samuel Winter
// Author: Tim Samuel Winter <tim.s.winter@googlemail.com>

#include <getopt.h>
#include <fstream>
#include <string>
#include <vector>
#include "./FileInterpreter.h"
#include "./Hashi.h"

// ____________________________________________________________________________
FileInterpreter::FileInterpreter() {
  // set default values
  _inputFile = "";
  _solutionFile = "";
  _undoOperations = 5;
}

// ____________________________________________________________________________
void FileInterpreter::printUsageAndExit() const {
  std::cerr << "Usage: ./HashiMain [options] <inputfile>\n";
  std::cerr << "Available options:\n";
  std::cerr << "--solution <solutionfile> : "
  "A solution for the given input file.\n";
  std::cerr << " (default: null)\n";
  std::cerr << "--undos <int> : Amount of allowed undo-operations.\n";
  std::cerr << " (default: 5)\n";
  exit(1);
}

// ____________________________________________________________________________
void FileInterpreter::parseCommandLineArguments(int argc, char** argv) {
  struct option options[] = {
    {"solution", 1, NULL, 's'},
    {"undos", 1, NULL, 'u' }
  };
  optind = 1;

  // Default values
  _inputFile = "";
  _solutionFile = "";
  _undoOperations = 5;

  while (true) {
    char c = getopt_long(argc, argv, "s:u:", options, NULL);
    if (c == -1) {break; }
    switch (c) {
      case 's':
        _solutionFile = optarg;
        break;
      case 'u':
        _undoOperations = atoi(optarg);
        break;
      default:
        printUsageAndExit();
    }
  }
  // require input file
  if (optind + 1 != argc) {
    printUsageAndExit();
  }
  _inputFile = argv[optind];
}

// ____________________________________________________________________________
bool FileInterpreter::checkFileEnding(const char* file, const char* ending)
const {
  return (strlen(file) >= strlen(ending) && strcmp(file + strlen(file)
  - strlen(ending), ending) == 0);
}

// ____________________________________________________________________________
void FileInterpreter::processFiles(Hashi* hashi) const {
  if (checkFileEnding(_inputFile, ".xy")) {
    setFieldxy(hashi);
  } else if (checkFileEnding(_inputFile, ".plain")) {
    setFieldPlain(hashi);
  } else {
    std::cerr << "<inputfile> has to have a .xy or .plain format! \n";
    printUsageAndExit();
  }
  if (checkFileEnding(_solutionFile, ".xy.solution")) {
    setSolution(hashi);
  } else {
    // no valid solution file
    hashi->_solutionFile = "";
  }
  try {
    // set allowed amount of undo operations
    hashi->_undos.resize(_undoOperations,  std::vector<int>(4));
  }
  catch (std::exception& e) {
    hashi->_undos.resize(5,  std::vector<int>(4));
  }
}

// ____________________________________________________________________________
void FileInterpreter::setFieldxy(Hashi* hashi) const {
  std::ifstream file(_inputFile);
  if (!file.is_open()) {
    std::cerr << "Error opening input file: "<< _inputFile << std::endl;
    exit(1);
  }

  // determine the size of the _numbers matrix
  std::string line;
  while (getline(file, line)) {
    std::stringstream ss(line);
    if (line[0] != '#' && line.length() != 0) {
      try {
        getline(ss, line, ',');
        if (std::stoi(line) > hashi->_max_x) {
          hashi->_max_x = std::stoi(line);
        }
        getline(ss, line, ',');
        if (std::stoi(line) > hashi->_max_y) {
          hashi->_max_y = std::stoi(line);
        }
      }
      catch (std::exception& e) {
          std::cerr << "Error reading the input file. Does it have the correct "
          "format? \n";
          exit(1);
      }
    }
  }
  // Consider that the index of _numbers starts with 0.
  hashi->_max_x++;
  hashi->_max_y++;

  // resize matrix
  hashi->_numbers.resize(hashi->_max_y,
   std::vector<int>(hashi->_max_x));

  // go to the beginning of the file
  file.clear();
  file.seekg(0, std::ios::beg);

  // read file
  int coordList[3];
  while (getline(file, line)) {
    std::stringstream ss(line);
    if (line[0] != '#' && line.length() != 0) {
      for (int i = 0; i < 3; i++) {
        getline(ss, line, ',');
        try {
          coordList[i] = std::stoi(line);
        }
        catch (std::exception& e) {
          std::cerr << "Error reading the input file. Does it have the correct "
          "format? \n";
          exit(1);
        }
      }

      // write numb in the given y,x position in the _numbers matrix
      hashi->_numbers[coordList[1]][coordList[0]] = coordList[2];
    }
  }
}

// ____________________________________________________________________________
void FileInterpreter::setFieldPlain(Hashi* hashi) const {
  std::ifstream file(_inputFile);
  if (!file.is_open()) {
    std::cerr << "Error opening input file: "<< _inputFile << std::endl;
    exit(1);
  }

  // determine the size of the _numbers matrix
  std::string line;
  size_t temp = 0;
  while (getline(file, line)) {
    if (line[0] != '#' && line.length() != 0) {
      hashi->_max_y++;
      if (temp != line.length() && temp != 0) {
        std::cerr << "The lines of the input file do not have the same "
         "length! Consider deleting redundant spaces at the end of the "
         "lines." << std::endl;
        exit(1);
      }
      if (line.length() > (unsigned)hashi->_max_x) {
        hashi->_max_x = line.length();
      }
      temp = line.length();
    }
  }

  // resize matrices
  hashi->_numbers.resize(hashi->_max_y,
   std::vector<int>(hashi->_max_x));

  // go to the beginning of the file
  file.clear();
  file.seekg(0, std::ios::beg);

  // read file the file char by char
  int linenr = 0;
  while (getline(file, line)) {
    if (line[0] != '#' && line.length() != 0) {
      for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == 32) {
          hashi->_numbers[linenr][i] = 0;
        } else if (line[i] < 32 || line[i] > 56) {
          std::cerr << "Error reading the input file. Does it have the correct "
          "format? \n";
        } else {
          hashi->_numbers[linenr][i] = line[i] - '0';
        }
      }
      linenr++;
    }
  }
}

// ____________________________________________________________________________
void FileInterpreter::setSolution(Hashi* hashi) const {
  std::ifstream file(_solutionFile);
  if (!file.is_open()) {
    // program should continue even if the solution file is not valid.
    hashi->_solutionFile = "";
  }

  // read file
  std::string line;
  int coordList[4];
  while (getline(file, line)) {
    std::stringstream ss(line);
    if (line[0] != '#' && line.length() != 0) {
      for (int i = 0; i < 4; i++) {
        getline(ss, line, ',');
        try {
          coordList[i] = std::stoi(line);
        }
        catch (std::exception& e) {
          // continue without solution file
          hashi->_solutionFile = "";
        }
      }
      // add the row to the matrix
      hashi->_sol.push_back
       ({coordList[0], coordList[1], coordList[2], coordList[3]});
    }
  }

  // set the _solutionFile name for the Hashi class
  hashi->_solutionFile = _solutionFile;
}
