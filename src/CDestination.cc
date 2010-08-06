#include "CDestination.hh"

extern void STOP();
extern sc_time G_CURRENT_TIME;
extern double G_TIME_TO_SECONDS;


const string
CDestination::Name() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  

  return "\"" + _Name + "\"";
}

const string 
CDestination::ChName() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  

  return _DstCh->Name();
}

void
CDestination::SetPacketDelay(uint64 delay)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _RandomPacketDelay = false;
  _PacketDelay = delay;
}

void
CDestination::RandomPacketDelay(uint64 min, uint64 max, uint64 step)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _RandomPacketDelay = true;
  _PacketDelayMin  = min; 
  _PacketDelayMax  = max;
  _PacketDelayStep = step;
}

void
CDestination::SetBiteDelay(uint64 delay)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _RandomBiteDelay = false;
  _BiteDelay = delay;
}

void
CDestination::RandomBiteDelay(uint64 min, uint64 max, uint64 step)
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _RandomBiteDelay = true;
  _BiteDelayMin  = min;
  _BiteDelayMax  = max;
  _BiteDelayStep = step;
}

void
CDestination::SetPerformance(double value, PerformanceUnit unit, 
			     uint64 bitesize) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _BiteSize = bitesize;
  switch (unit) {
  case Bps: 
    _Performance = value;
    break;
      
  case KBps: 
    _Performance = value * 1e+3;
    break;

  case MBps:
    _Performance = value * 1e+6;
    break;
      
  case GBps:
    _Performance = value * 1e+9;
    break;

  default: 
    LOG << "Illegal performance uint!" << endl;
  }

  _Cycle = _BiteSize / _Performance;
  LOG << "Cycle of " + Name() + " is " << _Cycle << endl;

  _NextActivePoint = _Cycle;
  _NextPipelineUpdatePoint = _Cycle;
    
}

void
CDestination::SetPipeline(uint64 pipeline ) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( pipeline == 0 ) {
    ERROR << "Zero pipeline?! are you sure?" << endl;
    STOP();
  }
  _PipelineUnit.resize(pipeline);
}

void
CDestination::Fill(const vector<CFacet*> & facets) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _PendingQueue.insert( _PendingQueue.end(), 
			facets.begin(), facets.end() );
}


TransferStatus
CDestination::Transfer() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  _PipelineState = PPL_TRANSFER;

  
  if ( G_TIME_TO_SECONDS < _NextActivePoint ) {
    LOG << "not active utill " << _NextActivePoint << " sec, (" << _Cycle << ")";
    //DSTLog("--", Inactive);
    return Inactive;
  }
  else {
    double diff = G_TIME_TO_SECONDS - _NextActivePoint ;
    double diff_count = diff / _Cycle;
    int diff_int = (int)  diff_count;
    _NextActivePoint += _Cycle * (diff_int + 1);

    CFacet *facet_to_transfer = _PendingQueue.front();

    if (!facet_to_transfer) return Inactive;

    CBite *top_bite;
    bool all_src_data_rdy = true;

    switch ( _StreamState ) {
    case PREPARE_PACKET_DELAY:
      _TimePoint = G_TIME_TO_SECONDS;
      _TimePoint = _TimePoint + GetPacketDelay() + (*facet_to_transfer)->ExtraDelay / 1e+9;

      _StreamState = WAIT_PACKET_DELAY;

    case WAIT_PACKET_DELAY:
      if ( _TimePoint > G_TIME_TO_SECONDS ) {
	LOG << "in packet delay till "<< _TimePoint;
	DSTLog("--", Busy_InPacketDelay);

	return Busy_InPacketDelay;
      }
      else {
	_StreamState = PREPARE_BITE_DELAY;
      }

    case PREPARE_BITE_DELAY:
      _TimePoint = G_TIME_TO_SECONDS;
      _TimePoint = _TimePoint + GetBiteDelay();
      _StreamState = WAIT_BITE_DELAY;

    case WAIT_BITE_DELAY:
      if ( _TimePoint > G_TIME_TO_SECONDS ) {
	LOG << "in bite delay till " << _TimePoint;
	DSTLog("--", Busy_InBiteDelay);
	return Busy_InBiteDelay;
      }
      else 
	_StreamState = STREAMING;
	

    case STREAMING:
      if ( _CurrentBite ) {
	//	if ( !_DstCh->SpaceAvailable(_BiteSize) ) {
	// dst log
	DSTLog("--", Blocked_ChannelFull);
	return Blocked_ChannelFull;
      }

      uint64 current_bite_size;
      bool is_last_bite;
      if ( (*facet_to_transfer)->DstTransferredSize + _BiteSize >= (*facet_to_transfer)->Size ) {
	current_bite_size = (*facet_to_transfer)->Size - (*facet_to_transfer)->DstTransferredSize;
	is_last_bite = true;
      }
      else {
	current_bite_size = _BiteSize;
	is_last_bite = false;
      }
      
      for (set<CSource*>::iterator iter=_UsedSources.begin(); 
	   iter != _UsedSources.end(); ++iter) {
	if ( ! (*iter)->DataAvailable(_CurrentOffset + current_bite_size)) {
	  // dst log
	  WARNING << (*iter)->Name() << " not ready for " << current_bite_size << " Bytes at " << _CurrentOffset << "!";
	  DSTLog((*iter)->Name(), Starvation_SourceNotReady);
	  return Starvation_SourceNotReady;
	}
      }

      _CurrentOffset += current_bite_size;

      _CurrentBite = new CBite ( facet_to_transfer, current_bite_size, is_last_bite);
      (*facet_to_transfer)->DstTransferredSize += current_bite_size;

      if ( is_last_bite ) {
	(*facet_to_transfer)->CreateTimeE = G_TIME_TO_SECONDS;

	_PendingQueue.pop_front();
	_StreamState = PREPARE_PACKET_DELAY;
	if ( _PendingQueue.size() == 0 ) {
	  DSTLog("--", OKandDone);
	  return OKandDone;
	}
	else {
	  DSTLog("--", TransferOK);
	  return TransferOK;
	}
      }
      else {
	_StreamState = PREPARE_BITE_DELAY;
	DSTLog("--", TransferOK);
	return TransferOK;
      }


//       top_bite = _PipelineUnit[_PipelineUnit.size() -1 ] ;
//       LOG << "push new bite into pipeline,";
//       if ( top_bite ) {
// 	LOG << "pop previous bite out.";
// 	_DstCh->Write(top_bite);
//       }
//       else {
// 	LOG << "empty bite in current stage.";
//       }
//       _PipelineUnit = _PipelineUnit.shift(-1);
//       _PipelineUnit[0] = current_bite;

//       DSTLog("--", TransferOK);



//     case FLUSH:
//       top_bite = _PipelineUnit[_PipelineUnit.size() -1 ] ;
	  
//       LOG << "flushing pipeline, ";
//       if ( !_DstCh->SpaceAvailable(_BiteSize) ) {
// 	// dst log
// 	WARNING << "insufficient space for " << _BiteSize << " Bytes in channel \"" << _DstCh->Name() << "\"!" << endl;
// 	DSTLog("--", Blocked_ChannelFull);
// 	return Blocked_ChannelFull;
//       }

//       if ( top_bite ) {
// 	LOG << "pop previous bite out, ";
// 	_DstCh->Write(top_bite);
//       }
//       else {
// 	LOG << "empty bite in current stage, ";
//       }

//       _PipelineUnit = _PipelineUnit.shift(-1);

//       if ( PipelineEmpty() ) {
// 	LOG << "flush done!";
// 	_StreamState = PREPARE_PACKET_DELAY;
// 	DSTLog("--", OKandDone);
// 	return OKandDone;
//       }
//       else {
// 	LOG << "need more flushing.";
// 	DSTLog("--", TransferOK);
// 	return TransferOK;
//       }
    }

  }
}

void
CDestination::UpdatePipeline()
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  switch (_PipelineState) {
  case PPL_TRANSFER:
  case PPL_RESUME:
  case PPL_UNPREPARED:
    if ( G_TIME_TO_SECONDS < _NextPipelineUpdatePoint ) {
      //PipelineLog(_PipelineState, "Inactive");
      //LOG << "Not update pipeline utill " << _NextPipelineUpdatePoint << " sec, (" << _Cycle << ")";
      return;
    }
    else {
      double diff = G_TIME_TO_SECONDS - _NextPipelineUpdatePoint ;
      double diff_count =  diff / _Cycle;
      int diff_int = (int)  diff_count;
      _NextPipelineUpdatePoint += _Cycle * (diff_int + 1);


      CBite *top_bite = _PipelineUnit[_PipelineUnit.size() -1 ] ;
	  
      LOG << "  " << Name() << " ";

      if ( top_bite ) {
	if ( ! Lock(_THPtr) ) {
	  WARNING << "write access exhausted!" << endl;
	  PipelineLog(_PipelineState, "write access exhausted");
	  return ;
	}

	if ( !_DstCh->SpaceAvailable(top_bite->Size) ) {
	  WARNING << "insufficient space for " << top_bite->Size << " Bytes in channel \"" << _DstCh->Name() << "\", halted." << endl;
	  PipelineLog(_PipelineState, "insufficient space");
	  return ;
	}

	_DstCh->Write(top_bite);
	LOG << top_bite->Size << " popped." << endl;
	PipelineLog(_PipelineState, "popped");

	if ( top_bite->Last ) {
	  if ( _PipelineState == PPL_RESUME ) {
	    _PipelineState = PPL_UNPREPARED;
	  }
	  else if ( _PipelineState == PPL_TRANSFER ) {
	    _PipelineState = PPL_TRANSFER;
	  }
	  else {
	    WARNING << "Warning: Encounter last beat in PPL_UNPREPARED!" << endl;
	    _PipelineState = PPL_UNPREPARED;
	  }

	  if (! Unlock(_THPtr) ) {
	    ERROR << "Cannot unlock DST " << Name() 
		  << ", maybe multipule unlock." << endl;
	    STOP();
	  }
	}
      }
      else {
	LOG << "emtpy beat." << endl;
	//PipelineLog(_PipelineState, "empty beat");
      }

      _PipelineUnit = _PipelineUnit.shift(-1);
      if ( _CurrentBite ) {
	_PipelineUnit[0] = _CurrentBite;
	_CurrentBite     = NULL;
      }

      top_bite = _PipelineUnit[_PipelineUnit.size() -1];
      if ( top_bite  && _PipelineState == PPL_UNPREPARED ) {
	_PipelineState = PPL_PREPARED;
      }
    }

    break;

  case PPL_PREPARED:
    // halt pipeline
    PipelineLog(_PipelineState, "halted");
    break;

  default:
    ERROR << "Buggy Pipeline state." << endl;
    exit(1);
  }
}


pair<bool, uint64>
CDestination::ReportReadStatus(CSource *source) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _UsedSources.count(source) ) 
    return pair<bool, uint64> (true, _CurrentOffset);
  else 
    return pair<bool, uint64> (false, 0);
}

void
CDestination::CreateSourceRecord(const vector<CSource*> & sources) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _UsedSources.insert(sources.begin(), sources.end());
}


void
CDestination::ResetOffet()
{
  _CurrentOffset = 0;
}


bool
CDestination::Lock(CTransHandler *writer ) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  return _DstCh->LockWr(writer);
}

bool
CDestination::Unlock(CTransHandler *writer) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  return _DstCh->UnlockWr(writer);
}

bool
CDestination::PipelineEmpty() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  for ( int i=0; i<_PipelineUnit.size(); i++) 
    if (_PipelineUnit[i] != 0) return false;
  return true;
}
  
double
CDestination::GetPacketDelay() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _RandomPacketDelay ) 
    return (double) Random(_PacketDelayMin, _PacketDelayMax, _PacketDelayStep) / 1e+9;
  else 
    return (double) _PacketDelay / 1e+9;
}

double
CDestination::GetBiteDelay() 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _RandomBiteDelay ) 
    return (double) Random(_BiteDelayMin, _BiteDelayMax, _BiteDelayStep) / 1e+9;
  else 
    return (double) _BiteDelay / 1e+9;
}

void
CDestination::DSTLog(const string &src_name,
		     TransferStatus status) 
{
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif


  DBDumper.DSTLog(G_TIME_TO_SECONDS,
		  Name(), 
		  src_name, 
		  TransferStatusName::Map(status));
}


void 
CDestination::PipelineLog(PipelineStatusEnum id, const string &msg)
{
  DBDumper.PipelineLog(G_TIME_TO_SECONDS, 
		       Name(),
		       PipelineStatusName::Map(id),
		       msg );
}

