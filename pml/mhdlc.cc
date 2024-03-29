#include "MetaHDL.hh"
#include <fstream>
#include <iostream>
using namespace std;

#include <sys/times.h>
#include <sys/time.h>

#include <EXTERN.h>
#include <perl.h>


CModTab G_ModuleTable;
CNumber* CONST_NUM_0 = new CNumber(32, 0);

PerlInterpreter *my_perl;

int main(int argc, char *argv[])
{
  timeval start, end;
  gettimeofday( &start, NULL);


  GetOpt("Command Line", argc, argv);
  ostream Log (cerr.rdbuf());
  ofstream LogFile;
  LogFile.open("mhdlc.log", ios_base::out);
  if ( ! LogFile.is_open() ) {
    cerr << "**Warning: Cannot open log file: mhdlc.log." << endl
	 << "           All logs go to stderr." << endl
	 << endl;
  }
  else {
    Log.rdbuf( LogFile.rdbuf() );
  }
  RptOpt(Log);
  CreateWorkdir();
  

  // for embeded perl
  char *embedding[] = { "", "-e", "0" };
  PERL_SYS_INIT3(NULL,NULL,NULL);
  my_perl = perl_alloc();
  perl_construct(my_perl);
  perl_parse(my_perl, NULL, 3, embedding, NULL);
  PL_exit_flags |= PERL_EXIT_DESTRUCT_END;



  // process files
  ulonglong file_cnt = files.size();
  for (ulonglong i = 0; i < file_cnt; ++i ) {
    tFileName f = DecomposeName( files[i] );

    CMHDLwrapper *mwrapper;
    cerr << "\n" << i+1 << "/" << file_cnt << " Parsing PML file:" << files[i] << endl;
      mwrapper = new CMHDLwrapper (files[i]);
      mwrapper->Parse();
  }

  Log << endl
      << endl
      << "==================================" << endl
      << " " << G_ModuleTable.Size() << " modules got during this run  " << endl
      << "==================================" << endl;
  
  G_ModuleTable.Print(Log);



//   Log << endl
//       << endl
//       << "==================================" << endl
//       << "    All macro in this run  " << endl
//       << "==================================" << endl;
//   print_macros(Log);


  LogFile.close();


  // free perl
  PL_perl_destruct_level = 1;
  perl_destruct(my_perl);
  perl_free(my_perl);
  PERL_SYS_TERM();



  // report time used
  gettimeofday( &end, NULL);

  struct tms time_used;
  clock_t t = times(&time_used);
  long CLK_PER_SEC = sysconf(_SC_CLK_TCK);
  if ( t == -1 ) {
    cerr << "Times overflow." << endl;
  }
  else {
    cerr << endl 
	 << "*********************************************************" << endl
	 << "   M e t a H D L   C o m p i l e r   f i n i s h e d" << endl
	 << endl
	 << "    CPU Time: " << (double) time_used.tms_utime / CLK_PER_SEC << " sec" << endl
	 << " System Time: " << (double) time_used.tms_stime / CLK_PER_SEC << " sec" << endl
	 << "   User Time: " << end.tv_sec - start.tv_sec << " sec" << endl
	 << "     workdir: " << workdir << endl
	 << endl
	 << "*********************************************************" << endl;
  }

  return 0;
}
