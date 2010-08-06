#ifndef __TRANCTION_HH__
#define __TRANCTION_HH__


#include <systemc.h>
#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <sstream>
using std::string;
using std::pair;
using std::map;
using std::vector;
using std::ostream;
using std::cout;
using std::stringstream;



#include <boost/graph/johnson_all_pairs_shortest.hpp>



#include "Graph.hh"
#include "CPacket.hh"
#include "CPacketFactory.hh"

#include "CReport.hh"
extern class CError ERROR;

#include "CDumper.hh"

#include "CMatrix.hh"


class CPMTransaction
{
protected:
  string _Name;
  PFG _PacketFlowGraph;

  bool _WeightDone;


public:
  PFG_vertex_index_map    Index;
  PFG_vertex_distance_map Distance;
  PFG_vertex_packet_map   Packet;
  PFG_vertex_name_map     VertexName;
  PFG_edge_weight_map     Weight;


public:
  inline CPMTransaction (const string &name) : _Name (name), _WeightDone (false) {
    Index      = get(vertex_index_t(), _PacketFlowGraph);
    Distance   = get(vertex_distance_t(), _PacketFlowGraph);
    Packet     = get(vertex_packet_t(), _PacketFlowGraph);
    VertexName = get(vertex_name_t(), _PacketFlowGraph);
    Weight     = get(edge_weight_t(), _PacketFlowGraph);
  };

public:


  PFG_vertex AddVertex(CPacket *packet) ;
  PFG_edge   AddEdge(PFG_vertex from, PFG_vertex to) ;

  unsigned long Size() ;
  const string  Name() ;
  
  void WriteGraphviz(ostream& os = cout) ;

  void DumpAllPacket(unsigned long id);
  void DumpAllEdge(unsigned long id);
  void ReportLatency(unsigned long id);

  void ReportCriticalFlow() ;
  void ReportCriticalPath() ;


private:
  void   CalcWeight() ;






};


extern CPMTransaction G_TRANS;


#endif
