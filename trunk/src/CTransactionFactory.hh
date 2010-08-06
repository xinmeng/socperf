#ifndef __CTRANSACTIONFACTORY_HH__
#define __CTRANSACTIONFACTORY_HH__

#include "CPMTransaction.hh"
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
using std::map;
using std::vector;
using std::stringstream;


class CTransactionFactory
{
private:
  map<string, vector<CPMTransaction*> > _AllTrans;


public:
  inline CTransactionFactory () {}

  CPMTransaction* NewTransaction( const string &name ) ;

  vector<CPMTransaction*> NewTransaction(const string &name, unsigned int count) ;

  void WriteGraphviz() ;
  bool WriteGraphviz(const string &name) ;

  void ReportLatency() ;
  bool ReportLatency(const string &name) ;

  void Dump();
  bool DumpPacket(const string &name);
  bool DumpEdge(const string &name);




private:

  void Normalize(string &name) ;

};


#endif
