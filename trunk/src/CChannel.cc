#include "CChannel.hh"

extern void STOP();
extern double G_TIME_TO_SECONDS;

// ------------------------------
//   CSimpleChannel
// ------------------------------
void 
CSimpleChannel::Fill( vector<CFacet*> facets ) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  for (unsigned int i=0; i<facets.size(); i++) {
    _Content.push_back( facets[i] );
    _Occupied += (*(facets[i]))->Size;
    if ( _Occupied > _Capacitance && _Finite ) {
      WARNING << Name() << " overflow via back door write, " << _Occupied << ">"  << _Capacitance << endl;
    }
  }
}

bool 
CSimpleChannel::LockRd(CTransHandler *reader) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _CurrentReader.count(reader) > 0 || _CurrentReader.size() < _ReaderCount ) {
    _CurrentReader.insert(reader);
    return true;
  }
  else {
    return false;
  }
}

bool 
CSimpleChannel::UnlockRd(CTransHandler *reader) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _CurrentReader.count(reader) > 0 ) {
    _CurrentReader.erase(reader);
    return true;
  }
  else {
    return false;
  }
}

bool 
CSimpleChannel::LockWr(CTransHandler *writer) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _CurrentWriter.count(writer) > 0 || _CurrentWriter.size() < _WriterCount ) {
    _CurrentWriter.insert(writer);
    return true;
  }
  else {
    return false;
  }
}

bool 
CSimpleChannel::UnlockWr(CTransHandler *writer) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _CurrentWriter.count(writer) > 0 ) {
    _CurrentWriter.erase(writer);
    return true;
  }
  else {
    return false;
  }
}


// Common method 
float 
CSimpleChannel::WaterMark( ) { 
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _Finite ) {
    return _Occupied / _Capacitance * 1.0; 
  }
  else {
    return 0;
  }
}


bool 
CSimpleChannel::SpaceAvailable( uint64 bite_size ) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  return !_Finite ||  _Capacitance - _Occupied >= bite_size; 
}


CFacet* 
CSimpleChannel::PickUpFacet(CFacetCondition *condition) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( _Content.size() == 0 ) {
    return NULL;
  }

  list<CFacet*>::iterator iter = _Content.begin();
  if ( (*condition) (*iter) ) {
    return *iter;
  }
  else if ( _Ordered ) {
    return NULL;
  }
  else {
    for ( ++iter; iter != _Content.end(); ++iter ) {
      if ( (*condition) ( *iter ) ) return *iter;
    }
    return NULL;
  }

}

CFacet* 
CSimpleChannel::PickUpFacet(CChannelCondition *condition) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( (*condition) (WaterMark()) ) {
    stringstream ss;
    ss << WaterMark();
    CPacket *pkt  = new CPacket ( ss.str(), 0, 0, 0 );
    PFG_vertex v = G_TRANS.AddVertex( pkt );
    CFacet *facet = new CFacet( &G_TRANS, v );

    return facet;
  }
  else {
    ERROR << "Cannot pick up packet with matched condition." << endl;
    STOP();
  }
}


void 
CSimpleChannel::Write(CBite *bite) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  bool exist = false;
  for ( list<CFacet*>::iterator iter = _Content.begin(); iter != _Content.end(); ++ iter) {
    if ( *iter == bite->Facet ) {
      exist = true;
      break;
    }
  }
  if ( !exist ) {
    (*(bite->Facet))->StoreTimeS = G_TIME_TO_SECONDS;
    _Content.push_back( bite->Facet );
  }

  if ( bite->Last ) {
    (*(bite->Facet))->StoreTimeE = G_TIME_TO_SECONDS;
  }

  (*(bite->Facet))->ChTransferredSize += bite->Size;
  _Occupied += bite->Size;

  _PacketFlow[(*(bite->Facet))->Name] += bite->Size;

  request_update();
}

void 
CSimpleChannel::Pop(CFacet *facet) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  _Occupied -= ((*facet)->Size - (*facet)->Offset);
  _Content.remove( facet );
  request_update();
}
  
bool 
CSimpleChannel::Read(CFacet *facet, uint64 new_offset) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  uint64 size;

  if ( new_offset >= (*facet)->ChTransferredSize) {
    size = (*facet)->ChTransferredSize - (*facet)->Offset;
    (*facet)->Offset = (*facet)->ChTransferredSize;
  }
  else {
    size = new_offset - (*facet)->Offset;
    (*facet)->Offset = new_offset;
  }

  _Occupied -= size;

  request_update();
    
  return (*facet)->Offset == (*facet)->Size;
}

void 
CSimpleChannel::update() {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  // log channel status
  DBDumper.CHLog(G_TIME_TO_SECONDS, Name(), 
		 _Content.size(), _Occupied,  _Capacitance);

  // log packet flow
  for ( map<string, double>::iterator iter = _PacketFlow.begin();
	iter != _PacketFlow.end(); ++iter ) {
    DBDumper.PktFlowLog(G_TIME_TO_SECONDS, Name(),
			iter->first, iter->second);
  }
}



// ------------------------------
//   CAssembleChannel
// ------------------------------
CFacet*
CAssembleChannel::MergeFacets( const set<CFacet*> & facet_set ) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  CFacet *first_facet = *(facet_set.begin());
  CPMTransaction *transaction = first_facet->Transaction;
  SplitStatus *top = (*first_facet)->SplitStack.top(); 

  if ( top->TotalCount == facet_set.size() ) {
    bool all_fragement_completed = true;

    uint64 total_size=0, total_length=0;
    double MinCreateTimeS=std::numeric_limits<double>::max(), MaxCreateTimeE=-1;
    double MinStoreTimeS=std::numeric_limits<double>::max(), MaxStoreTimeE=-1;
      
    for ( set<CFacet*>::iterator iter = facet_set.begin(); 
	  iter != facet_set.end(); ++iter ) {
      if ( (**iter)->IsChComplete() ) {
	MinCreateTimeS = ( MinCreateTimeS > (**iter)->CreateTimeS ) ? (**iter)->CreateTimeS : MinCreateTimeS;
	MaxCreateTimeE = ( MaxCreateTimeE < (**iter)->CreateTimeE ) ? (**iter)->CreateTimeE : MaxCreateTimeE;
	  
	MinStoreTimeS = ( MinStoreTimeS > (**iter)->StoreTimeS ) ? (**iter)->StoreTimeS : MinStoreTimeS;
	MaxStoreTimeE = ( MaxStoreTimeE > (**iter)->StoreTimeE ) ? (**iter)->StoreTimeE : MaxStoreTimeE;

	total_size   += (**iter)->Size;
	total_length += (**iter)->Length;
      }
      else {
	all_fragement_completed = false;
	break;
      }
    }

    if ( all_fragement_completed ) {
      CPacket *merged_pkt = new CPacket ( *(first_facet->Transaction->Packet[first_facet->From]) );
      merged_pkt->Size   = total_size;
      merged_pkt->Length = total_length;
      merged_pkt->ChTransferredSize = total_size;
      merged_pkt->DstTransferredSize = total_size;
      merged_pkt->CreateTimeS = MinCreateTimeS;
      merged_pkt->CreateTimeE = MaxCreateTimeE;
      merged_pkt->StoreTimeS = MinStoreTimeS;
      merged_pkt->StoreTimeE = MaxStoreTimeE;
      merged_pkt->SplitStack.pop();

      PFG_vertex merged_vertex = transaction->AddVertex(merged_pkt);
      for ( set<CFacet*>::iterator iter = facet_set.begin(); 
	    iter != facet_set.end(); ++iter ) {
	(*iter)->ToVertex( merged_vertex );
      }

      CFacet *merged_facet = new CFacet(transaction, merged_vertex);
      return merged_facet;
    }
    else {
      return NULL;
    }
  }
  else {
    return NULL;
  }
}


void
CAssembleChannel::Assemble(void) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  vector<CFacet*> to_assemble;

  for ( map<CPacket*, set<CFacet*> >::iterator iter = _AssemblePool.begin(); 
	iter != _AssemblePool.end(); ++iter ) {
    CFacet *merged_facet = MergeFacets( iter->second ); 
      
    if ( merged_facet ) {
      _AssemblePool.erase( iter ) ;
	
      if ( (*merged_facet)->SplitStack.size() ) {
	to_assemble.push_back( merged_facet );
      }
      else {
	_Content.push_back( merged_facet );
      }
    }
  }

  if ( to_assemble.size() ) {
    for ( vector<CFacet*>::iterator iter = to_assemble.begin();
	  iter != to_assemble.end(); ++iter ) {
      SplitStatus *top = (**iter)->SplitStack.top();

      _AssemblePool[top->Source].insert( (*iter) );
    }

    Assemble();
  }
}

void
CAssembleChannel::Write( CBite *bite ) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( (*(bite->Facet))->SplitStack.size() ) { // need to assemble
    CPacket *pkt = bite->Facet->Transaction->Packet[bite->Facet->From];
    pkt->ChTransferredSize += bite->Size;

    SplitStatus *top = pkt->SplitStack.top();
    CPacket *src    = top->Source;

    if ( _AssemblePool.count(src) == 0 || _AssemblePool[src].count(bite->Facet) == 0 ) {
      (*(bite->Facet))->StoreTimeS = G_TIME_TO_SECONDS;
      _AssemblePool[src].insert(bite->Facet);
    }

    if ( bite->Last ) {
      (*(bite->Facet))->StoreTimeE = G_TIME_TO_SECONDS;
    }

    _PacketFlow[(*(bite->Facet))->Name] += bite->Size;
    _Occupied += bite->Size;
  }
  else { // normal write
    CSimpleChannel::Write(bite);
  }

  request_update();
}


// ------------------------------
//    CAccumulateChannel
// ------------------------------
void
CAccumulateChannel::Write(CBite *bite) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  if ( (*(bite->Facet))->Accumulate ) { // need accumulate
    CPacket *source_packet = bite->Facet->Transaction->Packet[bite->Facet->From];
    CFacet *sink_facet = _AccumulatedFacets[source_packet->Name];
    CPacket *sink_packet;
    if ( !sink_facet ) {
      sink_packet = new CPacket ( *source_packet );
      sink_packet->CreateTimeS = G_TIME_TO_SECONDS;
      sink_packet->StoreTimeS  = G_TIME_TO_SECONDS;
      PFG_vertex sink_vertex = G_TRANS.AddVertex( sink_packet );
      sink_facet = new CFacet ( &G_TRANS, sink_vertex );
    }
    else {
      bite->Facet->ToVertex(sink_facet->From);

      if ( (*(bite->Facet))->Accumulate == SIZE ) {
	(*sink_facet)->Size   += (*(bite->Facet))->Size;
	(*sink_facet)->Length += (*(bite->Facet))->Length;
      }
      else {
	(*sink_facet)->Size   += (*(bite->Facet))->Length;
	(*sink_facet)->Length += (*(bite->Facet))->Length;
      }
    }

    if ( bite->Last ) {
      (*sink_facet)->CreateTimeE = source_packet->CreateTimeE;
      (*sink_facet)->StoreTimeE  = G_TIME_TO_SECONDS;
    }

    _PacketFlow[(*(bite->Facet))->Name] += bite->Size;
    _Occupied += bite->Size;
  }
  else { // normal write
    CSimpleChannel::Write(bite);
  }
  request_update();

}


CFacet*
CAccumulateChannel::PickUpFacet(CFacetCondition *condition){
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  CFacet *result = CSimpleChannel::PickUpFacet(condition);
  if ( !result ) {
    for ( map<string, CFacet*>::iterator iter = _AccumulatedFacets.begin(); 
	  iter != _AccumulatedFacets.end(); ++iter) {
      if ( (*condition) ( iter->second) ) {
	return iter->second; 
      }
    }
    return NULL;
  }
  else {
    return result;
  }
}


bool
CAccumulateChannel::Read( CFacet *facet, uint64 new_offset) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  for ( list<CFacet*>::iterator iter = _Content.begin(); 
	iter != _Content.end(); ++iter) {
    if ( facet == *iter ) {
      return CSimpleChannel::Read(facet, new_offset);
    }
  }

  for ( map<string, CFacet*>::iterator iter = _AccumulatedFacets.begin(); 
	iter != _AccumulatedFacets.end(); ++iter) {
    if ( facet == ( iter->second) ) {
      uint64 size = new_offset - (*facet)->Offset;
	
      _Occupied -= size;
	
      (*facet)->Offset = new_offset >= (*facet)->Size ? (*facet)->Size : new_offset ;
      request_update();
      return false; 
    }
  }

  ERROR << "Can not read Facet in accumulate channel." << endl;
  STOP();
  return false;
}

void
CAccumulateChannel::Pop(CFacet *facet) {
#ifdef __DEBUG_VERSION__
  SetDebugFlag();
#endif

  
  for ( list<CFacet*>::iterator iter = _Content.begin(); 
	iter != _Content.end(); ++iter) {
    if ( facet == *iter ) {
      CSimpleChannel::Pop(facet);
      return;
    }
  }
  request_update();
   
  ERROR << "Can not pop an accumulated facet" << endl;
}
