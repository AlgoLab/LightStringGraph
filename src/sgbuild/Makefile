# Makefile for sgBuild

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/

CFLAGS	= -g -Wall ${DEFINES} ${DEBUGDEF} -O0 -march=native -Wno-deprecated -std=gnu++0x -I. -I$(SRC_DIR) -fopenmp
CXXFLAGS= ${CFLAGS}
LIBS 	= #

SGBUILD_DEP= $(OBJ_DIR)sgbuild/sgbuild.o \
	$(OBJ_DIR)edgeLabelInterval.o    \
	$(OBJ_DIR)q_interval.o

.PHONY: all
all: sgbuild

${OBJ_DIR}%.o: $(SRC_DIR)%.cpp
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) ${STXXLINC} \
	-o $@ \
	-c $<

.PHONY: sgbuild
sgbuild: $(BIN_DIR)sgbuild
	@echo '* $@ OK!'

$(BIN_DIR)sgbuild: $(SGBUILD_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS)

clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
