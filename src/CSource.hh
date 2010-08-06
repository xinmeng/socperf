#ifndef __CSOURCE_HH__
#define __CSOURCE_HH__

#include <vector>
#include <typeinfo>
using std::vector;

#include "CChannel.hh"

extern bool MatchSRCName(const string&);
extern bool IsStopTime();


class CSource
{
private:
  string _Name;

  CChannel *_SrcCh;

  CFacetCondition   *_FacetCondition;
  CChannelCondition *_ChannelCondition;

public:
  bool Done;

  CFacet *CurrentFacet;


public:
  inline CSource ( const string &name, CChannel *ch, CFacetCondition *condition ) : 
    _Name (name), _SrcCh (ch), 
    _FacetCondition (condition), _ChannelCondition (NULL),
    Done (false), CurrentFacet(NULL) {}

  inline CSource ( const string &name, CChannel *ch, CChannelCondition *condition ) : 
    _Name (name), _SrcCh (ch), 
    _FacetCondition (NULL), _ChannelCondition (condition),
    Done (false), CurrentFacet(NULL) {}


public:
  const string Name() ;
  const string ChName() ;
  const string CondName(); 


  void BackDoorWrite( CPMTransaction *transaction, const vector<CPacket*> &packets ) ;
  
  CFacet* PickUpFacet(CTransHandler *reader) ;
  void ClaimFacet();
  bool Update(uint64 new_offset, bool dst_done) ;
  bool DataAvailable(uint64 new_offset) ;
  bool Lock(CTransHandler *reader) ;
  bool Unlock(CTransHandler *reader) ;
  

#ifdef __DEBUG_VERSION__
protected:
  int __name_flag;
  int __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchSRCName(_Name);
    __time_flag = IsStopTime();

  }
#endif


};

#endif
