#include "signal.h"

Signal::Signal(EDF& edf) {
  std::vector<std::pair<std::string, std::vector<float>>> signalsData = edf.getSignalsData();
  std::vector<std::string> labels = edf.getSignalLabels();
  std::vector<float> degrees = edf.getSignalDegrees();
  for (size_t i {0}; i < signalsData.size(); ++i) {
    _data.push_back(Channel(signalsData[i].first, labels[i], signalsData[i].second, degrees[i]));
  }
}


std::vector<std::string> Signal::getChannelNames() {
  std::vector<std::string> ch_names;
  for (Channel ch : _data) {
    ch_names.push_back(ch.getLabel());
  }
  
  return ch_names;
}


Eigen::MatrixXf Signal::getData() {
  std::vector<float> flatten_data;
  size_t rows = _data.size();
  size_t cols = _data[0].getData().size();

  flatten_data.reserve(rows * cols);
  for (auto& channel : _data) {
    flatten_data.insert(flatten_data.end(), channel.getData().begin(), channel.getData().end());
  }

  Eigen::MatrixXf data = Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(flatten_data.data(), rows, cols);
  return data;
}