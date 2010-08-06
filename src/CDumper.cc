#include "CDumper.hh"

#include "CReport.hh"
extern CError ERROR;


// th_table
dtl::tostream &operator<<(dtl::tostream &o, const th_table_row &s) { o << "("; o << s.sim_time << ", " ; o << s.th_name << ", " ; o << s.transform_name << ", " ; o << s.dst_name << ", " ; o << s.status ; o << ")"; return o; }; bool operator<(const th_table_row &lhs, const th_table_row &rhs) { if ((lhs.sim_time) < (rhs.sim_time)) return true; else if ((lhs.sim_time) > (rhs.sim_time)) return false;; if ((lhs.th_name) < (rhs.th_name)) return true; else if ((lhs.th_name) > (rhs.th_name)) return false;; if ((lhs.transform_name) < (rhs.transform_name)) return true; else if ((lhs.transform_name) > (rhs.transform_name)) return false;; if ((lhs.dst_name) < (rhs.dst_name)) return true; else if ((lhs.dst_name) > (rhs.dst_name)) return false;; if ((lhs.status) < (rhs.status)) return true; else if ((lhs.status) > (rhs.status)) return false;; return false; } namespace dtl { template<> class DefaultBCA<th_table_row> { public: void operator()(BoundIOs &cols, th_table_row &row) { cols["sim_time"] >> row.sim_time; cols["th_name"] >> row.th_name; cols["transform_name"] >> row.transform_name; cols["dst_name"] >> row.dst_name; cols["status"] >> row.status; } }; }; 



// ch_table
dtl::tostream &operator<<(dtl::tostream &o, const ch_table_row &s) { o << "("; o << s.sim_time << ", " ; o << s.ch_name << ", " ; o << s.facet_count << ", " ; o << s.occupied << ", " ; o << s.capacitance ; o << ")"; return o; }; bool operator<(const ch_table_row &lhs, const ch_table_row &rhs) { if ((lhs.sim_time) < (rhs.sim_time)) return true; else if ((lhs.sim_time) > (rhs.sim_time)) return false;; if ((lhs.ch_name) < (rhs.ch_name)) return true; else if ((lhs.ch_name) > (rhs.ch_name)) return false;; if ((lhs.facet_count) < (rhs.facet_count)) return true; else if ((lhs.facet_count) > (rhs.facet_count)) return false;; if ((lhs.occupied) < (rhs.occupied)) return true; else if ((lhs.occupied) > (rhs.occupied)) return false;; if ((lhs.capacitance) < (rhs.capacitance)) return true; else if ((lhs.capacitance) > (rhs.capacitance)) return false;; return false; } namespace dtl { template<> class DefaultBCA<ch_table_row> { public: void operator()(BoundIOs &cols, ch_table_row &row) { cols["sim_time"] >> row.sim_time; cols["ch_name"] >> row.ch_name; cols["facet_count"] >> row.facet_count; cols["occupied"] >> row.occupied; cols["capacitance"] >> row.capacitance; } }; }; 
 


// dst_table
dtl::tostream &operator<<(dtl::tostream &o, const dst_table_row &s) { o << "("; o << s.sim_time << ", " ; o << s.dst_name << ", " ; o << s.src_name << ", " ; o << s.status ; o << ")"; return o; }; bool operator<(const dst_table_row &lhs, const dst_table_row &rhs) { if ((lhs.sim_time) < (rhs.sim_time)) return true; else if ((lhs.sim_time) > (rhs.sim_time)) return false;; if ((lhs.dst_name) < (rhs.dst_name)) return true; else if ((lhs.dst_name) > (rhs.dst_name)) return false;; if ((lhs.src_name) < (rhs.src_name)) return true; else if ((lhs.src_name) > (rhs.src_name)) return false;; if ((lhs.status) < (rhs.status)) return true; else if ((lhs.status) > (rhs.status)) return false;; return false; } namespace dtl { template<> class DefaultBCA<dst_table_row> { public: void operator()(BoundIOs &cols, dst_table_row &row) { cols["sim_time"] >> row.sim_time; cols["dst_name"] >> row.dst_name; cols["src_name"] >> row.src_name; cols["status"] >> row.status; } }; }; 


// pktflow table
dtl::tostream &operator<<(dtl::tostream &o, const pktflow_table_row &s) { o << "("; o << s.sim_time << ", " ; o << s.ch_name << ", " ; o << s.pkt_name << ", " ; o << s.flow_count ; o << ")"; return o; }; bool operator<(const pktflow_table_row &lhs, const pktflow_table_row &rhs) { if ((lhs.sim_time) < (rhs.sim_time)) return true; else if ((lhs.sim_time) > (rhs.sim_time)) return false;; if ((lhs.ch_name) < (rhs.ch_name)) return true; else if ((lhs.ch_name) > (rhs.ch_name)) return false;; if ((lhs.pkt_name) < (rhs.pkt_name)) return true; else if ((lhs.pkt_name) > (rhs.pkt_name)) return false;; if ((lhs.flow_count) < (rhs.flow_count)) return true; else if ((lhs.flow_count) > (rhs.flow_count)) return false;; return false; } namespace dtl { template<> class DefaultBCA<pktflow_table_row> { public: void operator()(BoundIOs &cols, pktflow_table_row &row) { cols["sim_time"] >> row.sim_time; cols["ch_name"] >> row.ch_name; cols["pkt_name"] >> row.pkt_name; cols["flow_count"] >> row.flow_count; } }; };  

// pktinfo table
dtl::tostream &operator<<(dtl::tostream &o, const pktinfo_table_row &s) { o << "("; o << s.trans_name << ", " ; o << s.trans_id << ", " ; o << s.pkt_index << ", " ; o << s.pkt_name << ", " ; o << s.pkt_size << ", " ; o << s.pkt_length << ", " ; o << s.extra_delay << ", " ; o << s.create_time_s << ", " ; o << s.create_time_e << ", " ; o << s.store_time_s << ", " ; o << s.store_time_e << ", " ; o << s.wait_time_s << ", " ; o << s.wait_time_e << ", " ; o << s.split << ", " ; o << s.accumulate ; o << ")"; return o; }; bool operator<(const pktinfo_table_row &lhs, const pktinfo_table_row &rhs) { if ((lhs.trans_name) < (rhs.trans_name)) return true; else if ((lhs.trans_name) > (rhs.trans_name)) return false;; if ((lhs.trans_id) < (rhs.trans_id)) return true; else if ((lhs.trans_id) > (rhs.trans_id)) return false;; if ((lhs.pkt_index) < (rhs.pkt_index)) return true; else if ((lhs.pkt_index) > (rhs.pkt_index)) return false;; if ((lhs.pkt_name) < (rhs.pkt_name)) return true; else if ((lhs.pkt_name) > (rhs.pkt_name)) return false;; if ((lhs.pkt_size) < (rhs.pkt_size)) return true; else if ((lhs.pkt_size) > (rhs.pkt_size)) return false;; if ((lhs.pkt_length) < (rhs.pkt_length)) return true; else if ((lhs.pkt_length) > (rhs.pkt_length)) return false;; if ((lhs.extra_delay) < (rhs.extra_delay)) return true; else if ((lhs.extra_delay) > (rhs.extra_delay)) return false;; if ((lhs.create_time_s) < (rhs.create_time_s)) return true; else if ((lhs.create_time_s) > (rhs.create_time_s)) return false;; if ((lhs.create_time_e) < (rhs.create_time_e)) return true; else if ((lhs.create_time_e) > (rhs.create_time_e)) return false;; if ((lhs.store_time_s) < (rhs.store_time_s)) return true; else if ((lhs.store_time_s) > (rhs.store_time_s)) return false;; if ((lhs.store_time_e) < (rhs.store_time_e)) return true; else if ((lhs.store_time_e) > (rhs.store_time_e)) return false;; if ((lhs.wait_time_s) < (rhs.wait_time_s)) return true; else if ((lhs.wait_time_s) > (rhs.wait_time_s)) return false;; if ((lhs.wait_time_e) < (rhs.wait_time_e)) return true; else if ((lhs.wait_time_e) > (rhs.wait_time_e)) return false;; if ((lhs.split) < (rhs.split)) return true; else if ((lhs.split) > (rhs.split)) return false;; if ((lhs.accumulate) < (rhs.accumulate)) return true; else if ((lhs.accumulate) > (rhs.accumulate)) return false;; return false; } namespace dtl { template<> class DefaultBCA<pktinfo_table_row> { public: void operator()(BoundIOs &cols, pktinfo_table_row &row) { cols["trans_name"] >> row.trans_name; cols["trans_id"] >> row.trans_id; cols["pkt_index"] >> row.pkt_index; cols["pkt_name"] >> row.pkt_name; cols["pkt_size"] >> row.pkt_size; cols["pkt_length"] >> row.pkt_length; cols["extra_delay"] >> row.extra_delay; cols["create_time_s"] >> row.create_time_s; cols["create_time_e"] >> row.create_time_e; cols["store_time_s"] >> row.store_time_s; cols["store_time_e"] >> row.store_time_e; cols["wait_time_s"] >> row.wait_time_s; cols["wait_time_e"] >> row.wait_time_e; cols["split"] >> row.split; cols["accumulate"] >> row.accumulate; } }; }; 


// latency table
dtl::tostream &operator<<(dtl::tostream &o, const latency_table_row &s) { o << "("; o << s.trans_name << ", " ; o << s.trans_id << ", " ; o << s.from_pkt << ", " ; o << s.to_pkt << ", " ; o << s.latency_S << ", " ; o << s.latency_E ; o << ")"; return o; }; bool operator<(const latency_table_row &lhs, const latency_table_row &rhs) { if ((lhs.trans_name) < (rhs.trans_name)) return true; else if ((lhs.trans_name) > (rhs.trans_name)) return false;; if ((lhs.trans_id) < (rhs.trans_id)) return true; else if ((lhs.trans_id) > (rhs.trans_id)) return false;; if ((lhs.from_pkt) < (rhs.from_pkt)) return true; else if ((lhs.from_pkt) > (rhs.from_pkt)) return false;; if ((lhs.to_pkt) < (rhs.to_pkt)) return true; else if ((lhs.to_pkt) > (rhs.to_pkt)) return false;; if ((lhs.latency_S) < (rhs.latency_S)) return true; else if ((lhs.latency_S) > (rhs.latency_S)) return false;; if ((lhs.latency_E) < (rhs.latency_E)) return true; else if ((lhs.latency_E) > (rhs.latency_E)) return false;; return false; } namespace dtl { template<> class DefaultBCA<latency_table_row> { public: void operator()(BoundIOs &cols, latency_table_row &row) { cols["trans_name"] >> row.trans_name; cols["trans_id"] >> row.trans_id; cols["from_pkt"] >> row.from_pkt; cols["to_pkt"] >> row.to_pkt; cols["latency_S"] >> row.latency_S; cols["latency_E"] >> row.latency_E; } }; }; 

// edge table 
dtl::tostream &operator<<(dtl::tostream &o, const edge_table_row &s) { o << "("; o << s.trans_name << ", " ; o << s.trans_id << ", " ; o << s.src_vertex << ", " ; o << s.dst_vertex ; o << ")"; return o; }; bool operator<(const edge_table_row &lhs, const edge_table_row &rhs) { if ((lhs.trans_name) < (rhs.trans_name)) return true; else if ((lhs.trans_name) > (rhs.trans_name)) return false;; if ((lhs.trans_id) < (rhs.trans_id)) return true; else if ((lhs.trans_id) > (rhs.trans_id)) return false;; if ((lhs.src_vertex) < (rhs.src_vertex)) return true; else if ((lhs.src_vertex) > (rhs.src_vertex)) return false;; if ((lhs.dst_vertex) < (rhs.dst_vertex)) return true; else if ((lhs.dst_vertex) > (rhs.dst_vertex)) return false;; return false; } namespace dtl { template<> class DefaultBCA<edge_table_row> { public: void operator()(BoundIOs &cols, edge_table_row &row) { cols["trans_name"] >> row.trans_name; cols["trans_id"] >> row.trans_id; cols["src_vertex"] >> row.src_vertex; cols["dst_vertex"] >> row.dst_vertex; } }; }; 

// pipeline table 
dtl::tostream &operator<<(dtl::tostream &o, const pipeline_table_row &s) { o << "("; o << s.sim_time << ", " ; o << s.dst_name << ", " ; o << s.pipeline_status << ", " ; o << s.msg ; o << ")"; return o; }; bool operator<(const pipeline_table_row &lhs, const pipeline_table_row &rhs) { if ((lhs.sim_time) < (rhs.sim_time)) return true; else if ((lhs.sim_time) > (rhs.sim_time)) return false;; if ((lhs.dst_name) < (rhs.dst_name)) return true; else if ((lhs.dst_name) > (rhs.dst_name)) return false;; if ((lhs.pipeline_status) < (rhs.pipeline_status)) return true; else if ((lhs.pipeline_status) > (rhs.pipeline_status)) return false;; if ((lhs.msg) < (rhs.msg)) return true; else if ((lhs.msg) > (rhs.msg)) return false;; return false; } namespace dtl { template<> class DefaultBCA<pipeline_table_row> { public: void operator()(BoundIOs &cols, pipeline_table_row &row) { cols["sim_time"] >> row.sim_time; cols["dst_name"] >> row.dst_name; cols["pipeline_status"] >> row.pipeline_status; cols["msg"] >> row.msg; } }; }; 


CDumper::CDumper() {
  _Connected =  false;

  stringstream ss; 
  ss << time(NULL);
  _Prefix = ss.str();
}


void
CDumper::SetPrefix(const string &prefix)
{
  _Prefix = prefix != "" ? prefix : _Prefix;
}

void
CDumper::CreateName()
{
  _TH_tName         = _Prefix + "_TH";
  _CH_tName         = _Prefix + "_CH";
  _DST_tName        = _Prefix + "_DST";
  _PktFlow_tName    = _Prefix + "_PktFlow";
  _PktInfo_tName    = _Prefix + "_PktInfo";
  _Latency_tName    = _Prefix + "_Latency";
  _Edge_tName       = _Prefix + "_Edge";
  _Pipeline_tName   = _Prefix + "_Pipeline";

  _vTH         = new th_table_view (_TH_tName);
  _vCH         = new ch_table_view (_CH_tName);
  _vDST        = new dst_table_view (_DST_tName);
  _vPktFlow    = new pktflow_table_view (_PktFlow_tName);
  _vPktInfo    = new pktinfo_table_view (_PktInfo_tName);
  _vLatency    = new latency_table_view (_Latency_tName);
  _vEdge       = new edge_table_view (_Edge_tName);
  _vPipeline   = new pipeline_table_view (_Pipeline_tName);
}

const string & 
CDumper::ConnectDB(const string &odbc_connect_string) {
  try{
    DBConnection::GetDefaultConnection().Connect(odbc_connect_string);
  }
  
  catch (exception &ex) {
    // Show any database or other standard errors
    cerr << ex.what() << endl;
    exit(1);
  }

  CreateName();

  _Connected = true;
  return _Prefix;
}


void
CDumper::CreateTable() {
  if ( !_Connected ) {
    ERROR << "**Error: No database availabe, thus no table created." << endl;
    exit(1);
  }

  string create_th_table = "CREATE TABLE " + _TH_tName
    + " (" 
    + "sim_time double, " 
    + "th_name varchar (1024),"
    + "transform_name varchar (1024),"
    + "dst_name varchar (1024),"
    + "status varchar (1024)"
    + ")";

  string create_ch_table = "CREATE TABLE " + _CH_tName
    + " (" 
    + "sim_time double," 
    + "ch_name varchar (1024),"
    + "facet_count integer (32),"
    + "occupied integer (32),"
    + "capacitance integer (32)"
    + ")";

  string create_dst_table = "CREATE TABLE " + _DST_tName
    + " (" 
    + "sim_time double, " 
    + "dst_name varchar (1024),"
    + "src_name varchar (1024),"
    + "status varchar (1024)"
    + ")";

  string create_pktflow_table = "CREATE TABLE " + _PktFlow_tName
    + " (" 
    + "sim_time double, " 
    + "ch_name varchar (1024),"
    + "pkt_name varchar (1024),"
    + "flow_count double"
    + ")";

  string create_pktinfo_table = "CREATE TABLE " + _PktInfo_tName
    + " (" 
    + "trans_name varchar (1024),"
    + "trans_id double,"
    + "pkt_index int,"
    + "pkt_name varchar (1024)," 
    + "pkt_size double,"
    + "pkt_length double,"
    + "extra_delay double,"
    + "create_time_s double,"
    + "create_time_e double,"
    + "store_time_s double,"
    + "store_time_e double,"
    + "wait_time_s double,"
    + "wait_time_e double,"
    + "split integer,"
    + "accumulate integer"
    + ")";


  string create_latency_table = "Create TABLE " + _Latency_tName 
    + "(" 
    + "trans_name varchar (1024),"
    + "trans_id double,"
    + "from_pkt varchar (1024),"
    + "to_pkt varchar (1024)," 
    + "latency_S double,"
    + "latency_E double"
    + ")";



  string create_edge_table = "Create TABLE " + _Edge_tName
    + "(" 
    + "trans_name varchar( 1024),"
    + "trans_id int,"
    + "src_vertex int,"
    + "dst_vertex int"
    + ")";


  string create_pipeline_table = "Create TABLE " + _Pipeline_tName
    + "(" 
    + "sim_time double,"
    + "dst_name varchar (1024),"
    + "pipeline_status varchar (1024),"
    + "msg varchar (1024)"
    + ")";




  try{
    DBStmt(create_th_table).Execute();
    DBStmt(create_ch_table).Execute();
    DBStmt(create_dst_table).Execute();
    DBStmt(create_pktflow_table).Execute();
    DBStmt(create_pktinfo_table).Execute();
    DBStmt(create_latency_table).Execute();
    DBStmt(create_edge_table).Execute();
    DBStmt(create_pipeline_table).Execute();
  }
  
  catch (exception &ex) {
    // Show any database or other standard errors
    cerr << ex.what() << endl;
    exit(1);
  }
}

void
CDumper::THLog(const double &sim_time, 
	       const string &th_name, 
	       const string &transform_name,
	       const string &dst_name,
	       const string &status)
{
  th_table_row th_row;
  
  th_row.sim_time = sim_time; 
  th_row.th_name = th_name; 
  th_row.transform_name = transform_name;
  th_row.dst_name = dst_name;
  th_row.status = status;

  th_table_view::insert_iterator writ = *_vTH;
  *writ = th_row;
}

void
CDumper::CHLog(const double &sim_time, 
	       const string &ch_name,
	       const unsigned long &facet_count,
	       const unsigned long &occupied,
	       const unsigned long &capacitance )
{
  ch_table_row ch_row;

  ch_row.sim_time = sim_time; 
  ch_row.ch_name = ch_name;
  ch_row.facet_count = facet_count;
  ch_row.occupied = occupied;
  ch_row.capacitance = capacitance;

  ch_table_view::insert_iterator writ = *_vCH;
  *writ = ch_row;
}

void
CDumper::DSTLog(const double &sim_time,
		const string &dst_name,
		const string &src_name,
		const string &status )
{
  dst_table_row dst_row;
  dst_row.sim_time = sim_time;
  dst_row.dst_name = dst_name;
  dst_row.src_name = src_name;
  dst_row.status = status;
  
  dst_table_view::insert_iterator writ = *_vDST;
  *writ = dst_row;
}


void
CDumper::PktFlowLog(const double &sim_time,
		const string &ch_name,
		const string &pkt_name,
		const double &flow_count)
{
  pktflow_table_row pktflow_row;
  pktflow_row.sim_time = sim_time;
  pktflow_row.ch_name  = ch_name;
  pktflow_row.pkt_name = pkt_name;
  pktflow_row.flow_count = flow_count;

  pktflow_table_view::insert_iterator writ = *_vPktFlow;
  *writ = pktflow_row;
}


void 
CDumper::PktInfoLog (const string &trans_name,
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
		     const int &accumulate )
{
  pktinfo_table_row pktinfo_row;
  
  pktinfo_row.trans_name = trans_name;
  pktinfo_row.trans_id = trans_id;
  pktinfo_row.pkt_index = pkt_index;
  pktinfo_row.pkt_name = pkt_name; 
  pktinfo_row.pkt_size = pkt_size;
  pktinfo_row.pkt_length = pkt_length;
  pktinfo_row.extra_delay = extra_delay;
  pktinfo_row.create_time_s = create_time_s;
  pktinfo_row.create_time_e = create_time_e;
  pktinfo_row.store_time_s = store_time_s;
  pktinfo_row.store_time_e = store_time_e;
  pktinfo_row.wait_time_s = wait_time_s;
  pktinfo_row.wait_time_e = wait_time_e;
  pktinfo_row.split = split;
  pktinfo_row.accumulate = accumulate;

  pktinfo_table_view::insert_iterator writ = *_vPktInfo;
  *writ = pktinfo_row;
}


void 
CDumper::LatencyLog(const string &trans_name,
		    const double &trans_id,
		    const string &from_pkt,
		    const string &to_pkt, 
		    const double &latency_S,
		    const double &latency_E)
{
  latency_table_row latency_row;
  latency_row.trans_name = trans_name;
  latency_row.trans_id = trans_id;
  latency_row.from_pkt = from_pkt;
  latency_row.to_pkt = to_pkt; 
  latency_row.latency_S = latency_S;
  latency_row.latency_E = latency_E;

  latency_table_view::insert_iterator writ = *_vLatency;
  *writ = latency_row;
}



void 
CDumper::EdgeLog(const string &trans_name,
		 const int &trans_id,
		 const int &src_vertex,
		 const int &dst_vertex)
{
  edge_table_row edge_row;
  
  edge_row.trans_name = trans_name;
  edge_row.trans_id   = trans_id;
  edge_row.src_vertex = src_vertex;
  edge_row.dst_vertex = dst_vertex;

  edge_table_view::insert_iterator writ = *_vEdge;
  *writ = edge_row;
}


void 
CDumper::PipelineLog(const double &sim_time,
		     const string &dst_name, 
		     const string &pipeline_status,
		     const string &msg)
{
  pipeline_table_row pipeline_row;
  
  pipeline_row.sim_time = sim_time;
  pipeline_row.dst_name = dst_name;
  pipeline_row.pipeline_status = pipeline_status;
  pipeline_row.msg = msg;

  pipeline_table_view::insert_iterator writ = *_vPipeline;
  *writ = pipeline_row;
}
