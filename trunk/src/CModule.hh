#ifndef __CMODULE_HH__
#define __CMODULE_HH__

#include "CTransHandler.hh"


extern bool MatchMODName(const string&);
extern bool IsStopTime();

class CModule
{
protected:
  vector<CTransHandler*> _THList;
  string _Name;

#ifdef __DEBUG_VERSION__
protected:
  bool __name_flag;
  bool __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchMODName(_Name);
    __time_flag = IsStopTime();

  }
#endif

public:
  inline CModule (const string & name) : _Name (name) {}
  
  inline const string Name() {return "\"" + _Name + "\"";}

  inline void AddTH( CTransHandler *th) { _THList.push_back(th);}
  inline void Execute() {

#ifdef __DEBUG_VERSION__
    SetDebugFlag();
#endif
    for (unsigned int i=0; i<_THList.size(); i++) {
      _THList[i]->Execute();
    }
  }
  
};

#include "CTransactionFactory.hh"

#endif
