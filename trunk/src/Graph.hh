#ifndef __GRAPH_HH__
#define __GRAPH_HH__

#include <utility>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>

using namespace boost;
using std::pair;

// ------------------------------
//    Packet Flow Graph
// ------------------------------
enum vertex_packet_t {vertex_packet};

namespace boost {
  BOOST_INSTALL_PROPERTY (vertex, packet);
}


class CPacket;

typedef 
property<vertex_index_t, int, 
	 property<vertex_distance_t, double, 
		  property<vertex_packet_t, CPacket*, 
			   property<vertex_name_t, string> > > > PFGVertexProperty;

typedef
property<edge_weight_t, double> PFGEdgeProperty;


typedef adjacency_list<setS, vecS, directedS, PFGVertexProperty, PFGEdgeProperty> PFG;


typedef property_map<PFG, vertex_index_t>::type PFG_vertex_index_map;
typedef property_map<PFG, vertex_distance_t>::type PFG_vertex_distance_map;
typedef property_map<PFG, vertex_packet_t>::type PFG_vertex_packet_map;
typedef property_map<PFG, vertex_name_t>::type PFG_vertex_name_map;
typedef property_map<PFG, edge_weight_t>::type PFG_edge_weight_map;


typedef graph_traits<PFG>::vertex_descriptor PFG_vertex;
typedef graph_traits<PFG>::edge_descriptor PFG_edge;


// typedef pair<PFG_edge, bool> add_edge_return_type;



typedef graph_traits<PFG>::vertex_iterator PFG_vertex_iter;
typedef graph_traits<PFG>::edge_iterator PFG_edge_iter;

typedef pair<PFG_vertex_iter, PFG_vertex_iter> PFG_vertex_iter_pair;
typedef pair<PFG_edge_iter, PFG_edge_iter> PFG_edge_iter_pair;




#endif
