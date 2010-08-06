#ifndef __DESTINATION_HH__
#define __DESTINATION_HH__

#include <systemc.h>

#include <set>
#include <valarray>
#include <deque>
#include <utility>
#include <map>
#include <list>
using std::set;
using std::valarray;
using std::deque;
using std::pair;
using std::map;
using std::list;

#include "CReport.hh"
#include "CDumper.hh"


extern uint64 Random(uint64, uint64, uint64);

extern bool MatchDSTName(const string&);
extern bool IsStopTime();
class CDestination;
extern list<CDestination*> G_DST_LIST;

typedef enum PerformanceUnit { Bps, KBps, MBps, GBps};
typedef enum BiteSizeUnit {bit, B, KB, MB};
typedef enum TransferStatus {Starvation_NoPacket, 
			     Starvation_ReadAccessExhaution, 
			     Blocked_WriteAccessExhaustion,
			     Blocked_ChannelFull, 
			     Starvation_SourceNotReady,
			     Busy_InPacketDelay, 
			     Busy_InBiteDelay,
			     TransferOK, 
			     OKandDone, 
			     Inactive, 
			     AllDstDone, 
			     NothingToRun};

struct TransferStatusName
{
  inline static const string Map (TransferStatus id) {
    switch (id) {

    case Starvation_NoPacket: return "Starvation_NoPacket"; break;
    case Starvation_ReadAccessExhaution: return "Starvation_ReadAccessExhaution"; break;
 
    case Blocked_WriteAccessExhaustion: return "Blocked_WriteAccessExhaustion"; break;
    case Blocked_ChannelFull: return "Blocked_ChannelFull"; break;
    case Starvation_SourceNotReady: return "Starvation_SourceNotReady"; break;

    case Busy_InPacketDelay: return "Busy_InPacketDelay"; break;
    case Busy_InBiteDelay: return "Busy_InBiteDelay"; break;

    case TransferOK: return "TransferOK"; break;
    case OKandDone: return "OKandDone"; break;
    case Inactive: return "Inactive"; break;
    case AllDstDone: return "AllDstDone"; break;
    case NothingToRun: return "NothingToRun"; break;

    default: ERROR<<"Need to complete TransferStatusNameMap!! (" << id << ")\n"; exit(1);
    }
  }
};


typedef enum PipelineStatusEnum {PPL_UNPREPARED, 
				 PPL_PREPARED, 
				 PPL_RESUME, 
				 PPL_TRANSFER};

struct PipelineStatusName 
{
  inline static const string Map(PipelineStatusEnum id) {
    switch (id) {
    case PPL_UNPREPARED: return "PPL_UNPREPARED"; break; 
    case PPL_PREPARED: return "PPL_PREPARED"; break; 
    case PPL_RESUME: return "PPL_RESUME"; break; 
    case PPL_TRANSFER: return "PPL_TRANSFER"; break;
    default: 
      ERROR << "ERROR: what the fuck of PipelineStatus?? " << id << endl;
      exit(1);
    }
  }
};

typedef enum StreamingStatusEnum {PREPARE_PACKET_DELAY, 
				  WAIT_PACKET_DELAY, 
				  PREPARE_BITE_DELAY, 
				  WAIT_BITE_DELAY, 
				  STREAMING, 
				  FLUSH};

struct StreamingStatusName 
{
  inline static const string Map(StreamingStatusEnum id)
  {
    switch (id) {
    case PREPARE_PACKET_DELAY: return "PREPARE_PACKET_DELAY"; break; 
    case WAIT_PACKET_DELAY: return "WAIT_PACKET_DELAY"; break; 
    case PREPARE_BITE_DELAY: return "PREPARE_BITE_DELAY"; break; 
    case WAIT_BITE_DELAY: return "WAIT_BITE_DELAY"; break; 
    case STREAMING: return "STREAMING"; break; 
    case FLUSH: return "FLUSH"; break;      
    default:
      ERROR<< "ERROR: what the fuck of StreamingStatus?? " << id << endl;
      exit(1);
    }
  }
};



#include "CSource.hh"
class CTransHandler;

extern sc_clock G_CLOCK;

class CDestination
{
private:
  string _Name;

  CChannel *_DstCh;
  CTransHandler *_THPtr;

  double _Performance;
  uint64 _BiteSize;

  double _Cycle, _NextActivePoint, _NextPipelineUpdatePoint;

  bool _RandomPacketDelay;
  uint64 _PacketDelay, _PacketDelayMin, _PacketDelayMax, _PacketDelayStep;

  bool _RandomBiteDelay;
  uint64 _BiteDelay, _BiteDelayMin, _BiteDelayMax, _BiteDelayStep;

  deque<CFacet*> _PendingQueue;

  valarray<CBite*> _PipelineUnit;

  set<CSource*> _UsedSources;
  uint64 _CurrentOffset;

  StreamingStatusEnum _StreamState;
  PipelineStatusEnum  _PipelineState;

  double _TimePoint;

  CBite *_CurrentBite;

public:
  inline CDestination(const string &name, CChannel *ch, 
		      double value, 
		      PerformanceUnit unit, 
		      uint64 bitesize) 
  {
    _Name  = name;
    _DstCh = ch;

    SetPerformance(value, unit, bitesize);

    _RandomPacketDelay = false;
    _PacketDelay = 0;

    _RandomBiteDelay = false;
    _BiteDelay   = 0;

    _PipelineUnit.resize(1);

    _StreamState = PREPARE_PACKET_DELAY;
    _PipelineState = PPL_UNPREPARED;

    _CurrentBite = NULL;

    G_DST_LIST.push_back(this);
    
  }

public:
  const string Name() ;
  const string ChName() ;

  void UpdatePipeline() ;


  void SetPacketDelay(uint64 delay) ;
  void RandomPacketDelay(uint64 min, uint64 max, uint64 step) ;

  void SetBiteDelay(uint64 delay) ;
  void RandomBiteDelay(uint64 min, uint64 max, uint64 step) ;

  void SetPerformance(double value, PerformanceUnit unit, uint64 bitesize) ;
  void SetPipeline(uint64 pipeline ) ;

  void Fill(const vector<CFacet*> & facets) ;


  TransferStatus Transfer()  ;

  pair<bool, uint64> ReportReadStatus(CSource *source) ;

  void CreateSourceRecord(const vector<CSource*> & sources) ;
  void ResetOffet();



  inline bool IsPrepared() {
    return _PipelineState == PPL_PREPARED || _PipelineState == PPL_RESUME;
  }

  void RegisterTHPrt(CTransHandler *ptr) {_THPtr = ptr;}

  inline void SetToPrepare() {
    if (_PipelineState == PPL_TRANSFER ) 
      _PipelineState = PPL_UNPREPARED;
  }
  inline void SetToTransfer() {_PipelineState = PPL_TRANSFER;}
  inline void SetToResume() { _PipelineState = PPL_RESUME; }


  bool Lock(CTransHandler *writer ) ;
  bool Unlock(CTransHandler *writer);

private:
  bool PipelineEmpty() ;
  
  double GetPacketDelay() ;
  double GetBiteDelay() ;

  void DSTLog(const string &src_name, TransferStatus status) ;
  void PipelineLog(PipelineStatusEnum, const string & );

  
#ifdef __DEBUG_VERSION__
protected:
  int __name_flag;
  int __time_flag;

  inline void SetDebugFlag() {
    __name_flag = MatchDSTName(_Name);
    __time_flag = IsStopTime();

  }
#endif
};


#endif
