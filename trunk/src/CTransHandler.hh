#ifndef __CTRANSHANDLER_HH__
#define __CTRANSHANDLER_HH__

#include <string>
#include <vector> 
#include <set>
using std::string;
using std::vector;
using std::set;


#include "CTransform.hh"
#include "CDestination.hh"

#include "CDumper.hh"

extern bool MatchTHName(const string&);
extern bool IsStopTime();



class CTransHandler
{
protected:
  string _Name;

public:
  inline CTransHandler(const string &name) : _Name (name)  {}


  virtual inline const string Name() {return "\"" + _Name + "\"";}

  virtual TransferStatus Execute() =0;
  virtual bool IsReady() =0;

  virtual void Prepare() =0;
  virtual bool Resume() =0;
  virtual bool IsPrepared() =0;


#ifdef __DEBUG_VERSION__
protected:
  bool __name_flag;
  bool __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchTHName(_Name);
    __time_flag = IsStopTime();
  }
#endif


};

typedef enum LogicConeStateLiteral {LC_IDLE, 
				    LC_LOCK_SRC, 
				    LC_LOCK_DST, 
				    LC_TRANSFORM, 
				    LC_STREAMING, 
				    LC_DONE, 
				    LC_RESUME};

typedef enum THStateLiteral {TH_IDLE, TH_READY, TH_BUSY, TH_FLUSH, TH_RESUME};
class CSimpleTH : public CTransHandler
{
private:
  vector<vector<CSource*> > _SrcList;
  vector<LogicConeStateLiteral> _LogicConeState;
  set<CSource*> _SrcSet;

  vector<CDestination*> _DstList;
  vector<bool> _DstChLocked;
  vector<CTransform*> _TransformList;

  THStateLiteral _THState;

public:
  inline CSimpleTH(const string &name, 
		   CSource *src, CTransform *transform, CDestination *dst) : 
    CTransHandler(name), _THState (TH_IDLE) 
  {
    AddLogicCone(src, transform, dst);
  }


  inline CSimpleTH(const string &name, 
		   vector<CSource*> *src, CTransform *transform, CDestination *dst) : 
    CTransHandler(name), _THState (TH_IDLE) 
  {
    AddLogicCone(src, transform, dst);
  }

  TransferStatus Execute() ;
  void Prepare();
  bool Resume();
  TransferStatus Resume(const unsigned int &);

  void AddLogicCone(CSource *src, 
		    CTransform *transform, 
		    CDestination *dst);

  void AddLogicCone(vector<CSource*> *src, 
		    CTransform *transform, 
		    CDestination *dst );
  bool IsReady() ;
  bool IsPrepared() ;



private:
  void Init() ;

  TransferStatus ProcessLogicCone(const unsigned int &, const string &) ;


  int ExecTransform(int CommonIDX) ;
  
  bool Update(bool) ;
  
  void FlushSRC();

  TransferStatus PrepareLogicCone(const unsigned int &);
  TransferStatus ResumeLogicCone(const unsigned int &);
  TransferStatus TransferLogicCone(const unsigned int &);
    

  void THLog(int idx, TransferStatus status ) ;


};


class CGroupTH : public CTransHandler
{
protected:
  vector<CTransHandler*> _THList;
  vector<uint64> _Weight;
  CTransHandler *_CurrentTH;

public:
  inline CGroupTH (const string &name) : 
    CTransHandler (name),  _CurrentTH(NULL) {};

  virtual void AddTH(CTransHandler *th, uint64 weight=1) =0 ;

  virtual const string   Name()    =0;
  virtual bool           IsReady() =0;
  virtual TransferStatus Execute() =0;
  virtual void Prepare() =0;
  virtual bool Resume() =0;
  virtual bool IsPrepared() =0;

protected:
  virtual void SelectTH()=0;
};

class CRandomTH : public CGroupTH
{
public:
  inline CRandomTH(const string &name) : CGroupTH(name){}

  virtual void AddTH(CTransHandler *th, uint64 weight=1) =0 ;

  virtual const string   Name()    ;
  virtual bool           IsReady() ;
  virtual TransferStatus Execute() ;
  virtual void Prepare() ;
  virtual bool Resume();
  virtual bool IsPrepared();

protected:
  virtual void SelectTH();


};

class CRoundRobinTH : public CGroupTH
{
private:
  unsigned int _RRposition;

public:
  inline CRoundRobinTH(const string &name) : 
    CGroupTH(name), _RRposition(0)  {}

  virtual void AddTH(CTransHandler *th, uint64 weight=1)  ;

  virtual const string   Name()    ;
  virtual bool           IsReady() ;
  virtual TransferStatus Execute() ;
  virtual void Prepare() ;
  virtual bool Resume();
  virtual bool IsPrepared();
  

protected:
  virtual void SelectTH();

};


class CArbiterTH : public CGroupTH
{
private:
  uint64 _ArbRecord;
  uint64 _Selector;
  
public:
  inline CArbiterTH(const string &name) : CGroupTH(name), _ArbRecord(0), _Selector (0) {}

  virtual void AddTH(CTransHandler *th, uint64 weight=1)  ;

  virtual const string   Name()    ;
  virtual bool           IsReady() ;
  virtual TransferStatus Execute() ;
  virtual void Prepare() ;
  virtual bool Resume();
  virtual bool IsPrepared();

protected:
  virtual void SelectTH();
};


class CDispatchTH : public CGroupTH
{
private:
  set<CTransHandler*> _ReadyTHs;

public:
  inline CDispatchTH(const string &name) : CGroupTH(name) {}

  virtual void AddTH(CTransHandler *th, uint64 weight=1)  ;

  virtual const string   Name()    ;
  virtual bool           IsReady() ;
  virtual TransferStatus Execute() ;
  virtual inline void Prepare() {ERROR << "Dispatch can not operate in Prepare-Resume mode." << endl; exit(1);}
  virtual bool Resume() {ERROR << "Dispatch can not operate in Prepare-Resume mode." << endl; exit(1);}
  virtual bool IsPrepared() {ERROR << "Dispatch can not operate in Prepare-Resume mode." << endl; exit(1);}

protected:
  virtual void SelectTH();
};


#endif
