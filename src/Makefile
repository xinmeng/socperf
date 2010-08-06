CXX  = g++

SYSC_INC_DIR  = -I/home/xin_meng/src/include
SYSC_LIB_DIR  = -L/home/xin_meng/tools/lib-linux

BOOST_INC_DIR = -I/home/xin_meng/tools/include/boost-1_38

DTL_INC_DIR   = -I/usr/local/include/dtl
DTL_LIB_DIR   = -L/usr/local/lib/dtl

INCDIR = $(SYSC_INC_DIR) $(BOOST_INC_DIR) $(DTL_INC_DIR)
LIBDIR = $(SYSC_LIB_DIR) $(DTL_LIB_DIR)


DEBUG = 1
ifeq ($(DEBUG), 0)
OPT = -O3
else 
OPT = -gdwarf-2 -g3 -D__DEBUG_VERSION__
endif

Wall = 0 
ifeq ($(Wall), 1)
CPPFLAGS = -Wall
endif

CPPFLAGS += $(OPT) $(INCDIR) $(PROFLAGS)
LIBFLAGS += $(LIBDIR) -lsystemc -lDTL -lodbc $(PROFLAGS)

MAIN     = main.cc
MAIN_OBJ = main.o

SRC  = $(filter-out $(MAIN),$(wildcard *.cc))
OBJ  = $(SRC:.cc=.o)

HEADER = $(filter-out quick_def.hh,$(wildcard *.hh))

LIB  = ../lib/libmperf.a
INC  = $(foreach header,$(HEADER),../include/$(header))

GPROF = 
PROFLAGS = $(if $(GPROF),-pg)

DOT = $(wildcard *.dot)
JPG = $(DOT:.dot=.jpg)

EXEC = simpm


.phony: all exe lib jpg clean_all clean clean_jpg clean_log  test

all : exe

exe : $(EXEC)

$(EXEC) : $(MAIN_OBJ) $(OBJ)
	$(CXX) -o $@ $^ $(LIBFLAGS)


lib : $(LIB) $(INC) 


$(LIB) : $(OBJ)
	ar rcv $@ $^

../include/%.hh : %.hh
	cp $< $@


.depend : $(SRC) $(MAIN)
	$(CXX) -MM $(CPPFLAGS) $^ > .depend

def.hh : quick_def.hh
	$(CXX) -E $(CPPFLAGS) $< -o $@ 

jpg : $(JPG)


%.jpg : %.dot
	@echo -n "Create graph \""$@"\"..."
	@/home/xin_meng/tools/bin/dot -Tjpg -o$@ $^
	@echo "Done!"

clean_all : clean 
	@rm -fv $(LIB) $(INC) BROWSE TAGS

clean: clean_log clean_jpg
	@rm -fv $(EXEC) $(OBJ) $(MAIN_OBJ) .depend gmon.out

clean_jpg: 
	@rm -fv *.jpg *.dot 

clean_log: 
	@rm -fv *.log

test : 
	#echo $(OBJ)
	echo $(DOT)
	echo $(JPG)



-include .depend
