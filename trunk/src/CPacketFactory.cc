#include "CPacketFactory.hh"

void
CPacketFactory::SetSize( uint64 size ) 
{
  _RandomSize = false;
  _Size = size;
}
void
CPacketFactory::RandomSize( uint64 min, uint64 max, uint64 step) 
{
  assert ( max > min && step < (max - min) ) ;

  _RandomSize = true;
  _SizeMin = min; 
  _SizeMax = max;
  _SizeStep = step; 
}


void
CPacketFactory::SetLength( uint64 length ) 
{
  _RandomLength = false;
  _Length = length;
}
void
CPacketFactory::RandomLength( uint64 min, uint64 max, uint64 step) 
{
  assert ( max > min && step < (max - min) ) ;

  _RandomLength = true;
  _LengthMin = min; 
  _LengthMax = max;
  _LengthStep = step; 
}


void
CPacketFactory::SetDelay( uint64 delay ) 
{
  _RandomDelay = false;
  _Delay = delay;
}
void
CPacketFactory::RandomDelay( uint64 min, uint64 max, uint64 step) 
{
  assert ( max > min && step < (max - min) ) ;

  _RandomDelay = true;
  _DelayMin = min; 
  _DelayMax = max;
  _DelayStep = step; 
}

void
CPacketFactory::SetCount( uint64 count ) 
{
  _RandomCount = false;
  _Count = count;
}
void
CPacketFactory::RandomCount( uint64 min, uint64 max, uint64 step) 
{
  assert ( max > min && step < (max - min) ) ;

  _RandomCount = true;
  _CountMin = min; 
  _CountMax = max;
  _CountStep = step; 
}


vector<CPacket*>&
CPacketFactory::GenPacket() 
{
  uint64 count;
    
  if ( _RandomCount ) {
    count = Random( _CountMin, _CountMax, _CountStep );
  }
  else {
    count = _Count;
  }

  vector<CPacket*> *vec = new vector<CPacket*> ( count );

  for (unsigned int i=0; i<count; i++) {
    uint64 size, length, delay;
    if  ( _RandomSize ) {
      size = Random(_SizeMin, _SizeMax, _SizeStep);
    }
    else {
      size = _Size;
    }
      

    if  ( _RandomLength ) {
      length = Random(_LengthMin, _LengthMax, _LengthStep);
    }
    else {
      length = _Length;
    }

    if  ( _RandomDelay ) {
      delay = Random(_DelayMin, _DelayMax, _DelayStep);
    }
    else {
      delay = _Delay;
    }

    (*vec)[i] = new CPacket ( _Name, size, length, delay);
    (*vec)[i]->CreateTimeS = -1;
    (*vec)[i]->CreateTimeE = -1;
  }

  return *vec;
}
