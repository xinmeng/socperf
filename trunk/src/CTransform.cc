#include "CTransform.hh"

// ------------------------------
//   CTransform
// ------------------------------
void 
CTransform::BindSource(const vector<CSource*> &source)
{
  if ( source.size() != _SourceCount ) {
    ERROR << "ERROR: TRANSFORM " << Name() << " need " << _SourceCount << " sources, but is provided " << source.size() << endl;
    exit(1);
  }
  else {
    _Source = source;
  }
}

vector<CPacket*>
CTransform::ApplyTransform(const vector<CSource*> &source)
{
  #ifdef __DEBUG_VERSION__
  CTransform::SetDebugFlag();
  #endif

  CTransform::BindSource(source);

  vector<CPacket*> packets;
  CPacket *src_pkt = _Source[0]->CurrentFacet->Transaction->Packet[_Source[0]->CurrentFacet->From];
  CPacket *tgt_pkt = new CPacket (*src_pkt);
  packets.push_back(tgt_pkt);
  
  return Process(packets);
}


// ------------------------------
//   CNameTransform
// ------------------------------
vector<CPacket*> 
CNameTransform::Process ( vector<CPacket*> packets ) {
  for ( unsigned int i=0; i<packets.size(); ++i) {
    packets[i]->Name = _NewName;
  }
  return packets;
}


// ------------------------------
//    CCustomTransform
// ------------------------------
vector<CPacket*> 
CCustomTransform::Process(vector<CPacket*> packets) {
  for ( unsigned int i=0; i<packets.size(); i++) {
    packets[i]->CustomFields[_Key] = _Value;
  }

  return packets;
}



// ------------------------------
//    CSplitTransform
// ------------------------------
vector<CPacket*> 
CSplitTransform::Process(vector<CPacket*> packets) 
{
  vector<CPacket*> result;
    
  uint64 round_segments;
  struct SplitStatus *splitstatus;
  CPacket *split_pkt;

  for (unsigned int i=0; i<packets.size(); ++i) {
    switch (_SplitMethod) {
    case onLENGTH:
      _Remainder = packets[i]->Length % _ChunkSize;
      round_segments = (packets[i]->Length / _ChunkSize) + (_Remainder !=0 ? 1 : 0);
      splitstatus = new struct SplitStatus(packets[i], round_segments);
      for ( unsigned int j=0; j<round_segments; j++) {
	split_pkt = new CPacket ( *packets[i] );

	if ( j==round_segments-1) {
	  split_pkt->Length = _Remainder ? _Remainder : _ChunkSize;
	}
	else {
	  split_pkt->Length = _ChunkSize;
	}

	split_pkt->SplitStack.push(splitstatus);
	result.push_back(split_pkt);
      }
      break;
    case onSIZE:
      _Remainder = packets[i]->Size % _ChunkSize;
      round_segments = (packets[i]->Size / _ChunkSize) +  (_Remainder !=0 ? 1 : 0);
      splitstatus = new struct SplitStatus(packets[i], round_segments);
      for ( unsigned int j=0; j<round_segments; j++) {
	split_pkt = new CPacket( * packets[i] ); 

	if ( j==round_segments-1) {
	  split_pkt->Size   = _Remainder ? _Remainder : _ChunkSize;
	  split_pkt->Length = _Remainder ? _Remainder : _ChunkSize;
	}
	else {
	  split_pkt->Size   = _ChunkSize;
	  split_pkt->Length = _ChunkSize;
	}

	split_pkt->SplitStack.push(splitstatus);
	result.push_back(split_pkt);
      }
      break;
    }
  }
  return result;
}



// ------------------------------
//   CNumTransform
// ------------------------------
uint64
CNumTransform::OperandValue() { 
  if ( _UseImmediate ) {
    if ( _UseRandom ) 
      return Random(_Min, _Max, _Step); 
    else 
      return _Immediate;
  }
  else {
    return (*_Field)(_Source[0]->CurrentFacet->Transaction->Packet[_Source[0]->CurrentFacet->From]);
  }
}


// ------------------------------
//    CLengthTransform
// ------------------------------
vector<CPacket*> 
CLengthTransform::Process(vector<CPacket*> packets ) {
  for ( unsigned int i=0; i<packets.size(); ++i) {
    packets[i]->Length = OperandValue();
  }

  return packets;
}




// ------------------------------
//   CLengthTransformPkt
// ------------------------------
vector<CPacket*> 
CLengthTransformPkt::ApplyTransform(const vector<CSource*> &source ) {
  #ifdef __DEBUG_VERSION__
  CTransform::SetDebugFlag();
  #endif

  CTransform::BindSource(source);

  vector<CPacket*> packets;
  CPacket *src_pkt = _Source[0]->CurrentFacet->Transaction->Packet[_Source[0]->CurrentFacet->From];
  CPacket *tgt_pkt = new CPacket (*src_pkt);
  tgt_pkt->Length = OperandValue();
  packets.push_back(tgt_pkt);
  
  return packets;
}


vector<CPacket*> 
CLengthTransformPkt::Process(vector<CPacket*> packets)
{
  ERROR << "ERROR: CLengthTransformPkt " << Name() << " can only be called as 1st transform in a group." << endl;
  exit(1);
}



// ------------------------------
//    CSizeTransform
// ------------------------------
vector<CPacket*> 
CSizeTransform::Process(vector<CPacket*> packets)
{
  for ( unsigned int i=0; i<packets.size(); ++i) {
    uint64 value = OperandValue();

    packets[i]->Size   = value;
    packets[i]->Length = value;
  }
    
  return packets;
}



// ------------------------------
//   CSizeTransformPkt
// ------------------------------
vector<CPacket*>
CSizeTransformPkt::ApplyTransform(const vector<CSource*> &source )
{
  #ifdef __DEBUG_VERSION__
  CTransform::SetDebugFlag();
  #endif

  CTransform::BindSource(source);

  vector<CPacket*> packets;
  CPacket *src_pkt = _Source[0]->CurrentFacet->Transaction->Packet[_Source[0]->CurrentFacet->From];
  CPacket *tgt_pkt = new CPacket (*src_pkt);

  uint64 value = OperandValue();
  tgt_pkt->Size   = value;
  tgt_pkt->Length = value;
  packets.push_back(tgt_pkt);
  
  return packets;  
}


vector<CPacket*> 
CSizeTransformPkt::Process(vector<CPacket*> packets)
{
  ERROR << "ERROR: CSizeTransformPkt " << Name() << " can only be called as 1st transform in a group." << endl;
  exit(1);
}


// ------------------------------
//   CGroupTransform
// ------------------------------
void 
CGroupTransform::AddTransform(CTransform *tranform ) 
{
  _TransformList.push_back( tranform ); 
}

vector<CPacket*> 
CGroupTransform::ApplyTransform(const vector<CSource*> &source)  
{
  #ifdef __DEBUG_VERSION__
  SetDebugFlag();
  #endif

  CTransform::BindSource(source);

  vector<CPacket*> result;
  result = _TransformList[0]->ApplyTransform(source);
  for ( unsigned int i=1; i<_TransformList.size(); i++) {
    result = _TransformList[i]->Process(result);
  }
  return result;
}


vector<CPacket*> 
CGroupTransform::Process(vector<CPacket*> packets) 
{
  #ifdef __DEBUG_VERSION__
  SetDebugFlag();
  #endif

  for ( unsigned int i=0; i<_TransformList.size(); i++) {
    packets = _TransformList[i]->Process(packets);
  }
  return packets;
}

  
const string
CGroupTransform::Name() 
{
  string result = "_GroupXFM_" + _Name + "("; 

  for ( unsigned int i=0; i<_TransformList.size(); ++i) {
    result = result + _TransformList[i]->Name() + ", ";
  }

  result += ")";

  return result;
}


// ------------------------------
//   CRandomTransform
// ------------------------------
void 
CRandomTransform::AddTransform(CTransform* transform, uint64 weight)
{
  _TransformList.push_back( transform );
  if ( _Weight.size() == 0 ) {
    _Weight.push_back(weight);
  }
  else {
    _Weight.push_back(_Weight[_Weight.size()-1] + weight);
  }
}

vector<CPacket*> 
CRandomTransform::ApplyTransform(const vector<CSource*> &source)
{
  #ifdef __DEBUG_VERSION__
  SetDebugFlag();
  #endif

  CTransform::BindSource(source);

  if ( !_CurrentTransform ) {
    ERROR << "ERROR: " << Name() << " is not selected!" << endl;
    exit(1);
  }

  return _CurrentTransform->ApplyTransform(source);
}

vector<CPacket*> 
CRandomTransform::Process(vector<CPacket*> packets) 
{
  #ifdef __DEBUG_VERSION__
  SetDebugFlag();
  #endif

  if ( !_CurrentTransform ) {
    ERROR << "ERROR: " << Name() << " is not selected!" << endl;
    exit(1);
  }

  return _CurrentTransform->Process(packets);
}

const string 
CRandomTransform::Name() 
{
  _CurrentTransform = _TransformList[WeightedRandom(_Weight)];
  
  string result = "_RandXFM_" + _Name + "=" + _CurrentTransform->Name() + "(";
  for ( unsigned int i=0; i<_TransformList.size(); ++i) {
    result = result + _TransformList[i]->Name() + ", ";
  }
  result += ")";

  return result;
}


