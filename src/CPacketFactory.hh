#ifndef __CPACKETFACTORY_HH__
#define __CPACKETFACTORY_HH__

#include "CPacket.hh"

extern uint64 Random(uint64, uint64, uint64);

class CPacketFactory 
{
private:
  string _Name;
  
  bool _RandomSize;
  uint64 _Size, _SizeMin, _SizeMax, _SizeStep;

  bool _RandomLength;
  uint64 _Length, _LengthMin, _LengthMax, _LengthStep;

  bool _RandomDelay;
  uint64 _Delay, _DelayMin, _DelayMax, _DelayStep;
  sc_time_unit _DelayUnit;

  bool _RandomCount;
  uint64 _Count, _CountMin, _CountMax, _CountStep;

  inline CPacketFactory () {}

public:
  inline CPacketFactory ( const string & name ) : 
    _Name ( name ), 
    _RandomSize ( true ), _Size (0), _SizeMin (1), _SizeMax(RAND_MAX), _SizeStep (1),
    _RandomLength ( true ), _Length (0), _LengthMin (1), _LengthMax(RAND_MAX), _LengthStep (1),
    _RandomDelay ( true ), _Delay (0), _DelayMin (1), _DelayMax(RAND_MAX), _DelayStep (1), _DelayUnit (SC_NS),
    _RandomCount ( false ), _Count (1), _CountMin (1), _CountMax (RAND_MAX), _CountStep (1) {}

  void SetSize( uint64 size ) ;
  void RandomSize( uint64 min, uint64 max, uint64 step=1) ;


  void SetLength( uint64 length ) ;
  void RandomLength( uint64 min, uint64 max, uint64 step=1) ;


  void SetDelay( uint64 delay ) ;
  void RandomDelay( uint64 min, uint64 max, uint64 step=1) ;

  void SetCount( uint64 count ) ;
  void RandomCount( uint64 min, uint64 max, uint64 step=1) ;


  vector<CPacket*>& GenPacket() ;
  
  


};


#endif
