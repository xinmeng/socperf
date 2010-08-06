#include "CTransactionFactory.hh"



CPMTransaction*
CTransactionFactory::NewTransaction( const string &name ) 
{
  CPMTransaction *trans = new CPMTransaction (name);
    
  _AllTrans[name].push_back(trans);

  return trans;
}

vector<CPMTransaction*>
CTransactionFactory::NewTransaction(const string &name, unsigned int count) 
{
  vector<CPMTransaction*> result;
    
  CPMTransaction *trans;
  for	(unsigned long i=0; i<count; ++i) {
    trans = new CPMTransaction (name);
    result.push_back(trans);
  }

  _AllTrans[name].insert(_AllTrans[name].end(), result.begin(), result.end());

  return result;
}

void
CTransactionFactory::WriteGraphviz() {
  for ( map<string, vector<CPMTransaction*> >::iterator iter = _AllTrans.begin();
	iter != _AllTrans.end(); ++iter) {
    WriteGraphviz(iter->first);
  }
}

bool
CTransactionFactory::WriteGraphviz(const string &name) 
{
  if ( _AllTrans.count(name) ) {
    for ( unsigned long i=0; i<_AllTrans[name].size(); ++i) {
      stringstream FileName;
      FileName << name << "." << i << ".dot";

      string FileNameStr = FileName.str();

      Normalize(FileNameStr);

      ofstream DotFile(FileNameStr.c_str());
      _AllTrans[name][i]->WriteGraphviz(DotFile);
    }
    return true;
  }
  else {
    return false;
  }
}

void
CTransactionFactory::ReportLatency() 
{
  for ( map<string, vector<CPMTransaction*> >::iterator iter = _AllTrans.begin();
	iter != _AllTrans.end(); ++iter) {
    ReportLatency(iter->first);
  }
}

bool
CTransactionFactory::ReportLatency(const string &name) 
{
  if ( _AllTrans.count(name) ) {
    for ( unsigned long i=0; i<_AllTrans[name].size(); ++i) {
      _AllTrans[name][i]->ReportLatency(i);
    }
    return true;
  }
  else {
    return false;
  }
}



void 
CTransactionFactory::Dump()
{
  for ( map<string, vector<CPMTransaction*> >::iterator iter = _AllTrans.begin();
	iter != _AllTrans.end(); ++iter) {
    DumpPacket(iter->first);
    DumpEdge(iter->first);
  }
}

bool 
CTransactionFactory::DumpPacket(const string &name)
{
  if ( _AllTrans.count(name) ) {
    for ( unsigned long i=0; i<_AllTrans[name].size(); ++i) {
      _AllTrans[name][i]->DumpAllPacket(i);
    }
    return true;
  }
  else {
    return false;
  }
}


bool 
CTransactionFactory::DumpEdge(const string &name)
{
  if ( _AllTrans.count(name) ) {
    for ( unsigned long i=0; i<_AllTrans[name].size(); ++i) {
      _AllTrans[name][i]->DumpAllEdge(i);
    }
    return true;
  }
  else {
    return false;
  }
}




void
CTransactionFactory::Normalize(string &name) 
{
  size_t pos;

  while ( (pos = name.find(':')) != string::npos ) {
    name.replace(pos, 1, "-" );
  }

  while ( (pos = name.find(' ')) != string::npos ) {
    name.replace(pos, 1, "_");
  }
}

