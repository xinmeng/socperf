#include "DTL.h"
#include "table.h"
using namespace dtl;

// DTL_TABLE8(connection_table, 
// 	   double, sim_time,
// 	   string, th_name,
// 	   string, dst_name,
// 	   string, dst_ch_name,
// 	   string, transform_name,
// 	   string, src_name,
// 	   string, src_ch_name, 
// 	   string, src_cond_name);


DTL_TABLE5(th_table,
	   double, sim_time,
	   string, th_name,
	   string, transform_name,
	   string, dst_name, 
	   string, status);

DTL_TABLE5(ch_table,
	   double, sim_time,
	   string, ch_name,
	   unsigned long, facet_count,
	   unsigned long, occupied, 
	   unsigned long, capacitance);

DTL_TABLE4(dst_table,
	   double, sim_time, 
	   string, dst_name,
	   string, src_name, 
	   string, status);

DTL_TABLE4(pktflow_table,
	   double, sim_time, 
	   string, ch_name,
	   string, pkt_name,
	   double, flow_count);

DTL_TABLE15(pktinfo_table,
	    string, trans_name,
	    double, trans_id,
	    int,    pkt_index,
	    string, pkt_name, 
	    double, pkt_size,
	    double, pkt_length,
	    double, extra_delay,
	    double, create_time_s,
	    double, create_time_e,
	    double, store_time_s,
	    double, store_time_e,
	    double, wait_time_s,
	    double, wait_time_e,
	    int, split,
	    int, accumulate
	    );


DTL_TABLE6(latency_table,
	   string, trans_name,
	   double, trans_id,
	   string, from_pkt,
	   string, to_pkt, 
	   double, latency_S,
	   double, latency_E);

DTL_TABLE4(edge_table,
	   string, trans_name,
	   int, trans_id,
	   int, src_vertex,
	   int, dst_vertex);

DTL_TABLE4(
	   pipeline_table,
	   double, sim_time,
	   string, dst_name, 
	   string, pipeline_status,
	   string, msg);
