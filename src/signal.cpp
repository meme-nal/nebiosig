#include "signal.h"

Signal::Signal(EDF& edf) {
  std::vector<std::pair<std::string, std::vector<float>>> signalsData = edf.getSignalsData();
  std::vector<std::string> labels = edf.getSignalLabels();
  std::vector<float> degrees = edf.getSignalDegrees();
  for (size_t i {0}; i < signalsData.size(); ++i) {
    _data.push_back(Channel(signalsData[i].first, labels[i], signalsData[i].second, degrees[i]));
  }
}