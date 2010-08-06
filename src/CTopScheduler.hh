#ifndef __CTOPSCHEDULER_HH__
#define __CTOPSCHEDULER_HH__


#include "CModule.hh"

extern sc_clock G_CLOCK;
extern sc_time G_CURRENT_TIME;
extern string G_TIME_TO_STRING;
extern double G_TIME_TO_SECONDS; 
extern list<CDestination*> G_DST_LIST;

class CTopScheduler : public sc_module 
{
  SC_HAS_PROCESS (CTopScheduler);

protected:
  vector<CModule*> _ModList;
  string _Name;

public:
  inline CTopScheduler (sc_module_name name) : sc_module (name) {
    _Name = name;

    SC_CTHREAD(Execute, G_CLOCK);
  }
  
  inline const string Name() {return "\"" + _Name + "\"";}

  inline void AddMod( CModule *mod) { _ModList.push_back(mod);}

  inline void Execute() {

    for (;;) {
      stringstream time_str;
      G_CURRENT_TIME = sc_time_stamp();

//       time_str << G_CURRENT_TIME;
//       G_TIME_TO_STRING  = time_str.str();

      G_TIME_TO_SECONDS = G_CURRENT_TIME.to_seconds();

      LOG << "============================================" << endl
	  << "\t" << G_TIME_TO_SECONDS << endl
	  << "============================================" << endl;
      for (unsigned int i=0; i<_ModList.size(); i++) {
	LOG << endl
	    << " Module " + _ModList[i]->Name()<< endl
	    << "---------------------------------------" << endl;
	_ModList[i]->Execute();
      }

      LOG << "\tUpdate pipelines " << endl
	  << "--------------------------------------------" << endl;
      for (list<CDestination*>::iterator iter = G_DST_LIST.begin(); 
	   iter != G_DST_LIST.end(); iter++)
	{
	  (*iter)->UpdatePipeline();
	}
      
      wait();
    }
  }
  
};

extern CTopScheduler G_TOP;

#define module(objname, modname) 		\
  CModule objname (modname);			\
  G_TOP.AddMod(&objname);



#endif
