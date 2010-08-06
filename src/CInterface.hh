#ifndef __CINTERFACE_HH__
#define __CINTERFACE_HH__

#include <systemc.h>
#include "CCondition.hh"

class CTransHandler;
class CFacet;
class CBite;

class CInterface : virtual public sc_interface 
{
public :
  // Write interface
  virtual bool SpaceAvailable( uint64 bite_size ) =0;
  virtual float WaterMark () =0;
  virtual bool LockWr(CTransHandler *writer) =0;
  virtual bool UnlockWr(CTransHandler *writer) =0;
  virtual void Write (CBite *bite) =0;
  // virtual bool CheckCondition ( CChannelCondition *condition) =0;
  virtual CFacet *PickUpFacet ( CChannelCondition *condition) =0;

  // Read interface
  // virtual bool CheckCondition ( CFacetCondition *condition) =0;
  virtual CFacet *PickUpFacet ( CFacetCondition *condition) =0;
  // virtual bool DataAvailable ( CFacet *facet, uint64 offset) =0;
  virtual bool Read ( CFacet *facet, uint64 new_offset) =0;
  virtual void Pop (CFacet *facet) =0;
  virtual bool LockRd(CTransHandler *reader)=0;
  virtual bool UnlockRd(CTransHandler *reader) =0;



protected:
  inline CInterface() {}
  
private: 
  inline CInterface (const CInterface&) {}
  inline CInterface& operator=(const CInterface&) {}
  
};

#endif
