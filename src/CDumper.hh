#ifndef __CDUMPER_HH__
#define __CDUMPER_HH__

#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
using std::stringstream;
using std::string;
using std::exception;

#include "DTL.h"
#include "table.h"
using namespace dtl;



struct th_table_row { double sim_time; string th_name; string transform_name; string dst_name; string status; }; 
typedef dtl::DBView<th_table_row> th_table_view;


struct ch_table_row { double sim_time; string ch_name; unsigned long facet_count; unsigned long occupied; unsigned long capacitance; }; 
typedef dtl::DBView<ch_table_row> ch_table_view;


struct dst_table_row { double sim_time; string dst_name; string src_name; string status; }; 
typedef dtl::DBView<dst_table_row> dst_table_view;

struct pktflow_table_row { double sim_time; string ch_name; string pkt_name; double flow_count; }; 
typedef dtl::DBView<pktflow_table_row> pktflow_table_view; 
 
struct pktinfo_table_row { string trans_name; double trans_id; int pkt_index; string pkt_name; double pkt_size; double pkt_length; double extra_delay; double create_time_s; double create_time_e; double store_time_s; double store_time_e; double wait_time_s; double wait_time_e; int split; int accumulate; };
typedef dtl::DBView<pktinfo_table_row> pktinfo_table_view; 


struct latency_table_row { string trans_name; double trans_id; string from_pkt; string to_pkt; double latency_S; double latency_E; }; 
typedef dtl::DBView<latency_table_row> latency_table_view; 

struct edge_table_row { string trans_name; int trans_id; int src_vertex; int dst_vertex; }; 
typedef dtl::DBView<edge_table_row> edge_table_view; 

struct pipeline_table_row { double sim_time; string dst_name; string pipeline_status; string msg; };
typedef dtl::DBView<pipeline_table_row> pipeline_table_view; 


class CDumper 
{
private: 
  bool _Connected;
  string _Prefix;

  string _TH_tName, 
    _CH_tName, 
    _DST_tName, 
    _PktFlow_tName,
    _PktInfo_tName,
    _Latency_tName,
    _Edge_tName,
    _Pipeline_tName;

  th_table_view *_vTH;
  ch_table_view *_vCH;
  dst_table_view *_vDST;
  pktflow_table_view *_vPktFlow;
  pktinfo_table_view *_vPktInfo;
  latency_table_view *_vLatency;
  edge_table_view *_vEdge;
  pipeline_table_view *_vPipeline;
  


public: 
  CDumper ();

private:
  void CreateName();

public:
  void SetPrefix(const string &);
  const string & ConnectDB(const string &odbc_connect_string);
  void CreateTable();

  void THLog(const double &sim_time, 
	     const string &th_name, 
	     const string &transform_name,
	     const string &dst_name,
	     const string &status);

  void CHLog(const double &sim_time, 
	     const string &ch_name,
	     const unsigned long &facet_count,
	     const unsigned long &occupied,
	     const unsigned long &capacitance );

  void DSTLog(const double &sim_time,
	      const string &dst_name,
	      const string &src_name,
	      const string &status );
  
  void PktFlowLog(const double &sim_time,
		  const string &ch_name,
		  const string &pkt_name,
		  const double &flow_count);

  void PktInfoLog (const string &trans_name,
		   const double &trans_id,
		   const int    &pkt_index,
		   const string &pkt_name, 
		   const double &pkt_size,
		   const double &pkt_length,
		   const double &extra_delay,
		   const double &create_time_s,
		   const double &create_time_e,
		   const double &store_time_s,
		   const double &store_time_e,
		   const double &wait_time_s,
		   const double &wait_time_e,
		   const int &split,
		   const int &accumulate );

  void LatencyLog(const string &trans_name,
		  const double &trans_id,
		  const string &from_pkt,
		  const string &to_pkt, 
		  const double &latency_S,
		  const double &latency_E);

  void EdgeLog(const string &trans_name,
	       const int &trans_id,
	       const int &src_vertex,
	       const int &dst_vertex);
  
  void PipelineLog(const double &sim_time,
		   const string &dst_name, 
		   const string &pipeline_status,
		   const string &msg);

};



extern CDumper DBDumper;


#endif
