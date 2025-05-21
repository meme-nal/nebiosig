#include "edf.h"

EDF::EDF(const std::string& filepath) {
  std::ifstream edfFile(filepath, std::ios::binary);
  if (!edfFile.is_open()) {
    throw std::runtime_error("Failed to open EDF file");
  }

  // READING FIRST PART OF EDF
  EDFHeader header;
  edfFile.read(reinterpret_cast<char*>(header.version.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.patientID.data()), 80);
  edfFile.read(reinterpret_cast<char*>(header.recordingID.data()), 80);
  edfFile.read(reinterpret_cast<char*>(header.startDate.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.startTime.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.numBytesHeader.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.reserved.data()), 44);
  edfFile.read(reinterpret_cast<char*>(header.numRecords.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.recordDuration.data()), 8);
  edfFile.read(reinterpret_cast<char*>(header.numSignals.data()), 4);

  _version = header.version[0] - '0';
  int filenamePositionStart = filepath.find_last_of('/');
  _filename = filepath.substr(filenamePositionStart + 1, filepath.size() - 1);
  _patientId = std::string(header.patientID.begin(), header.patientID.end());
  _recordingId = std::string(header.recordingID.begin(), header.recordingID.end());
  _startDate = std::string(header.startDate.begin(), header.startDate.end());
  _startTime = std::string(header.startTime.begin(), header.startTime.end());
  std::string numSignals(header.numSignals.begin(), header.numSignals.end());
  std::string numBytesHeader(header.numBytesHeader.begin(), header.numBytesHeader.end());
  _numBytesHeader = std::stoi(numBytesHeader);
  _reserved = std::string(header.reserved.begin(), header.reserved.end());
  std::string numRecords(header.numRecords.begin(), header.numRecords.end());
  _numRecords = std::stoi(numRecords);
  std::string recordDuration(header.recordDuration.begin(), header.recordDuration.end());
  _recordDuration = std::stof(recordDuration);
  int ns = std::stoi(numSignals);
  _numSignals = ns;

  // GO TO LAST PART
  edfFile.seekg(256);

  // READING LAST PART OF EDF
  EDFHeaderLast headerLast(ns);

  edfFile.read(reinterpret_cast<char*>(headerLast.labels.data()), ns * 16);
  edfFile.read(reinterpret_cast<char*>(headerLast.transducerTypes.data()), ns * 80);
  edfFile.read(reinterpret_cast<char*>(headerLast.physicalDimension.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.physicalMinimum.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.physicalMaximum.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.digitalMinimum.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.digitalMaximum.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.prefiltering.data()), ns * 80);
  edfFile.read(reinterpret_cast<char*>(headerLast.numSamplesInEachDataRecord.data()), ns * 8);
  edfFile.read(reinterpret_cast<char*>(headerLast.reserved.data()), ns * 32);

  // LABELS READING
  std::string tmpStr = "";
  for (size_t i {0}; i < headerLast.labels.size(); ++i) {
    if ((i + 1) % 16 == 0) {
      _labels.push_back(tmpStr);
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.labels[i];
  }

  // TRANSDUCER TYPES READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.transducerTypes.size(); ++i) {
    if ((i + 1) % 80 == 0) {
      _transducerTypes.push_back(tmpStr);
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.transducerTypes[i];
  }

  // PHYSICAL DIMENSIONS READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.physicalDimension.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _physicalDimensions.push_back(tmpStr);
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.physicalDimension[i];
  }
  for (size_t i {0}; i < _physicalDimensions.size(); ++i) {
    if (_physicalDimensions[i][0] == 'm') {
      _physicalDimensionDegrees.push_back(MILLI);
    } else if (_physicalDimensions[i][0] == 'u') {
      _physicalDimensionDegrees.push_back(MICRO);
    } else if (_physicalDimensions[i][0] == 'n') {
      _physicalDimensionDegrees.push_back(NANO);
    }
  }

  // PHYSICAL MINIMUM READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.physicalMinimum.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _physicalMinimums.push_back(std::stof(tmpStr));
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.physicalMinimum[i];
  }

  // PHYSICAL MAXIMUM READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.physicalMaximum.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _physicalMaximums.push_back(std::stof(tmpStr));
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.physicalMaximum[i];
  }

  // DIGITAL MINIMUM READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.digitalMinimum.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _digitalMinimums.push_back(std::stof(tmpStr));
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.digitalMinimum[i];
  }

  // DIGITAL MAXIMUM READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.digitalMaximum.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _digitalMaximums.push_back(std::stof(tmpStr));
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.digitalMaximum[i];
  }

  // PREFILTERINGS READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.prefiltering.size(); ++i) {
    if ((i + 1) % 80 == 0) {
      _prefilterings.push_back(tmpStr);
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.prefiltering[i];
  }

  // NUMBER OF SAMPLES IN EACH DATA RECORD READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.numSamplesInEachDataRecord.size(); ++i) {
    if ((i + 1) % 8 == 0) {
      _numSamples.push_back(std::stoi(tmpStr));
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.numSamplesInEachDataRecord[i];
  }

  // RESERVED LAST READING
  tmpStr = "";
  for (size_t i {0}; i < headerLast.reserved.size(); ++i) {
    if ((i + 1) % 32 == 0) {
      _reservedLast.push_back(tmpStr);
      tmpStr = "";
      continue;
    }

    tmpStr += headerLast.reserved[i];
  }

  // READING DATA RECORDS
  std::vector<std::vector<short>> dataTmp;
  for (size_t i {0}; i < ns; ++i) {
    std::vector<short> dataRecord(_numSamples[i] * _numRecords);
    edfFile.read(reinterpret_cast<char*>(dataRecord.data()), _numSamples[i] * sizeof(short));
    dataTmp.push_back(dataRecord);
  }

  edfFile.close();
  
  // READING AND MAPPING THE MAIN DATA
  for (size_t i {0}; i < ns - 1; ++i) { // because last signal is annotations
    std::vector<float> channelValues;
    for (size_t j {0}; j < dataTmp[i].size(); ++j) {
      float channelValue = static_cast<float>(dataTmp[i][j]);
      float mappedChannelValue = (channelValue - _digitalMinimums[i]) / (_digitalMaximums[i] - _digitalMinimums[i]) * (_physicalMaximums[i] - _physicalMinimums[i]) + _physicalMinimums[i];
      channelValues.push_back(mappedChannelValue);
    }
    _data.push_back(std::make_pair(_labels[i], channelValues));
  }
}

void EDF::printInfo() const {
  // additional variables for pretty printed text
  std::string horLine(119, '-');

  std::cout << "#" + horLine + "#\n";
  std::cout << "# filename: " << _filename << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# version of file: " << _version << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# patient identification: " << _patientId << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# recording identification: " << _recordingId << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# start date: " << _startDate << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# start time: " << _startTime << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# number of header bytes: " << _numBytesHeader << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# reserved: " << _reserved << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# number of records: " << _numRecords << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# record duration: " << _recordDuration << " s" << "\n";
  std::cout << '#' + horLine + "#\n";
  std::cout << "# number of signals: " << _numSignals << "\n";
  std::cout << '#' + horLine + "#\n";

  std::cout << "# labels: \n";
  for (size_t i {0}; i < _labels.size(); ++i) {
    std::cout << "# " << _labels[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# transducer types: \n";
  for (size_t i {0}; i < _transducerTypes.size(); ++i) {
    std::cout << "# " << _transducerTypes[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# physical dimensions: \n";
  for (size_t i {0}; i < _physicalDimensions.size(); ++i) {
    std::cout << "# " << _physicalDimensions[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# physical minimums: \n";
  for (size_t i {0}; i < _physicalMinimums.size(); ++i) {
    std::cout << "# " << _physicalMinimums[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# physical maximums: \n";
  for (size_t i {0}; i < _physicalMaximums.size(); ++i) {
    std::cout << "# " << _physicalMaximums[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# digital minimums: \n";
  for (size_t i {0}; i < _digitalMinimums.size(); ++i) {
    std::cout << "# " << _digitalMinimums[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# digital maximums: \n";
  for (size_t i {0}; i < _digitalMaximums.size(); ++i) {
    std::cout << "# " << _digitalMaximums[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# prefilterings: \n";
  for (size_t i {0}; i < _prefilterings.size(); ++i) {
    std::cout << "# " << _prefilterings[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# number of samples in each data record: \n";
  for (size_t i {0}; i < _numSamples.size(); ++i) {
    std::cout << "# " << _numSamples[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";

  std::cout << "# reserved last: \n";
  for (size_t i {0}; i < _reservedLast.size(); ++i) {
    std::cout << "# " << _reservedLast[i] << "\n";
  }
  std::cout << '#' + horLine + "#\n";
}