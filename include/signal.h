#ifndef SIGNAL_H
#define SIGNAL_H

#include <string>
#include <vector>

#include "edf.h"

#include "ext/Eigen/Dense"

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


class Annotation {
public:
  Annotation();
  Annotation(const std::string& eventName, size_t start, size_t end)
  : _eventName(eventName)
  , _start(start)
  , _end(end)
  {}

private:
  std::string _eventName;
  size_t      _start;
  size_t      _end;
};


class Signal {
public:
  Signal();
  Signal(std::vector<Channel>& data, std::vector<Annotation>& annotations)
  : _data(data)
  , _annotations(annotations)
  {}

  Signal(EDF& edf);

public:
  std::pair<size_t, size_t> shape() {
    return std::make_pair(_data.size(), _data[0].getData().size());
  }
  std::vector<std::string> getChannelNames();
  Eigen::MatrixXf getData();
  Channel& getChannel(const size_t index);
  Channel& getChannel(const std::string& channelName);

  // TODO: Slicing

private:
  std::vector<Channel>    _data;
  std::vector<Annotation> _annotations;
};

#endif // SIGNAL_H