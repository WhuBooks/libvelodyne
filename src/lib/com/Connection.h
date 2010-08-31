#ifndef CONNECTION_H
#define CONNECTION_H

#include "IOException.h"

#include <stdint.h>

struct LaserData {
  uint16_t mu16Distance;
  uint8_t mu8Intensity;
};

struct VelodynePacket {
  uint16_t mu16HeaderInfo;
  uint16_t mu16RotationalInfo;
  LaserData maLaserData[32];
  uint16_t mu16SpinCount;
  uint32_t mu32Reserved;
};

class Connection {
  Connection(const Connection &other);
  Connection& operator = (const Connection &other);

  int32_t readBytes() const throw(IOException);

  static const uint16_t mcu16PacketSize = 1206;

  double mf64Timeout;
  int32_t mi32Socket;
  uint8_t mau8Packet[mcu16PacketSize];
  uint16_t mu16PacketPos;

public:
  Connection(double f64Timeout = 2.5);
  ~Connection();

  double getTimeout() const;

  void setTimeout(double f64Time);

  void open() throw(IOException);

  void close() throw(IOException);

  bool isOpen() const;

  const VelodynePacket& receivePacket() throw(IOException);

protected:

};

#endif // CONNECTION_H