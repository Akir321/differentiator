CXX = g++
CXX_FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline\
 -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default\
 -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy\
 -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op\
 -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual\
 -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing\
 -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG\
 -D_EJUDGE_CLIENT_SIDE 

SRC_DIR  = source/
OBJ_DIR  = object/
DMP_DIR  = gr_dump/
LOG_DIR  = log/

INCLUDES =  $(SRC_DIR)tree_of_expressions.h \
			$(SRC_DIR)tree_graphic_dump.h   \
			$(SRC_DIR)html_logfile.h        \
			$(SRC_DIR)exp_tree_write.h      \
			$(SRC_DIR)exp_tree_read.h

OBJECTS  =  $(OBJ_DIR)test_exp_tree.o       \
		    $(OBJ_DIR)tree_of_expressions.o \
			$(OBJ_DIR)tree_graphic_dump.o   \
			$(OBJ_DIR)html_logfile.o        \
			$(OBJ_DIR)exp_tree_write.o      \
			$(OBJ_DIR)exp_tree_read.o

DUMPS    =  $(DMP_DIR)*.dot                 \
			$(DMP_DIR)*.png
         

all: test_exp_tree.exe

test_exp_tree.exe: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(CXX_FLAGS)


$(OBJ_DIR)test_exp_tree.o:       $(SRC_DIR)test_exp_tree.cpp         $(INCLUDES) 
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR)tree_of_expressions.o: $(SRC_DIR)tree_of_expressions.cpp   $(INCLUDES) 
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR)tree_graphic_dump.o:   $(SRC_DIR)tree_graphic_dump.cpp     $(INCLUDES)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR)html_logfile.o: $(SRC_DIR)html_logfile.cpp $(INCLUDES)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR)exp_tree_write.o: $(SRC_DIR)exp_tree_write.cpp $(INCLUDES)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

$(OBJ_DIR)exp_tree_read.o: $(SRC_DIR)exp_tree_read.cpp $(INCLUDES)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)



.PHONY: clean clean_dumps clean_logs

clean:
	rm $(OBJECTS) *.exe 
	
clean_dumps:
	rm $(DUMPS)

clean_logs:
	rm $(LOG_DIR)*.html
