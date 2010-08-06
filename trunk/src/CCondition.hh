#ifndef __CCONDITION_HH__
#define __CCONDITION_HH__

#include "CFacet.hh"
#include "CPacketField.hh"
#include <assert.h>

// operation mode is encapsulated in CFacetCondition
//   SERIAL_AHEAD : entire packet is popped out once
//                  transformation is started
//   SERIAL_POST  : entire packet is popped out when
//                  transformation is finished
//   PIPELINE     : Pop data along with transformation
typedef enum OPMODE { SERIAL_AHEAD, SERIAL_POST, PIPELINE };

class CFacetCondition
{
public:
  string TranscationName;
  OPMODE Mode;

protected:
  string PacketName;
  uint64 PacketSize, PacketLength;
  map<string, string> CustomFields;

  inline void _GetField(CFacet *facet) {
    PacketName   = (*facet)->Name;
    PacketSize   = (*facet)->Size;
    PacketLength = (*facet)->Length;
    CustomFields = (*facet)->CustomFields;
  }

public:
  inline CFacetCondition ( const string &name ) : TranscationName (name), Mode ( PIPELINE ) {}
  inline CFacetCondition ( const string &name, OPMODE mode ) : TranscationName (name), Mode ( mode ) {}

  inline CFacetCondition () : TranscationName (""), Mode (PIPELINE) {}
  inline CFacetCondition (OPMODE mode) : TranscationName (""), Mode (mode) {}

  virtual inline bool operator() ( CFacet *facet ) {
    _GetField(facet);

    if ( TranscationName != "" && TranscationName != facet->Transaction->Name() ) 
      return false;
    else if ( (Mode == SERIAL_AHEAD || Mode == SERIAL_POST) && ! (*facet)->IsChComplete() ) 
      return false;
    else if ( (*facet)->Picked )
      return false;
    else 
      return true;
      
  }
};


#define F_COND_WITH_TRANSNAME(ptrname, transname, mode, exp)					\
  class CCond_##ptrname : public CFacetCondition {						\
  public:											\
  inline CCond_##ptrname(const string &name, OPMODE mode) : CFacetCondition(name, mode) {}	\
    virtual inline bool operator() (CFacet *facet) {						\
      bool prechek = CFacetCondition::operator() (facet);					\
      if ( ! prechek )										\
	return false;										\
      else											\
	return exp;										\
    }												\
  };												\
  CCond_##ptrname *ptrname = new CCond_##ptrname (transname, mode);

#define F_COND_WITHOUT_TRANSNAME(ptrname, mode, exp)			\
  class CCond_##ptrname : public CFacetCondition {			\
  public:								\
    inline CCond_##ptrname(OPMODE mode) : CFacetCondition(mode) {}	\
    virtual inline bool operator() (CFacet *facet) {			\
      bool prechek = CFacetCondition::operator() (facet);		\
      if ( ! prechek )							\
	return false;							\
      else								\
	return exp;							\
    }									\
  };									\
  CCond_##ptrname *ptrname = new CCond_##ptrname (mode);		\



class CChannelCondition
{
private:
  float _Floor, _Ceiling;
public:
  inline CChannelCondition() : _Floor(0.), _Ceiling (1.) {};
  inline CChannelCondition(float floor, float ceiling = 1. ) : 
    _Floor (floor), _Ceiling (ceiling) {
    assert(floor < ceiling && floor >=0 && ceiling <= 1);
  }

  inline bool operator() (float water_mark) {
    return water_mark >= _Floor && water_mark <= _Ceiling;
  }

};

#define C_COND(ptrname, floor, ceiling) \
CChannelCondition *ptrname = new CChannelCondition (floor, ceiling);


#endif
