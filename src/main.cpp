#include "signal.h"

int main(int argc, char** argv) {
  std::string filepath = argv[1];

  EDF edf = EDF(filepath);

  Signal s(edf);
  

  return 0;
}
