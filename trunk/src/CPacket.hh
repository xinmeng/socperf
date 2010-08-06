#ifndef __PACKET_HH__
#define __PACKET_HH__

#include <systemc.h>
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <iostream>

using std::string;
using std::stack;
using std::map;
using std::vector;
using std::ostream;



class CPacket;

struct SplitStatus {
  CPacket *Source;
  uint64 TotalCount;

  inline SplitStatus (CPacket* src, uint64 total) : 
    Source ( src ), TotalCount (total) {}
};

enum AccumulateKey {NONE=0, SIZE, LENGH};

class CPacket 
{
public:
  string Name;

  uint64 Size, DstTransferredSize, ChTransferredSize, Offset;
  uint64 Length;

  uint64 ExtraDelay;
    
  double CreateTimeS, CreateTimeE, WaitTimeS, WaitTimeE, StoreTimeS, StoreTimeE;

  map<string, string> CustomFields;

  AccumulateKey Accumulate;
  
  stack<SplitStatus*> SplitStack;


  bool Picked;


public: 
  inline CPacket ( const string & name, 
		   uint64 size, uint64 length, 
		   uint64 delay, 
		   AccumulateKey accumulate=NONE ) : 
    Name ( name ), 
    Size ( size ), DstTransferredSize (0), ChTransferredSize (0), Offset (0),
    Length ( length) , 
    ExtraDelay ( delay ), 
    CreateTimeS (0), CreateTimeE (0),
    WaitTimeS (0), WaitTimeE (0),
    StoreTimeS (0), StoreTimeE (0),
    Accumulate ( accumulate ),
    Picked (false) {}


  inline CPacket ( const CPacket &packet ) : 
    Name ( packet.Name ), 
    Size ( packet.Size ), DstTransferredSize (0), ChTransferredSize (0), Offset (0),
    Length (packet.Length),
    ExtraDelay (0), 
    CreateTimeS (0), CreateTimeE (0),
    WaitTimeS (0), WaitTimeE (0),
    StoreTimeS (0), StoreTimeE (0),
    CustomFields (packet.CustomFields),
    Accumulate (packet.Accumulate),
    SplitStack (packet.SplitStack),
    Picked (false) {}


  inline bool IsChComplete() {
    return ChTransferredSize == Size;
  }

  inline bool IsDstComplete() {
    return DstTransferredSize == Size;
  }

  friend inline ostream& operator<<(ostream& os, const CPacket& packet) {
    os << "   Name: " << packet.Name << endl
       << "   Size: " << packet.Size << endl
       << " Length: " << packet.Length << endl
       << "  Delay: " << packet.ExtraDelay << endl
       << endl;

    return os;
  }

  inline void SaveSplitInfo ( SplitStatus *ss ) {
    SplitStack.push ( ss );
  }
  
  inline SplitStatus* IsSplit() {
    if ( SplitStack.empty() ) {
      return NULL;
    }
    else {
      return SplitStack.top();
    }
  }

private: 
  inline CPacket () {}

};



#endif
