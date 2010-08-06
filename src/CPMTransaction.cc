#include "CPMTransaction.hh"

extern void STOP();

PFG_vertex
CPMTransaction::AddVertex(CPacket *packet) 
{
  PFG_vertex v  = add_vertex(_PacketFlowGraph);
  Packet[v]     = packet;
  
  stringstream ss;
  ss << Index[v] << ",\\n" << packet->Name ;
  
  // VertexName[v] = packet->Name;
  VertexName[v] = ss.str();
  return v;
}

PFG_edge
CPMTransaction::AddEdge(PFG_vertex from, PFG_vertex to) 
{
  pair<PFG_edge, bool> r = add_edge(from, to, _PacketFlowGraph);
  if ( ! r.second )  
    {
      ERROR << "**Error: Edge add faile" << endl;
      STOP();
    }
  return r.first;
}

unsigned long
CPMTransaction::Size() 
{
  return num_vertices(_PacketFlowGraph);
}

const string
CPMTransaction::Name() 
{ 
  return _Name; 
}

void
CPMTransaction::WriteGraphviz(ostream& os) 
{
  CalcWeight();
  write_graphviz(os, _PacketFlowGraph,
		 make_label_writer(VertexName), make_label_writer(Weight) );
}

void  
CPMTransaction::ReportCriticalFlow() 
{};

void  
CPMTransaction::ReportCriticalPath() 
{};


// ----------------------------------------------------------------------
//             WatiTimeS
//        |<---------------------->|
//        |                        |
//        |                        |
//        |          WaitTimeE     |
//        |                  \     |
//        |              |<------->|
//        |              |         |
//        |              |         +==============================+
//        |              |         | dst_pkt                      |
//        |              |         +==============================+
//        +==============+
//        | src_pkt (ch) |
//        +==============+
//        ^              ^
//        |              |
//     StoreTimeS     StoreTimeE
//
//    +==============+
//    |  src_pkt     |
//    +==============+
//    ^              ^
//    |              |
//  CreateTimeS   CreateTimeE
//                                                                     time
// -------------------------------------------------------------------------> 
void
CPMTransaction::ReportLatency(unsigned long id) 
{
  CalcWeight();
    
  CMatrix<double> DistanceMatrix(Size(), Size(), std::numeric_limits<double>::max());

  johnson_all_pairs_shortest_paths(_PacketFlowGraph, DistanceMatrix, Index, Weight, 0);

  for (unsigned long i=0; i<DistanceMatrix.size1(); ++i)
    for (unsigned long j=i+1; j<DistanceMatrix.size2(); ++j) {
      if ( DistanceMatrix[i][j] != std::numeric_limits<double>::max() ) {
	double latency_S = Packet[i]->CreateTimeS == -1 ? Packet[j]->CreateTimeS : Packet[j]->CreateTimeS - Packet[i]->CreateTimeS;
	double latency_E = Packet[i]->CreateTimeE == -1 ? Packet[j]->CreateTimeE : Packet[j]->CreateTimeE - Packet[i]->CreateTimeE;
      
	DBDumper.LatencyLog(_Name, 
			    id, 
			    Packet[i]->Name,
			    Packet[j]->Name,
			    latency_S,
			    latency_E);
      }
    }

  //  LOG << DistanceMatrix << endl;
}


void 
CPMTransaction::DumpAllPacket(unsigned long id)
{
  CalcWeight();

  PFG_vertex_iter v, v_end;
  
  for (tie(v, v_end)=vertices(_PacketFlowGraph); v != v_end; ++v)  {
    CPacket *pkt = Packet[*v];
    DBDumper.PktInfoLog (_Name, 
			 id, 
			 
			 Index[*v],
			 pkt->Name,
			 pkt->Size,
			 pkt->Length,
			 pkt->ExtraDelay,

			 pkt->CreateTimeS, pkt->CreateTimeE,
			 pkt->StoreTimeS,  pkt->StoreTimeE,
			 pkt->WaitTimeS, pkt->WaitTimeE,

			 pkt->SplitStack.size(),
			 pkt->Accumulate );

  }
}

void 
CPMTransaction::DumpAllEdge(unsigned long id)
{
  CalcWeight();

  PFG_edge_iter e, e_end;
  
  for (tie(e, e_end)=edges(_PacketFlowGraph); e != e_end; ++e)  {
    DBDumper.EdgeLog (_Name, 
		      id, 
		      Index[source(*e, _PacketFlowGraph)],
		      Index[target(*e, _PacketFlowGraph)]
		      );
  }
}


void  
CPMTransaction::CalcWeight() 
{
  if ( _WeightDone ) return ;

  LOG << "Calculating weight for each edge in transaction \"" << Name() << "\"...";
  PFG_edge_iter ei, ei_end;
  for ( tie(ei, ei_end) = edges(_PacketFlowGraph); ei != ei_end; ++ei) {
    CPacket *src_pkt = Packet[source(*ei, _PacketFlowGraph)];
    CPacket *tgt_pkt = Packet[target(*ei, _PacketFlowGraph)];
    if (src_pkt->CreateTimeS == -1) {
      src_pkt->WaitTimeS = 0;
      src_pkt->WaitTimeE = 0;
    }
    else {
      src_pkt->WaitTimeS = tgt_pkt->CreateTimeS - src_pkt->StoreTimeS;
      src_pkt->WaitTimeE = tgt_pkt->CreateTimeS - src_pkt->StoreTimeE;
    }

    if (tgt_pkt->CreateTimeE == std::numeric_limits<double>::max() ) {
      Weight[*ei] = std::numeric_limits<double>::max() ;
    }
    else {
      //double wt = tgt_pkt->CreateTimeE - tgt_pkt->CreateTimeS + src_pkt->WaitTimeS;
      double wt = tgt_pkt->CreateTimeS - src_pkt->CreateTimeS;
      Weight[*ei] = Weight[*ei] > wt ? wt : Weight[*ei];
    }
  }

  _WeightDone = true;

  LOG << "Done!" << endl;
}
