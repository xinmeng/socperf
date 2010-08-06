#include "CTransHandler.hh"


extern void STOP();
extern double G_TIME_TO_SECONDS;


// ------------------------------
//   CSimpleTH
// ------------------------------
TransferStatus
CSimpleTH::Execute() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH] " << Name() << endl;
  switch (_THState) {
    bool all_src_done, all_dst_done;

  case TH_IDLE:
    LOG << "_THState: Idle" << endl;
    if ( IsReady() ) {
      _THState = TH_READY;
    }
    else { 
      LOG << "no packet to process." << endl
	  << endl;
      return Starvation_NoPacket;
    }
      
  case TH_READY:
    LOG << "_THState: Ready to run" << endl;
    _THState = TH_BUSY;

  case TH_BUSY:
    LOG << "_THState: Busy" << endl;
    TransferStatus status;

    for ( unsigned int CommonIDX=0; CommonIDX < _DstList.size(); ++CommonIDX) {
      status = TransferLogicCone(CommonIDX);
      THLog(CommonIDX, status);
    }

    all_dst_done = true;
    for ( unsigned int i=0; i<_LogicConeState.size(); i++){
      if (_LogicConeState[i] != LC_DONE) {
	all_dst_done = false;
	break;
      }
    }
    all_src_done = Update(all_dst_done);
      

    if ( all_dst_done ) {
      if ( all_src_done ) {
	LOG << "TH is done and ready to start a new round." << endl;
	_THState = TH_IDLE;
	return AllDstDone;
      }
      else {
	WARNING << "**WARNING: All DST are done, but some SRC are not done." << endl
		<< "           Maybe in pipeline mode and source packet is longer the DST packet." << endl
		<< "           Now begin SRC flushing..." << endl;
	FlushSRC();
	_THState = TH_IDLE;
	return AllDstDone;
      }
    }
    else {
      LOG << "Some DST not done, need more time to transfer." << endl;
      return TransferOK;
    }
    
//   case TH_FLUSH:
//     all_src_done = true;
//     for (set<CSource*>::iterator src_iter = _SrcSet.begin(); 
// 	 src_iter != _SrcSet.end(); ++ src_iter) {
//       if ( ! (*src_iter)->Done ) {
// 	LOG << "Flushing SRC " << (*src_iter)->Name() << ", ";
// 	uint64 new_offset = (uint64)-1;

// 	if ( (*src_iter)->Update(new_offset, true) ) {
// 	  LOG << "done! ";
// 	  if ( (*src_iter)->Unlock(this) ) {
// 	    LOG << "SRC is done, and unlocked." << endl;
// 	  }
// 	  else {
// 	    ERROR << "Cannot unlock SRC " << (*src_iter)->Name() << ", maybe multiple unlock." << endl;
// 	    STOP();
// 	  }
// 	}
// 	else {
// 	  LOG << "need more flush." << endl;
// 	  all_src_done = false;
// 	}
//       }
//     }
    
//     if ( all_src_done ) {
//       _THState = TH_IDLE;
//       LOG << "TH is done and ready to start a new round." << endl;
//       return AllDstDone;
//     }
//     else {
//       return TransferOK;
//     }

  }
}


void
CSimpleTH::FlushSRC()
{
  for (set<CSource*>::iterator src_iter = _SrcSet.begin(); 
       src_iter != _SrcSet.end(); ++ src_iter) {
    if ( ! (*src_iter)->Done ) {
      LOG << "Flushing SRC " << (*src_iter)->Name() << ", ";
      uint64 new_offset = (uint64)-1;
      if ( (*src_iter)->Update(new_offset, true) ) {
	LOG << "done! ";
	if ( (*src_iter)->Unlock(this) ) {
	  LOG << "SRC is done, and unlocked." << endl;
	}
	else {
	  ERROR << "Cannot unlock SRC " << (*src_iter)->Name() << ", maybe multiple unlock." << endl;
	  STOP();
	}
      }
      else {
	ERROR << "SRC is too loooooooooooooong." << endl;
	exit(1);
      }
    }
  }
}



void
CSimpleTH::AddLogicCone(CSource *src, 
			CTransform *transform, 
			CDestination *dst) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  vector<CSource*> srclist;
  srclist.push_back(src);
  _SrcList.push_back(srclist);

  dst->CreateSourceRecord(srclist);
  _DstList.push_back(dst);

  dst->RegisterTHPrt(this);

  _TransformList.push_back(transform);
    
  _LogicConeState.push_back(LC_IDLE);
  _DstChLocked.push_back(false);
}


void
CSimpleTH::AddLogicCone(vector<CSource*> *src,
			CTransform *transform, 
			CDestination *dst)
{
}



bool
CSimpleTH::IsReady() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH " << Name() << "]" << endl;
  unsigned int CommonIDX;
  switch ( _THState ) {
  case TH_IDLE:
    Init();

    for ( CommonIDX=0; CommonIDX<_DstList.size(); CommonIDX++) {
      LOG << "-LogicCone No." << CommonIDX << endl;
	  
      bool all_src_ready = true;
      for ( vector<CSource*>::iterator src_it = _SrcList[CommonIDX].begin();
	    src_it != _SrcList[CommonIDX].end(); ++src_it ) {
	LOG << "   SRC " << (*src_it)->Name() << " ";

	if ( _SrcSet.count(*src_it) ) {
	  LOG << "already picked up." << endl;
	  continue;
	}
	else if ( (*src_it)->PickUpFacet(this) ) {
	  LOG << "is ready, packet picked!" << endl;
	  _SrcSet.insert( *src_it );
	}
	else {
	  // src log Starvation_ReadAccessExhaution
	  LOG << "has no packet." << endl;
	  all_src_ready = false;
	}
      }

      if ( !all_src_ready ) {
	return false;
      }
    }

    _THState = TH_READY;

    for ( set<CSource*>::iterator iter = _SrcSet.begin(); 
	  iter != _SrcSet.end(); ++iter) {
      (*iter)->ClaimFacet();
    }

    return true;

  case TH_READY: 	return true;
  case TH_BUSY:	return false;
  }
}

bool 
CSimpleTH::IsPrepared()
{
  bool prepared = true;
  for ( unsigned int i = 0 ; i < _DstList.size(); i++ ) {
    prepared &= _DstList[i]->IsPrepared(); 
  }

  return prepared;
}


void
CSimpleTH::Prepare()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH] " << Name() << " prepare..." << endl;
  switch (_THState) {
    bool all_src_done, all_dst_done;

  case TH_IDLE:
    LOG << "_THState: Idle" << endl;
    if ( IsReady() ) {
      _THState = TH_READY;
    }
    else { 
      LOG << "no packet to process." << endl
	  << endl;
      return ;
    }
      
  case TH_READY:
    LOG << "_THState: Ready to run" << endl;
    _THState = TH_BUSY;

  case TH_BUSY:
    LOG << "_THState: Busy" << endl;
    TransferStatus status;

    for ( unsigned int CommonIDX=0; CommonIDX < _DstList.size(); ++CommonIDX) {
      status = PrepareLogicCone(CommonIDX);
      THLog(CommonIDX, status);
    }


    all_dst_done = true;
    for ( unsigned int i=0; i<_LogicConeState.size(); i++){
      if (_LogicConeState[i] != LC_DONE) {
	all_dst_done = false;
	break;
      }
    }
    all_src_done = Update(all_dst_done);
      

    if ( all_dst_done ) {
      if ( all_src_done ) {
	LOG << "TH is done and ready to start a new round." << endl;
	_THState = TH_IDLE;
	return;
      }
      else {
	WARNING << "**WARNING: All DST are done, but some SRC are not done." << endl
		<< "           Maybe in pipeline mode and source packet is longer the DST packet." << endl
		<< "           Now begin SRC flushing..." << endl;
	FlushSRC();
	_THState = TH_IDLE;
	return;
      }
    }
    else {
      LOG << "Some DST not done, need more time to transfer." << endl;
      return ;
    }

    break;
    
//   case TH_FLUSH:
//     all_src_done = true;
//     for (set<CSource*>::iterator src_iter = _SrcSet.begin(); 
// 	 src_iter != _SrcSet.end(); ++ src_iter) {
//       if ( ! (*src_iter)->Done ) {
// 	LOG << "Flushing SRC " << (*src_iter)->Name() << ", ";
// 	uint64 new_offset = (uint64)-1;

// 	if ( (*src_iter)->Update(new_offset, true) ) {
// 	  LOG << "done! ";
// 	  if ( (*src_iter)->Unlock(this) ) {
// 	    LOG << "SRC is done, and unlocked." << endl;
// 	  }
// 	  else {
// 	    ERROR << "Cannot unlock SRC " << (*src_iter)->Name() << ", maybe multiple unlock." << endl;
// 	    STOP();
// 	  }
// 	}
// 	else {
// 	  LOG << "need more flush." << endl;
// 	  all_src_done = false;
// 	}
//       }
//     }
    
//     if ( all_src_done ) {
//       _THState = TH_IDLE;
//       LOG << "TH is done and ready to start a new round." << endl;
//       return AllDstDone;
//     }
//     else {
//       return TransferOK;
//     }

  }
}


TransferStatus
CSimpleTH::PrepareLogicCone(const unsigned int &CommonIDX)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  _DstList[CommonIDX]->SetToPrepare();
  return ProcessLogicCone(CommonIDX, "Prepare");

}


bool
CSimpleTH::Resume() 
{
  bool all_src_done, all_dst_done;
  
  TransferStatus status;
  for ( unsigned int CommonIDX=0; CommonIDX < _DstList.size(); ++CommonIDX) {
    status = ResumeLogicCone(CommonIDX);
    THLog(CommonIDX, status);
  }
    
  all_dst_done = true;
  for ( unsigned int i=0; i<_LogicConeState.size(); i++){
    if (_LogicConeState[i] != LC_DONE) {
      all_dst_done = false;
      break;
    }
  }
  all_src_done = Update(all_dst_done);
      

  if ( all_dst_done ) {
    if ( all_src_done ) {
      LOG << "TH is done and ready to start a new round." << endl;
      _THState = TH_IDLE;
      return true;
    }
    else {
      WARNING << "**WARNING: All DST are done, but some SRC are not done." << endl
	      << "           Maybe in pipeline mode and source packet is longer the DST packet." << endl
	      << "           Now begin SRC flushing..." << endl;
      FlushSRC();
      _THState = TH_IDLE;
      return true;
    }
  }
  else {
    LOG << "Some DST not done, need more time to transfer." << endl;
    return false;
  }
}


TransferStatus
CSimpleTH::ResumeLogicCone(const unsigned int &CommonIDX)
{
//   if ( _LogicConeState[CommonIDX] != STREAMING ) {
//     ERROR << "ERROR: Cannot resume on " << _LogicConeState[CommonIDX] << endl;
//     exit(1);
//   }

  _DstList[CommonIDX]->SetToResume();

  if ( _LogicConeState[CommonIDX] == LC_IDLE ) {
    return Inactive;
  }
  else {
    return ProcessLogicCone(CommonIDX, "Resume");
  }

}



void
CSimpleTH::Init() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  _SrcSet.clear();

  for ( unsigned int i=0; i<_DstList.size(); i++) {
    _LogicConeState[i] = LC_IDLE;
    _DstList[i]->ResetOffet();
  }
}


TransferStatus
CSimpleTH::TransferLogicCone(const unsigned int & CommonIDX)
{
  _DstList[CommonIDX]->SetToTransfer();
  
  return ProcessLogicCone(CommonIDX, "Transfer");
}

TransferStatus
CSimpleTH::ProcessLogicCone(const unsigned int& CommonIDX, const string &msg) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  TransferStatus status;
  bool all_src_locked = true;
    
  LOG << msg << " on DST " + _DstList[CommonIDX]->Name() << endl;


  string xfmname;
  switch (_LogicConeState[CommonIDX] ) {
  case LC_IDLE:
    _LogicConeState[CommonIDX] = LC_LOCK_SRC;

  case LC_LOCK_SRC:
    for ( vector<CSource*>::iterator iter = _SrcList[CommonIDX].begin();
	  iter != _SrcList[CommonIDX].end(); ++iter) {
      LOG << "   SRC " << (*iter)->Name() << " ";
      if ( !(*iter)->Lock(this) ) {
	// TH log
	WARNING << "read ports exhausted!" << endl;
	all_src_locked = false;
	break;
      }
      else {
	LOG << " locked." << endl;
      }
    }

    if ( all_src_locked ) {
      LOG << "  --All sources locked--" << endl;
      _LogicConeState[CommonIDX] = LC_TRANSFORM;
    }
    else {
      WARNING << "**Source resource exhausted, transfer can not start!**" << endl;
      return Starvation_ReadAccessExhaution;
    }


//   case LOCK_DST:
//     LOG << "   DST " << _DstList[CommonIDX]->Name() << " ";
//     if ( _DstList[CommonIDX]->Lock(this) ) {
//       LOG << "locked." << endl;
//       _LogicConeState[CommonIDX] = TRANSFORM;
//     }
//     else {
//       WARNING << "write access exhausted!" << endl;
//       return Blocked_WriteAccessExhaustion;
//     }

  case LC_TRANSFORM:
    xfmname = _TransformList[CommonIDX]->Name();
    // ConnLog
    LOG << "   Execute TRANFORM " << xfmname << ", got " << ExecTransform(CommonIDX) << " facets." << endl;
    _LogicConeState[CommonIDX] = LC_STREAMING;

  case STREAMING:
    LOG << "   Transfering facets...";
    status = _DstList[CommonIDX]->Transfer();
	
    // TH log

    switch(status) {
    case Starvation_SourceNotReady:
    case Blocked_ChannelFull: 
      WARNING << TransferStatusName::Map(status) << endl;
      break;

    case OKandDone: 
      _LogicConeState[CommonIDX] = LC_DONE;
//       if (!_DstList[CommonIDX]->Unlock(this) ) {
// 	ERROR << "Cannot unlock DST " << _DstList[CommonIDX]->Name() 
// 	      << ", maybe multipule unlock." << endl;
// 	STOP();
//       }
    case Busy_InPacketDelay: 
    case Busy_InBiteDelay:
    case TransferOK: 
    case Inactive: 
      LOG << TransferStatusName::Map(status) << endl;
      break;
	  
    default:
      ERROR << "What the hell is the status?!" << endl;
      STOP();

    }
    return status;

  case LC_DONE:
    LOG << "transfer is already finished." << endl;
    return OKandDone;
  }
}


int
CSimpleTH::ExecTransform(int CommonIDX) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  vector<CPacket*> packets = _TransformList[CommonIDX]->ApplyTransform(_SrcList[CommonIDX]);
  CFacet *mainfacet = _SrcList[CommonIDX][0]->CurrentFacet;
    
  vector<CFacet*> facets;
  for ( vector<CPacket*>::iterator iter = packets.begin();
	iter != packets.end(); ++iter) {
    (*iter)->CreateTimeS = G_TIME_TO_SECONDS;
    (*iter)->CreateTimeE = std::numeric_limits<double>::max();

    PFG_vertex v = mainfacet->Transaction->AddVertex( *iter );
    facets.push_back( new CFacet (mainfacet->Transaction, v) );
      
    for ( vector<CSource*>::iterator src_iter = _SrcList[CommonIDX].begin();
	  src_iter != _SrcList[CommonIDX].end(); ++src_iter) {
      (*src_iter)->CurrentFacet->ToVertex(v);
    }
  }

  _DstList[CommonIDX]->Fill( facets );

  return facets.size();
}
  
bool
CSimpleTH::Update(bool all_dst_done) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  bool all_src_done = true;

  for (set<CSource*>::iterator src_iter = _SrcSet.begin(); 
       src_iter != _SrcSet.end(); ++ src_iter) {
    LOG << "Update SRC " << (*src_iter)->Name() << ", ";
    if ( ! (*src_iter)->Done ) {
      uint64 current_offset = (*((*src_iter)->CurrentFacet))->Offset;
      uint64 max_offset     = (*((*src_iter)->CurrentFacet))->ChTransferredSize;
      uint64 new_offset = max_offset;

      for ( unsigned int i=0; i<_DstList.size(); ++i) {
	pair<bool, uint64> src_status = _DstList[i]->ReportReadStatus( *src_iter );
	new_offset = (src_status.first && new_offset > src_status.second) ? 
	  src_status.second : new_offset;
      }

      LOG << "with (" << new_offset << "," << all_dst_done << ")...";
      if ( (*src_iter)->Update(new_offset, all_dst_done) ) {
	if ( (*src_iter)->Unlock(this) ) {
	  LOG << "SRC is done, and unlocked." << endl;
	}
	else {
	  ERROR << "Cannot unlock SRC " << (*src_iter)->Name() << ", maybe multiple unlock." << endl;
	  STOP();
	}
      }
      else {
	all_src_done = false;
	LOG << "is not done, can not unlock." << endl;
      }
    }
    else {
      LOG << "is already done." << endl;
    }
  }
  return all_src_done;
}


void
CSimpleTH::THLog(int idx, TransferStatus status ) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  if ( status != Inactive ) {

    DBDumper.THLog(G_TIME_TO_SECONDS,
		   Name(), 
		   _TransformList[idx]->Name(),
		   _DstList[idx]->Name(),
		   TransferStatusName::Map(status) );
  }
}

// void
// CSimpleTH::ConnLog() 
// {
// #ifdef __DEBUG_VERSION__
//   SetDebugFlag();
// #endif


//   for ( unsigned int i=0; i<_TransformList.size(); i++) {
//     string srclist;
//     for ( vector<CSource*>::iterator src = _SrcList[i].begin();
// 	  src != _SrcList[i].end(); ++src) {
//       DBDumper.ConnectionLog(G_TIME_TO_SECONDS, 
// 			     Name(), 
// 			     _DstList[i]->Name(), 
// 			     _DstList[i]->ChName(),
// 			     _TransformList[i]->Name(), 
// 			     (*src)->Name(),
// 			     (*src)->ChName(),
// 			     (*src)->CondName()  );
//     }
//   }
// }





// ------------------------------
//   CRandomTH
// ------------------------------
void 
CRandomTH::AddTH(CTransHandler *th, uint64 weight)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  if ( _THList.size() == 0 ) {
    _Weight.push_back(weight);
  }
  else {
    _Weight.push_back( _Weight[_Weight.size()-1] + weight );
  }

  _THList.push_back(th);
}


const string
CRandomTH::Name()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  string name;
  name = name + "_Rand_" + "\"" + _Name + "\"" + "_(";
  for ( unsigned int i=0; i<_THList.size(); ++i) {
    name = name + _THList[i]->Name() + ", ";
  }
  name += ")";

  return name;
}


bool
CRandomTH::IsReady()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if (!_CurrentTH) {
    SelectTH();
  }
  
  if (_CurrentTH) {
    return _CurrentTH->IsReady();
  }
  else {
    return false;
  }
}


void
CRandomTH::Prepare()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  if ( !_CurrentTH) {
    SelectTH();
  }

  _CurrentTH->Prepare();


}


bool
CRandomTH::IsPrepared()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  if ( !_CurrentTH ) {
    SelectTH();
  }
  
  return _CurrentTH->IsPrepared();
}


bool
CRandomTH::Resume()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  if ( !_CurrentTH ) {
    ERROR << "ERROR: Call CRandomTH::Resume without Prepare." << endl;
    exit(1);
  }

  if ( _CurrentTH->Resume() ) {
    _CurrentTH = NULL;
    return true;
  }
  else {
    return false;
  }

}

TransferStatus
CRandomTH::Execute()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  LOG << "[TH] " << Name() << endl;
  if ( !_CurrentTH ) {
    SelectTH();
  }

  TransferStatus status = _CurrentTH->Execute();

  if ( status != Inactive ) {
    DBDumper.THLog(G_TIME_TO_SECONDS, 
		   Name(), "", _CurrentTH->Name(), 
		   TransferStatusName::Map(status));
  }

  _CurrentTH = ( status == AllDstDone || 
		 status == Starvation_NoPacket) ? NULL : _CurrentTH;
  return status;

}


void
CRandomTH::SelectTH()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  _CurrentTH = NULL;

  _CurrentTH = _THList[ WeightedRandom(_Weight) ];  
}




// ------------------------------
//    CRoundRobinTH
// ------------------------------
void
CRoundRobinTH::AddTH(CTransHandler *th, uint64 weight) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  _THList.push_back(th);
}

const string 
CRoundRobinTH::Name()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  string name;
  name = name + "_RR_" + "\"" + _Name + "\"" + "_(" ;

  for ( unsigned int i=0; i<_THList.size(); ++i) 
    name = name + _THList[i]->Name() + ", ";
  
  name += ")";

  return name;
}


bool
CRoundRobinTH::IsReady()
{
  bool ready; 
  LOG << "[TH-RR] " << Name() << " is ready..."; 

  ready = IsPrepared();

  LOG << ready << endl;

  return ready;


//   if ( !_CurrentTH ) {
//     SelectTH();
//   }

//   if ( _CurrentTH ) {
//     return _CurrentTH->IsReady();
//   }
//   else {
//     return false;
//   }
}


bool 
CRoundRobinTH::IsPrepared()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH-RR] " << Name() << " is prepared...";

  bool ready = false;

  for (unsigned int i=0; i<_THList.size(); ++i) {
    ready |= _THList[i]->IsPrepared();
  }
 
  LOG << ready << endl;
  return ready;

}


void
CRoundRobinTH::Prepare()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH-RR] " << Name() << " preparing..." << endl;

  for (unsigned int i=0; i<_THList.size(); ++i) {
    _THList[i]->Prepare();
  }
 
}


bool
CRoundRobinTH::Resume()
{
  LOG << "[TH-RR] " << Name() << " resuming..." << endl;

  TransferStatus status = Execute();
  if ( status == AllDstDone ) {
    LOG << "resume done." << endl;
    return true;
  }
  else {
    return false;
  }
}

TransferStatus
CRoundRobinTH::Execute()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH-RR] " << Name() << " executing..." << endl;

  LOG << "  Prepare all other containted TH..." << endl;
  for (unsigned int i=(_RRposition+1)%_THList.size(); 
       i != _RRposition; i = (i+1)%_THList.size()) {
    _THList[i]->Prepare();
  }

  LOG << "  On current (" << _RRposition << ") TH..." << endl;
  if (_THList[_RRposition]->IsPrepared() ) {
    LOG << "resume..." << endl;
    _THList[_RRposition]->Resume();
    return TransferOK;
  }
  else {
    LOG << "prepare and swith to next TH..." << endl;
    _THList[_RRposition]->Prepare();
    _RRposition = (_RRposition+1)%_THList.size();
    return AllDstDone;
  }


//   if ( !_CurrentTH ) {
//     SelectTH();
//   }
  
//   if ( !_CurrentTH ) {
//     DBDumper.THLog(G_TIME_TO_SECONDS, 
// 		   Name(), "", "", 
// 		   TransferStatusName::Map(NothingToRun));

//     return NothingToRun;
//   }
//   else {
//     TransferStatus status = _CurrentTH->Execute();

//     if ( status != Inactive ) {
//       DBDumper.THLog(G_TIME_TO_SECONDS, 
// 		     Name(), "", _CurrentTH->Name(), 
// 		     TransferStatusName::Map(status));
//     }

//     _CurrentTH = ( status == AllDstDone) ? NULL : _CurrentTH;
//     return status;
//   }
}

void
CRoundRobinTH::SelectTH() 
{
// #ifdef __DEBUG_VERSION__
//   SetDebugFlag();
// #endif


//   for (unsigned int i=(_RRposition+1)%_THList.size(); 
//        i <= (_RRposition-1)%_THList.size(); ++i) {
//     _THList[i]->Prepare();
//   }

//   if (_THList[_RRposition]->IsPrepared() ) {
//     _THList[_RRposition]->Resume();
//   }
//   else {
//     _THList[_RRposition]->Prepare();
//     _RRposition = (_RRposition+1)%_THList.size();
//   }
}


// ------------------------------
//    CArbiterTH
// ------------------------------
void
CArbiterTH::AddTH(CTransHandler *th, uint64 weight) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  _THList.push_back(th);
  if ( _Weight.size() == 0  ) {
    _Weight.push_back(weight);
  }
  else {
    _Weight.push_back(_Weight[_Weight.size()-1] + weight );
  }
}


const string
CArbiterTH::Name()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  string name ;

  name = name + "_Arb_" + "\"" + _Name + "\"" + "_(";
  for ( unsigned int i=0; i<_THList.size(); ++i) {
    name = name + _THList[i]->Name() + ", " ;
  }
  name += ")";

  return name;
  
}


bool
CArbiterTH::IsReady()
{
  return IsPrepared();

//   if (!_CurrentTH) {
//     SelectTH();
//   }

//   if ( _CurrentTH ) {
//     return _CurrentTH->IsReady();
//   }
//   else {
//     return false;
//   }
}


bool
CArbiterTH::IsPrepared()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  bool ready = false;

  for (unsigned int i=0; i<_THList.size(); ++i) {
    ready |= _THList[i]->IsPrepared();
  }
 
  return ready;

}


void 
CArbiterTH::Prepare()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  for (unsigned int i=0; i<_THList.size(); ++i) {
    _THList[i]->Prepare();
  }
}


bool
CArbiterTH::Resume()
{
  TransferStatus status = Execute();
  if ( status == AllDstDone ) {
    return true;
  }
  else {
    return false;
  }
}



TransferStatus
CArbiterTH::Execute()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH] " << Name() << endl;  

  for (unsigned int i=(_ArbRecord+1)%_THList.size(); 
       i != _ArbRecord; i = (i+1)%_THList.size()) {
    _THList[i]->Prepare();
  }

  if (_THList[_ArbRecord]->IsPrepared() ) {
    _THList[_ArbRecord]->Resume();
    return TransferOK;
  }
  else {
    _THList[_ArbRecord]->Prepare();

    ++_Selector;
    if ( _Selector >= _Weight[_ArbRecord] ) {
      _ArbRecord = (_ArbRecord+1)%_THList.size();
    }
    _Selector = _Selector % _Weight[_Weight.size()-1];

    return AllDstDone;
  }


//   if ( !_CurrentTH ) {
//     SelectTH();
//   }
  
//   if ( !_CurrentTH ) {
//     DBDumper.THLog(G_TIME_TO_SECONDS, 
// 		   Name(), "", "", 
// 		   TransferStatusName::Map(NothingToRun));

//     return NothingToRun;
//   }
//   else {
//     TransferStatus status = _CurrentTH->Execute();

//     if ( status != Inactive ) {
//       DBDumper.THLog(G_TIME_TO_SECONDS, 
// 		     Name(), "", _CurrentTH->Name(), 
// 		     TransferStatusName::Map(status));
//     }

//     _CurrentTH = ( status == AllDstDone) ? NULL : _CurrentTH;
//     return status;
//   }

}


void
CArbiterTH::SelectTH() 
{
// #ifdef __DEBUG_VERSION__
//   SetDebugFlag();
// #endif


//   uint64 max_weight=0;
//   int    selected_th=-1;

//   for ( unsigned int i=0; i<_THList.size(); i++) 
//     if ( _THList[i]->IsReady() ) {
//       if ( _Weight[i] > max_weight ) {
// 	max_weight  = _Weight[i];
// 	selected_th = i;
//       }
//     }

//   if ( selected_th == -1 ) {
//     _CurrentTH = NULL;
//   }
//   else {
//     _CurrentTH = _THList[selected_th];
//   }
}






// ------------------------------
//   CDispatchTH
// ------------------------------
void 
CDispatchTH::AddTH(CTransHandler *th, uint64 weight)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _THList.push_back(th);
}


const string
CDispatchTH::Name()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  string name;

  name = name + "_Disp_" + "\"" + _Name + "\"" + "_(";
  for (unsigned int i=0; i<_THList.size(); ++i) 
    name = name + _THList[i]->Name() + ", ";
  name += ")";

  return name;
}

bool
CDispatchTH::IsReady()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  SelectTH();

  if ( _ReadyTHs.size() > 0 ) {
    return true;
  }
  else {
    return false;
  }
}

TransferStatus
CDispatchTH::Execute()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  LOG << "[TH] " << Name() << endl;
  SelectTH();

  TransferStatus status;
  for (set<CTransHandler*>::iterator iter = _ReadyTHs.begin();
       iter != _ReadyTHs.end(); ++iter) {
    status = (*iter)->Execute();
    if ( status != Inactive ) {
      DBDumper.THLog(G_TIME_TO_SECONDS, 
		     Name(), "", (*iter)->Name(),
		     TransferStatusName::Map(status));
    }
    if (status == AllDstDone) {
      _ReadyTHs.erase(iter);
    }
  }

  return status;
}


void
CDispatchTH::SelectTH() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  for ( unsigned int i=0; i<_THList.size(); ++i) {
    if ( _ReadyTHs.count(_THList[i]) == 0 && 
	 _THList[i]->IsReady() ) {
      _ReadyTHs.insert(_THList[i]);
    }
  }
}
