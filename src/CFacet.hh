#ifndef __CFACET_HH__
#define __CFACET_HH__

#include "CPMTransaction.hh"

struct CFacet
{
  CPMTransaction *Transaction;
  PFG_vertex      From;

  inline CFacet(CPMTransaction* transaction, PFG_vertex from ) : 
    Transaction ( transaction ), From (from) {}


  inline CPacket* operator->() {
    return Transaction->Packet[From];
  }

  inline void ToVertex( PFG_vertex v ) {
    PFG_edge e = Transaction->AddEdge(From, v);
    Transaction->Weight[e] = std::numeric_limits<double>::max();
  }

//   inline PFG_vertex ToVertex( CPacket *pakcket ) {
//     PFG_vertex v = Transaction->AddVertex(pakcket);
//     PFG_edge e = Transaction->AddEdge(From, v);
//     Transaction->Weight[e] = std::numeric_limits<double>::max();
//     return v;
//   }


};



#endif
