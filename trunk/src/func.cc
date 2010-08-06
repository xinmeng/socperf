#include <systemc.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <string>
#include <set>
using std::vector;
using std::string;
using std::set;


// ----------------------------------------
//    Global funtions
// ----------------------------------------
void SetSeed(unsigned int seed) 
{
  srandom( seed );
}

uint64 Random()
{
  return random();
}

uint64 Random(uint64 min, uint64 max)
{
  assert(max > min);

  uint64 num = random();
  return num % (max - min + 1) + min;
}

uint64 Random(uint64 min, uint64 max, uint64 step)
{
  assert(step < max - min && max > min);

  uint64 num = random() % (max - min + 1);
  return (num + min) - (num % step);
  
}

unsigned int WeightedRandom(const vector<uint64> & weight) {
  uint64 selector = Random( 0, weight[weight.size()-1]-1 );
  for ( unsigned int i=0; i<weight.size(); i++) 
    if ( selector < weight[i] )  return i;
  return 0;
}


extern bool G_INTERNAL_ERROR;

void
STOP()
{
  G_INTERNAL_ERROR = true;
  sc_stop();
}


void PrintHelp(const string &cmd)
{
  cout << "Command line options:" << endl
       << "  -s, --seed" << endl
       << "    Specify random seed" << endl
       << endl
       << "  -t, --time" << endl
       << "    Specify a double simulation time" << endl
       << endl
       << "  -dch, -dsrc, -ddst, -dth, -dmod" << endl
       << "    Only for debug! Conditional breakpoint assistant." << endl
       << "    Followed by a string denote entity name." << endl
       << endl
       << "  -dtime" << endl
       << "    Only for debug! Conditional breakpoint assistant." << endl
       << "    Followed by a double time point in SC_NS unit to stop." << endl
       << endl
       << "  -v, --version" << endl
       << "    Print version information" << endl
       << endl
       << "  -h, --help" << endl
       << "    Print this message" << endl
       << endl;
}

void PrintVersion()
{
  cout << "version 0.1, June 2009, mengxin@vlsi.zju.edu.cn" << endl
       << endl;
}



extern bool G_DBG_CH, G_DBG_SRC, G_DBG_DST, G_DBG_TH, G_DBG_MOD, G_DBG_XFM;
extern set<string> G_sCH, G_sSRC, G_sDST, G_sTH, G_sMOD, G_sXFM;

bool 
MatchCHName(const string &name)
{
  if ( G_DBG_CH ) 
    return G_sCH.count(name) > 0 ;
  else 
    return false;
}

bool 
MatchSRCName(const string &name)
{
  if ( G_DBG_SRC ) 
    return G_sSRC.count(name) > 0 ;
  else 
    return false;
}

bool 
MatchDSTName(const string &name)
{
  if ( G_DBG_DST ) 
    return G_sDST.count(name) > 0 ;
  else 
    return false;
}


bool 
MatchTHName(const string &name)
{
  if ( G_DBG_TH ) 
    return G_sTH.count(name) > 0 ;
  else 
    return false;
}


bool 
MatchMODName(const string &name)
{
  if ( G_DBG_MOD ) 
    return G_sMOD.count(name) > 0 ;
  else 
    return false;
}

bool
MatchXFMName(const string &name)
{
  if ( G_DBG_XFM ) 
    return G_sXFM.count(name) > 0;
  else 
    return false;
}


extern bool G_TIME_POINT_STOP;
extern set<sc_time> G_sTimePoint;
extern sc_time G_CURRENT_TIME;
extern double G_TIME_TO_SECONDS;
extern double G_TIME_AFTER_POINT;

bool 
IsStopTime()
{
  if ( G_TIME_POINT_STOP ) {
    if ( G_sTimePoint.count(G_CURRENT_TIME) > 0 || 
	 G_TIME_TO_SECONDS > G_TIME_AFTER_POINT )
    return true;
  }
  else {
    return false;
  }
}
