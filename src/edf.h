#ifndef EDF_H
#define EDF_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

/*
* EDF+ FILE CONSISTS OF HEADER AND DATA RECORDS
* HEADER CONSISTS OF TWO PARTS
* FIRST IS ALWAYS 256 BYTES
* SECOND IS NUMBER OF SIGLANS (ns) * 256 BYTES 
* DATA RECORDS CONSIST OF (ns) * NUMBER OF SAMPLES IN CURRENT SIGNAL BYTES
*/

const float MILLI = 0.001;
const float MICRO = 0.000001;
const float NANO  = 0.000000001;

/* THE FIRST PART OF EDF+ HEADER IS 256 BYTES */
struct EDFHeader {
  EDFHeader() {
    version.resize(8);
    patientID.resize(80);
    recordingID.resize(80);
    startDate.resize(8);
    startTime.resize(8);
    numBytesHeader.resize(8);
    reserved.resize(44);
    numRecords.resize(8);
    recordDuration.resize(8);
    numSignals.resize(4);
  }

  std::vector<char> version; // MUST BE 0
  std::vector<char> patientID; 
  std::vector<char> recordingID;
  std::vector<char> startDate;
  std::vector<char> startTime;
  std::vector<char> numBytesHeader;
  std::vector<char> reserved;
  std::vector<char> numRecords; // -1 IF UNKNOWN 
  std::vector<char> recordDuration; // IN SECONDS BETWEEN 0 AND 1
  std::vector<char> numSignals;
};


/* THE LAST PART OF EDF+ HEADER IS ns * 256 */
struct EDFHeaderLast {
  EDFHeaderLast(const int ns) {
    labels.resize(ns * 16);
    transducerTypes.resize(ns * 80);
    physicalDimension.resize(ns * 8);
    physicalMinimum.resize(ns * 8);
    physicalMaximum.resize(ns * 8);
    digitalMinimum.resize(ns * 8);
    digitalMaximum.resize(ns * 8);
    prefiltering.resize(ns * 80);
    numSamplesInEachDataRecord.resize(ns * 8);
    reserved.resize(ns * 32);
  }

  std::vector<char> labels; // LIST OF CHANNEL NAMES
  std::vector<char> transducerTypes;
  std::vector<char> physicalDimension;
  std::vector<char> physicalMinimum;
  std::vector<char> physicalMaximum;
  std::vector<char> digitalMinimum;
  std::vector<char> digitalMaximum;
  std::vector<char> prefiltering; // HIGH PASS AND LOW PASS FILTERS INFORMATION FOR EACH CHANNEL
  std::vector<char> numSamplesInEachDataRecord;
  std::vector<char> reserved;
};


class EDF {
public:
  EDF(const std::string& filepath);
  void printInfo() const;
  std::vector<std::pair<std::string, std::vector<float>>> getSignalsData() {
    return _data;
  }
  std::vector<std::string> getSignalLabels() const {
    return _labels;
  }
  std::vector<float> getSignalDegrees() const {
    return _physicalDimensionDegrees;
  }

private:
  int _version;
  std::string _filename;
  std::string _patientId;
  std::string _recordingId;
  std::string _startDate;
  std::string _startTime;
  int _numBytesHeader;
  std::string _reserved;
  int _numRecords;
  float _recordDuration;
  int _numSignals;

  std::vector<std::string> _labels;
  std::vector<std::string> _transducerTypes;
  std::vector<std::string> _physicalDimensions;
  std::vector<float>      _physicalDimensionDegrees;
  std::vector<float> _physicalMinimums;
  std::vector<float> _physicalMaximums;
  std::vector<float> _digitalMinimums;
  std::vector<float> _digitalMaximums;
  std::vector<std::string> _prefilterings;
  std::vector<int> _numSamples;  
  std::vector<std::string> _reservedLast;

  std::vector<std::pair<std::string, std::vector<float>>> _data;

private:
  const size_t EDF_VERSION_SIZE = 8;
  const size_t EDF_HEADER_FIELD_SIZE = 80;
};

#endif // EDF_H