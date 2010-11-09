#include "VelodynePointCloud.h"

#include <iostream>
#include <fstream>

#include <math.h>

using namespace std;

VelodynePointCloud::VelodynePointCloud() : mf64Timestamp(0) {
}

VelodynePointCloud::VelodynePointCloud(const VelodynePacket &vdynePacket,
    const VelodyneCalibration &vdyneCalibration) {
  mf64Timestamp = vdynePacket.getTimestamp();
  for (uint32_t i = 0; i < vdynePacket.mcu16DataChunkNbr; i++) {
    uint32_t u32IdxOffs = 0;
    const DataChunk &data = vdynePacket.getDataChunk(i);
    if (data.mu16HeaderInfo == vdynePacket.mcu16LowerBank)
      u32IdxOffs = data.mcu16LasersPerPacket;

    double f64Rotation =
      vdyneCalibration.deg2rad((double)data.mu16RotationalInfo /
      (double)vdynePacket.mcu16RotationResolution);

    for (uint32_t j = 0; j < data.mcu16LasersPerPacket; j++) {
      uint32_t u32LaserIdx = u32IdxOffs + j;

      double f64Distance = vdyneCalibration.getDistCorr(u32LaserIdx)
        + (double)data.maLaserData[j].mu16Distance /
        (double)vdynePacket.mcu16DistanceResolution;

      if (f64Distance < mcf64MinDistance)
        break;

      double f64SinRot = sin(f64Rotation) *
        vdyneCalibration.getCosRotCorr(u32LaserIdx) -
        cos(f64Rotation) * vdyneCalibration.getSinRotCorr(u32LaserIdx);
      double f64CosRot = cos(f64Rotation) *
        vdyneCalibration.getCosRotCorr(u32LaserIdx) +
        sin(f64Rotation) * vdyneCalibration.getSinRotCorr(u32LaserIdx);

      f64Distance /= (double)mcu16MeterConversion;
      double f64HorizOffsCorr =
        vdyneCalibration.getHorizOffsCorr(u32LaserIdx) /
        (double)mcu16MeterConversion;
      double f64VertOffsCorr =
        vdyneCalibration.getVertOffsCorr(u32LaserIdx) /
        (double)mcu16MeterConversion;

      double f64xyDist = f64Distance *
        vdyneCalibration.getCosVertCorr(u32LaserIdx) -
        f64VertOffsCorr * vdyneCalibration.getSinVertCorr(u32LaserIdx);

      Point3D point;
      point.mf64X = f64xyDist * f64SinRot - f64HorizOffsCorr * f64CosRot;
      point.mf64Y = f64xyDist * f64CosRot + f64HorizOffsCorr * f64SinRot;
      point.mf64Z = f64Distance *
        vdyneCalibration.getSinVertCorr(u32LaserIdx) + f64VertOffsCorr *
        vdyneCalibration.getCosVertCorr(u32LaserIdx);
      mPointCloudVector.push_back(point);
    }
  }
}

VelodynePointCloud::VelodynePointCloud(const VelodynePointCloud &other)
  : mf64Timestamp(other.mf64Timestamp),
    mPointCloudVector(other.mPointCloudVector) {
}

VelodynePointCloud& VelodynePointCloud::operator =
  (const VelodynePointCloud &other) {
  if (this != &other) {
    mf64Timestamp = other.mf64Timestamp;
    mPointCloudVector = other.mPointCloudVector;
  }
  return *this;
}

VelodynePointCloud::~VelodynePointCloud() {
}

void VelodynePointCloud::read(istream &stream) {
}

void VelodynePointCloud::write(ostream &stream) const {
  stream << "Timestamp: " << mf64Timestamp << endl;
  for (uint32_t i = 0; i < mPointCloudVector.size(); i++)
    stream << mPointCloudVector[i].mf64X << " "
           << mPointCloudVector[i].mf64Y << " "
           << mPointCloudVector[i].mf64Z << endl;
}

void VelodynePointCloud::read(ifstream &stream) {
}

void VelodynePointCloud::write(ofstream &stream) const {
  for (uint32_t i = 0; i < mPointCloudVector.size(); i++)
    stream << mPointCloudVector[i].mf64X << " "
           << mPointCloudVector[i].mf64Y << " "
           << mPointCloudVector[i].mf64Z << endl;
}

double VelodynePointCloud::getTimestamp() const {
  return mf64Timestamp;
}

vector<Point3D>::const_iterator VelodynePointCloud::getStartIterator() const {
  return mPointCloudVector.begin();
}

vector<Point3D>::const_iterator VelodynePointCloud::getEndIterator() const {
  return mPointCloudVector.end();
}

void VelodynePointCloud::setTimestamp(double f64Timestamp) {
  mf64Timestamp = f64Timestamp;
}

void VelodynePointCloud::pushPoint(const Point3D &point) {
  mPointCloudVector.push_back(point);
}

ostream& operator << (ostream &stream,
  const VelodynePointCloud &obj) {
  obj.write(stream);
  return stream;
}

istream& operator >> (istream &stream,
  VelodynePointCloud &obj) {
  obj.read(stream);
  return stream;
}

ofstream& operator << (ofstream &stream,
  const VelodynePointCloud &obj) {
  obj.write(stream);
  return stream;
}

ifstream& operator >> (ifstream &stream,
  VelodynePointCloud &obj) {
  obj.read(stream);
  return stream;
}