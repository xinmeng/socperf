#ifndef __TRANSFORM_HH__
#define __TRANSFORM_HH__

#include <vector>
#include <set>
#include <algorithm>
#include <string> 
using std::vector;
using std::set;
using std::string;

#include "CFacet.hh"
#include "CSource.hh"


extern uint64 Random();
extern uint64 Random(uint64 min, uint64 max);
extern uint64 Random(uint64 min, uint64 max, uint64 step);
extern unsigned int WeightedRandom(const vector<uint64> &);

extern bool MatchXFMName(const string &);
extern bool IsStopTime();


class CTransform
{
protected:
  string _Name;
  vector<CSource*> _Source;
  unsigned int _SourceCount;

public:
  inline CTransform(const string &name, unsigned int srccount) : 
    _Name (name), _SourceCount (srccount) {}

public:
  virtual vector<CPacket*> ApplyTransform (const vector<CSource*>&) ;
  virtual inline const string Name() {
    return "\"" + _Name + "\"";
  }

  virtual vector<CPacket*> Process (vector<CPacket*> packets) =0;

protected:
  virtual void BindSource(const vector<CSource*>&);

#ifdef __DEBUG_VERSION__
protected:
  bool __name_flag;
  bool __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchXFMName(_Name);
    __time_flag = IsStopTime();
  }
#endif
  
};



class CNameTransform : public CTransform
{
private:
  string _NewName;

public:
  inline CNameTransform (const string &name, const string &newname ) : 
    CTransform (name, 1), _NewName ( newname ) {}

  vector<CPacket*> Process ( vector<CPacket*> packets ) ;

};


class CCustomTransform : public CTransform
{
private:
  string _Key;
  string _Value;
  
public:
  inline CCustomTransform(const string &name, 
			  const string &key, const string &value) : 
    CTransform (name, 1), 
    _Key (key), _Value (value) {}

public:
  vector<CPacket*> Process(vector<CPacket*> packets) ;
};


enum SplitMethod {onSIZE, onLENGTH};
class CSplitTransform : public CTransform
{
private:
  SplitMethod _SplitMethod;
  uint64 _ChunkSize;
  uint64 _Remainder;


public:
  inline CSplitTransform(const string &name) : 
    CTransform (name, 1),
    _SplitMethod ( onSIZE ), _ChunkSize(32) {}

  inline CSplitTransform(const string &name, SplitMethod splitmethod, uint64 chunksize) : 
    CTransform (name, 1),
    _SplitMethod ( splitmethod ), _ChunkSize(chunksize) {}

  vector<CPacket*> Process(vector<CPacket*> packets);
};


class CNumTransform : public CTransform
{
protected:
  CPacketField<uint64> *_Field;

  bool _UseImmediate;
  uint64 _Immediate;
  
  bool _UseRandom;
  uint64 _Min, _Max, _Step;

public:
  inline CNumTransform(const string &name) : 
    CTransform (name, 1),
    _Field (NULL),
    _UseImmediate ( true ), _Immediate ( 0 ), 
    _UseRandom (true), _Min (0), _Max (0), _Step (0) {};

  inline CNumTransform(const string &name, uint64 immediate) : 
    CTransform (name, 1),
    _Field (NULL),
    _UseImmediate ( true ), _Immediate ( immediate ), 
    _UseRandom (false), _Min (0), _Max (0), _Step (0) {};

  inline CNumTransform(const string &name, 
		       uint64 min, uint64 max, uint64 step) : 
    CTransform (name, 1),
    _Field (NULL),
    _UseImmediate ( true ), _Immediate ( 0 ), 
    _UseRandom (true), _Min (min), _Max (max), _Step (step) {};

  inline CNumTransform(const string &name, CPacketField<uint64> *field) : 
    CTransform (name, 1),
    _Field (field),
    _UseImmediate ( false ), _Immediate ( 0 ), 
    _UseRandom (false), _Min (0), _Max (0), _Step (0) {};


  virtual vector<CPacket*> Process(vector<CPacket*> packets) =0;

  uint64 OperandValue() ; 
};



class CLengthTransform : public CNumTransform 
{
public: 
  inline CLengthTransform (const string &name, uint64 immediate) : 
    CNumTransform(name, immediate) {}

  inline CLengthTransform(const string &name, 
			  uint64 min, uint64 max, uint64 step) : 
    CNumTransform(name, min, max, step) {}

  vector<CPacket*> Process(vector<CPacket*> packets );

};


class CLengthTransformPkt : public CNumTransform 
{
public: 
  inline CLengthTransformPkt (const string &name, CPacketField<uint64> *field) : 
    CNumTransform(name, field) {}

  vector<CPacket*> ApplyTransform(const vector<CSource*>&);
  vector<CPacket*> Process(vector<CPacket*> packets );

};




class CSizeTransform : public CNumTransform 
{
public: 
  inline CSizeTransform (const string &name, uint64 immediate) : 
    CNumTransform(name, immediate) {}

  inline CSizeTransform(const string &name, uint64 min, uint64 max, uint64 step) : 
    CNumTransform(name, min, max, step) {}

  vector<CPacket*> Process (vector<CPacket*> packets ) ;
};



class CSizeTransformPkt : public CNumTransform 
{
public: 
  inline CSizeTransformPkt (const string &name, CPacketField<uint64> *field) : 
    CNumTransform(name, field) {}

  vector<CPacket*> ApplyTransform(const vector<CSource*> &source);
  vector<CPacket*> Process (vector<CPacket*> packets ) ;
};




class CGroupTransform : public CTransform 
{
private: 
  vector<CTransform*> _TransformList;
  
public: 
  inline CGroupTransform(const string &name, unsigned int count) : CTransform (name, count) {}

  void AddTransform(CTransform *tranform ) ;
  const string Name() ;
  vector<CPacket*> ApplyTransform(const vector<CSource*> &) ;
  vector<CPacket*> Process(vector<CPacket*> packets) ;
};


class CRandomTransform : public CTransform
{
private:
  vector<CTransform*> _TransformList;
  vector<uint64>      _Weight;

  CTransform *_CurrentTransform;


public:
  inline CRandomTransform(const string &name, unsigned int count) : CTransform (name, count) {};

  void AddTransform(CTransform* transform, uint64 weight=1); 
  const string Name() ;
  vector<CPacket*> ApplyTransform(const vector<CSource*>&) ;
  vector<CPacket*> Process(vector<CPacket*> packets) ;
};


// class CMultiTransform : public CTransform
// {
// private:
//   vector<CSource*> _SourceList;
  
//   map<CSource*, uint64> PacketSize, PacketLength;
//   map<CSource*, string> PacketName;
//   map<CSource*, map<string, string> > CustomFields;

// public:
//   inline CMultiTransform(const string &name, const vector<CSource*> &sources) :
//     CTransform(name), _SourceList (sources) {
//     for ( int i=0; i<_SourceList.size(); i++) {
//       PacketSize[_SourceList[i]] = 
//     }
//   }

// };


#endif
