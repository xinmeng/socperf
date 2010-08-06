#ifndef __CCHANNEL_HH__
#define __CCHANNEL_HH__

#include <set>
#include <list>
#include <vector>
#include <map> 
#include <sstream>
using std::set;
using std::list;
using std::vector;
using std::map;
using std::stringstream;

#include "CInterface.hh"
#include "CCondition.hh"
#include "CBite.hh"

#include "CReport.hh"
#include "CDumper.hh"


extern bool MatchCHName(const string &);
extern bool IsStopTime();



class CTransHandler ;

class CChannel : virtual public sc_prim_channel, 
		 virtual public sc_module,
		 public CInterface

		 
{
protected:
  string _Name;

public:
  inline CChannel (sc_module_name name ) : sc_module ( name ){
    _Name = name ;
    request_update();
  }
  
  virtual uint64 GetReaderCount() =0;
  virtual uint64 GetWriterCount() =0;
  virtual inline const string Name() {return "\"" + _Name + "\"";}

  virtual void Fill( vector<CFacet*> facets ) =0;


#ifdef __DEBUG_VERSION__
protected:
  // for debug
  bool __name_flag;
  bool __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchCHName(_Name);
    __time_flag = IsStopTime();

  }
#endif


};




class CSimpleChannel : public CChannel 
{
protected:
  list<CFacet*> _Content;

  uint64 _Capacitance, _Occupied;
  uint64 _ReaderCount, _WriterCount;
  


  set<CTransHandler*> _CurrentReader, _CurrentWriter;

  bool _Finite;
  bool _Ordered;

  map<string, double> _PacketFlow;

public: 
  inline CSimpleChannel ( sc_module_name name, bool ordered, uint64 capacitance ) : 
    CChannel ( name ), 
    _Capacitance (capacitance), _Occupied(0),
    _ReaderCount(1), _WriterCount(1), 
    _Ordered (ordered) 
  {
    if ( !capacitance ) 
      _Finite = false;
    else 
      _Finite = true;
  }

  virtual inline void SetReaderCount(uint64 c) {_ReaderCount = c;}
  virtual inline uint64 GetReaderCount() {return _ReaderCount;}

  virtual inline void SetWriterCount(uint64 c) {_WriterCount = c;}
  virtual inline uint64 GetWriterCount() {return _WriterCount;}

  virtual void Fill( vector<CFacet*> facets );

  virtual bool LockRd(CTransHandler *reader) ;

  virtual bool UnlockRd(CTransHandler *reader) ;

  virtual bool LockWr(CTransHandler *writer) ;

  virtual bool UnlockWr(CTransHandler *writer) ;


  // Common method 
  virtual float WaterMark ( ) ;
  virtual bool SpaceAvailable ( uint64 bite_size ) ;

  virtual CFacet* PickUpFacet(CFacetCondition *condition) ;
  virtual CFacet* PickUpFacet(CChannelCondition *condition) ;


  virtual void Write(CBite *bite) ;
  virtual void Pop(CFacet *facet) ;
  virtual bool Read(CFacet *facet, uint64 new_offset) ;

protected:
  virtual void update() ;

};



class CForwardChannel : public CSimpleChannel
{
public:
  inline CForwardChannel ( sc_module_name name ) : 
    CSimpleChannel ( name, true, 0 ) {
  }

  inline CForwardChannel ( sc_module_name name, uint64 capacitance ) : 
    CSimpleChannel ( name, true, capacitance ) {
  }

  inline CForwardChannel ( sc_module_name name, bool ordered, uint64 capacitance ) : 
    CSimpleChannel ( name, ordered, capacitance ) {
  }


};



class CAssembleChannel : public CSimpleChannel
{
public:
  inline CAssembleChannel ( sc_module_name name ) : 
    CSimpleChannel ( name, true, 0 ) {
  }

  inline CAssembleChannel ( sc_module_name name, uint64 capacitance ) : 
    CSimpleChannel ( name, true, capacitance ) {
  }

  inline CAssembleChannel ( sc_module_name name, bool ordered, uint64 capacitance ) : 
    CSimpleChannel ( name, ordered, capacitance ) {
  }

  
private:
  map<CPacket*, set<CFacet*> > _AssemblePool;

  CFacet* MergeFacets( const set<CFacet*> & facet_set ) ;
  void Assemble(void) ;


protected:
  virtual inline void update() { Assemble(); CSimpleChannel::update(); }


public:
  void Write( CBite *bite ) ;

  
  
};


class CAccumulateChannel : public CSimpleChannel 
{
public:
  inline CAccumulateChannel ( sc_module_name name ) : 
    CSimpleChannel ( name, true, 0 ) {
  }

  inline CAccumulateChannel ( sc_module_name name, uint64 capacitance ) : 
    CSimpleChannel ( name, true, capacitance ) {
  }

  inline CAccumulateChannel ( sc_module_name name, bool ordered, uint64 capacitance ) : 
    CSimpleChannel ( name, ordered, capacitance ) {
  }


private:
  map<string, CFacet*> _AccumulatedFacets;
  
public:
  void Write(CBite *bite) ;


  CFacet* PickUpFacet(CFacetCondition *condition);

  bool Read ( CFacet *facet, uint64 new_offset) ;

  void Pop (CFacet *facet) ;



  
};




#endif
