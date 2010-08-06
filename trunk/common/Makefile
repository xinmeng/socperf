


.phony : clean_link clean_all 

simpm:  | Makefile.in


Makefile.in : 
	find $(PM_SRC_DIR)/src/ -name "Makefile" -exec ln -s {} Makefile.in \;
	find $(PM_SRC_DIR)/src/ -name "*.hh" -exec ln -s {} \;
	find $(PM_SRC_DIR)/src/ -name "*.cc" -exec ln -s {} \;

clean_link: 
	find . -name "*.hh" -exec unlink {} \;
	find . -name "*.cc" -exec unlink {} \;
	find . -name "Makefile.in" -exec unlink {} \;

clean_all : clean_link

-include Makefile.in
