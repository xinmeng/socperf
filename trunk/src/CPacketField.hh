#ifndef __CPACKETFIELD_HH__
#define __CPACKETFIELD_HH__

#include <string>
using std::string;

#include "CFacet.hh"

template <class T> class CPacketField
{
public:
  virtual T& operator() (CPacket *packet) =0;
};


class CPacketName : public CPacketField <string>
{
public:
  inline string& operator() (CPacket *packet) {
    return packet->Name;
  }
};

class CPacketUserField : public CPacketField <string>
{
private:
  string _Key;

  inline CPacketUserField() {};

public:
  inline CPacketUserField (const string& key )  : _Key ( key ) {}
  inline string& operator () (CPacket *packet ) {
    return packet->CustomFields[_Key];
  }
};



class CPacketSize : public CPacketField <uint64> 
{
public: 
  inline uint64& operator() (CPacket *packet) {
    return packet->Size;
  }
};


class CPacketLength : public CPacketField <uint64> 
{
public: 
  inline uint64& operator() (CPacket *packet) {
    return packet->Length;
  }
};


// class CPacketArrivalTime : public CPacketField <uint64> 
// {
// public: 
//   inline uint64& operator() (CPacket *packet) {
//     uint64 *puint = new uint64 ( packet->ArrivalTime.value());
//     return *puint;
//   }
// };

// class CPacketLeaveTime : public CPacketField <uint64> 
// {
// public: 
//   inline uint64& operator() (CPacket *packet) {
//     uint64 *puint = new uint64 (packet->LeaveTime.value());
//     return *puint;
//   }
// };


#endif
