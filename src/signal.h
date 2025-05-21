#ifndef SIGNAL_H
#define SIGNAL_H

#include <string>
#include <vector>

#include "edf.h"

#include <eigen3/Eigen/Dense>

class Channel {
public:
  Channel(std::string& label, std::string& type, std::vector<float>& data, const float degree)
  : _label(label)
  , _type(type)
  , _data(data)
  , _degree(degree)
  {}

public:
  std::string& getLabel() { return _label; }
  std::string getType() { return _type; }
  const float getDegree() { return _degree; }
  std::vector<float>& getData() { return _data; }

private:
  std::string        _label;
  std::string        _type;
  std::vector<float> _data;
  const float        _degree;
};


class Signal {
public:
  Signal();
  Signal(std::vector<Channel>& data)
  : _data(data)
  {}

  Signal(EDF& edf);

public:
  std::pair<size_t, size_t> shape() {
    return std::make_pair(_data.size(), _data[0].getData().size());
  }
  std::vector<std::string> getChannelNames();
  Eigen::MatrixXf getData();
  //Channel& getChannel(size_t index);
  //Channel& getChannel(std::string channelName);

  // TODO: Slicing

private:
  std::vector<Channel> _data;
};

#endif // SIGNAL_H