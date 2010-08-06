#include "CSource.hh"

const string
CSource::Name() 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  

  string cond;
  if ( _FacetCondition ) {
    cond = ".FC";
  }
  else {
    cond = ".CC";
  }

  return "\"" + _Name + "\"" + cond;
}

const string
CSource::ChName() 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  return _SrcCh->Name();
}

const string
CSource::CondName() 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  if ( _FacetCondition ) {
    return "FC";
  }
  else {
    return "CC";
  }
}


void
CSource::BackDoorWrite( CPMTransaction *transaction, const vector<CPacket*> &packets ) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  vector<CFacet*> facets;
  PFG_vertex v;
  for (unsigned int i=0; i<packets.size(); i++) {
    packets[i]->ChTransferredSize = packets[i]->Size;
    v = transaction->AddVertex(packets[i]);
    facets.push_back(new CFacet (transaction, v));
  }
  _SrcCh->Fill( facets ) ;
}
    

CFacet*
CSource::PickUpFacet(CTransHandler *reader) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  Done = false;

  if ( _FacetCondition ) CurrentFacet = _SrcCh->PickUpFacet(_FacetCondition);
  else                   CurrentFacet = _SrcCh->PickUpFacet(_ChannelCondition) ;

  return CurrentFacet;
}

void
CSource::ClaimFacet()
{
  (*CurrentFacet)->Picked = true;
}

bool
CSource::Update(uint64 new_offset, bool dst_done) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  if ( _ChannelCondition ) {
    Done = true;
    return true;
  }
  else {
    switch (_FacetCondition->Mode) {
    case SERIAL_AHEAD: 
      if ( new_offset > 0 || dst_done ) {
	_SrcCh->Pop(CurrentFacet);
	Done = true;
      }
      break;

    case SERIAL_POST:
      if ( new_offset >= (*CurrentFacet)->Size || dst_done ) {
	_SrcCh->Pop(CurrentFacet);
	Done = true;
      }
      else {
	Done = false;
      }
      break;

    case PIPELINE:
      Done = _SrcCh->Read(CurrentFacet, new_offset);
      if (Done) _SrcCh->Pop(CurrentFacet);
      break;
	
    default:
      ERROR << "What is your condition mode?!" << endl;
    }
    return Done;
  }
}

bool
CSource::DataAvailable(uint64 new_offset) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  if ( _ChannelCondition || Done ) {
    return true;
  }
  else {
    return
      (*CurrentFacet)->IsChComplete() || 
      (*CurrentFacet)->ChTransferredSize >= new_offset;
  }
}

bool
CSource::Lock(CTransHandler *reader) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  if ( _FacetCondition ) {
    return _SrcCh->LockRd(reader);
  }
  else {
    return true;
  }
}
    
bool
CSource::Unlock(CTransHandler *reader) 
{
#ifdef __DESTINATION_HH__
  SetDebugFlag();
#endif

  
  if ( _FacetCondition ) {
    return _SrcCh->UnlockRd(reader);
  }
  else {
    return true;
  }
}


