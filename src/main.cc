#include <sys/times.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string>
#include <set>
#include <fstream>
using std::string;
using std::set;
using std::ofstream;



// ----------------------------------------
//    Required Global Variables
// ----------------------------------------

#include <systemc.h>
sc_clock G_CLOCK( "G_CLOCK", 1, SC_NS);

#include "CDumper.hh"
CDumper DBDumper;


#include "CTopScheduler.hh"
CPMTransaction G_TRANS ("G_TRANS");
CTopScheduler G_TOP ("G_TOP");

sc_time G_CURRENT_TIME;

string G_TIME_TO_STRING;
double G_TIME_TO_SECONDS;
double G_TIME_AFTER_POINT;


CTransactionFactory TransactionFactory;

list<CDestination*> G_DST_LIST;

bool G_DBG_CH, G_DBG_SRC, G_DBG_DST, G_DBG_TH, G_DBG_MOD, G_DBG_XFM;
set<string> G_sCH, G_sSRC, G_sDST, G_sTH, G_sMOD, G_sXFM;

bool G_TIME_POINT_STOP;
set<sc_time> G_sTimePoint;

bool G_INTERNAL_ERROR = false;

// ------------------------------
//        Main
// ------------------------------
extern void SetSeed(unsigned int);
extern void PrintHelp(const string &);
extern void PrintVersion();

int main(int argc, char** argv)
{
  timeval start, end;
  gettimeofday( &start, NULL);

  bool verbose_mode = false;
  bool log_mode     = false;

  double sim_time = 32;
  unsigned int seed = 1;

  G_DBG_CH = false;
  G_DBG_SRC = false;
  G_DBG_DST = false;
  G_DBG_TH = false;
  G_DBG_MOD = false;

  G_TIME_POINT_STOP = false;


  // Get command line options
  string arg_opt, arg_val;
  double converted_double;

  for ( int i=1; i<argc; i++) {
    arg_opt = argv[i];
    
    if ( arg_opt == "-s" || arg_opt == "--seed" ) {
      if ( i+1 < argc ) {
	arg_val = argv[++i];
	if (arg_val.find_first_not_of("1234567890") == string::npos) {
	  seed = (unsigned int) atol(arg_val.c_str());
	  SetSeed(seed);
	}
	else {
	  ERROR << "**Invalid number \"" << arg_val << "\" provided at argument " << i << " to -s|--seed option." << endl;
	  exit(1);
	}
      }
      else {
	ERROR << "**Need number for -s|--seed option at argument " << i << endl;
	exit(1);
      }
    }
    else if (arg_opt == "-t" || arg_opt == "--time") {
      if ( i+1 < argc ) {
	arg_val = argv[++i];
	converted_double = strtod(arg_val.c_str(), NULL);
	if ( converted_double == 0 || converted_double == HUGE_VAL || converted_double == -HUGE_VAL ) {
	  ERROR << "**Invalid number \"" << arg_val << "\" at argument " << i << " for -t|--time." << endl;
	  exit(1);
	}
	else {
	  sim_time = converted_double;
	}
      }
      else {
	ERROR << "**Need number for -t|--time option at argument " << i << endl;
	exit(1);
      }

    }
    else if (arg_opt == "--prefix") {
      if (i+1 < argc) {
	DBDumper.SetPrefix(argv[++i]);
      }
      else {
	ERROR << "**Need string for --prefix option at argument " << i << endl;
	exit(1);
      }
    }
    else if (arg_opt == "-dattime") {
      if ( i+1 < argc ) {
	arg_val = argv[++i];
	converted_double = strtod(arg_val.c_str(), NULL);
	if ( converted_double == 0 || converted_double == HUGE_VAL || converted_double == -HUGE_VAL ) {
	  ERROR << "**Invalid double number \"" << arg_val << "\" at argument " << i << " for -dtime." << endl;
	  exit(1);
	}
	else {
	  G_TIME_POINT_STOP = true;
	  G_sTimePoint.insert(sc_time(converted_double, SC_NS));
	}
      }
      else {
	ERROR << "**Need double number for -dattime option at argument " << i << endl;
	exit(1);	  
      }
    }
    else if (arg_opt == "-daftertime") {
      if ( i+1 < argc ) {
	arg_val = argv[++i];
	converted_double = strtod(arg_val.c_str(), NULL);
	if ( converted_double == 0 || converted_double == HUGE_VAL || converted_double == -HUGE_VAL ) {
	  ERROR << "**Invalid double number \"" << arg_val << "\" at argument " << i << " for -daftertime." << endl;
	  exit(1);
	}
	else {
	  G_TIME_POINT_STOP = true;
	  G_TIME_AFTER_POINT = converted_double;
	}
      }
      else {
	ERROR << "**Need double number for -daftertime option at argument " << i << endl;
	exit(1);	  
      }
      
    }
    else if (arg_opt == "--log" ) {
      log_mode = true;
    }
    else if (arg_opt == "--verbose") {
      verbose_mode = true;
      LOG.AddDst(&cout);
    }
    else if (arg_opt == "-dch" ) {
      if ( i+1 < argc ) {
	G_DBG_CH = true;
	G_sCH.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -dch option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-dsrc" ) {
      if ( i+1 < argc ) {
	G_DBG_SRC = true;
	G_sSRC.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -dsrc option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-ddst" ) {
      if ( i+1 < argc ) {
	G_DBG_DST = true;
	G_sDST.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -ddst option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-dth" ) {
      if ( i+1 < argc ) {
	G_DBG_TH = true;
	G_sTH.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -dth option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-dmod" ) {
      if ( i+1 < argc ) {
	G_DBG_MOD = true;
	G_sMOD.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -dmod option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-dxfm") {
      if ( i+1 < argc ) {
	G_DBG_XFM = true;
	G_sXFM.insert(argv[++i]);
      }
      else {
	ERROR << "**Need string for -dxfm option at argument " << i<< endl;
	exit(1);
      }
    }
    else if (arg_opt == "-h" || arg_opt == "--help") {
      PrintHelp(argv[0]);
      exit(0);
    }
    else if (arg_opt == "--version" ) {
      PrintVersion();
      exit(0);
    }
    else {
      ERROR << "**Usupported argument \"" << arg_opt << "\" at " << i << endl;
      PrintHelp(argv[0]);
      exit(1);
    }
  }

  string TablePrefix = DBDumper.ConnectDB("UID=pm;PWD=pm;DSN=pm;");

  string LogFileName  = TablePrefix + ".log";
  ofstream LogFileStream (LogFileName.c_str() );

  if ( log_mode ) {
    WARNING.AddDst(&LogFileStream);
    ERROR.AddDst(&LogFileStream);
    LOG.AddDst(&LogFileStream);
  }

  LOG << "   Table Prefix is " << TablePrefix << endl
      << "Simulation time is " << sim_time << " ns" << endl
      << "           Seed is " << seed << endl
      << endl;

  if ( G_DBG_CH ) {
    LOG << "Debugging Channel: " << endl;
    for ( set<string>::iterator iter = G_sCH.begin(); 
	  iter != G_sCH.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }


  if ( G_DBG_SRC ) {
    LOG << "Debugging Source: " << endl;
    for ( set<string>::iterator iter = G_sSRC.begin(); 
	  iter != G_sSRC.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }


  if ( G_DBG_DST ) {
    LOG << "Debugging Destination: " << endl;
    for ( set<string>::iterator iter = G_sDST.begin(); 
	  iter != G_sDST.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }

  if ( G_DBG_TH ) {
    LOG << "Debugging Transaction Hander: " << endl;
    for ( set<string>::iterator iter = G_sTH.begin(); 
	  iter != G_sTH.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }

  if ( G_DBG_MOD ) {
    LOG << "Debugging Modules: " << endl;
    for ( set<string>::iterator iter = G_sMOD.begin(); 
	  iter != G_sMOD.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }

  if ( G_TIME_POINT_STOP ) {
    LOG << "Stop time points: " << endl;
    for ( set<sc_time>::iterator iter = G_sTimePoint.begin();
	  iter != G_sTimePoint.end(); ++iter) 
      LOG << *iter << endl;
    LOG << endl;
  }


  DBDumper.CreateTable();


  // include your design here
  #include "arch.h"
  #include "flow.h"









  // ==============================
  //   Don't touch following code
  // ==============================
  sc_start( sc_time(sim_time, SC_NS) );


  










  if ( !verbose_mode ) {
    LOG.AddDst(&cout);
  }

  LOG << "Dumping all packet information into database..." << endl;
  TransactionFactory.Dump();

  LOG << "Write DOT format flowchart..." << endl;
  TransactionFactory.WriteGraphviz();

  LOG << "Dumping all pair latency into database..." << endl;
  TransactionFactory.ReportLatency();

  // report time used
  gettimeofday( &end, NULL);
  unsigned long sec, min, hr;

  sec = end.tv_sec - start.tv_sec;

  min = sec / 60;
  sec = sec % 60;
  
  hr  = min / 60;
  min = min % 60;
  
  if (!log_mode) {
    WARNING.AddDst(&LogFileStream);
    ERROR.AddDst(&LogFileStream);
    LOG.AddDst(&LogFileStream);
  }

  LOG << endl
      << "**************************************************" << endl;
  if (G_INTERNAL_ERROR)
    ERROR << "  Performance Simulation Finished with ERROR" << endl;
  else 
    LOG << "      Performance Simulation Finished" << endl;

  LOG << "      -------------------------------" << endl
      << endl
      << " Simulation Time: " << G_TIME_TO_SECONDS << " sec" << endl
      << "     Actual time: " << hr << ":" << min << ":" << sec << endl
      << "    Table Prefix: " << TablePrefix << endl
      << "**************************************************" << endl;

  
  LogFileStream.close();


}

