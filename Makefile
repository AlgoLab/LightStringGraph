# Makefile for LightStrinGraph

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/

STXXLLIB=-L${STXXLPATH}/lib -lstxxl -lpthread
STXXLINC=-I${STXXLPATH}/include
CFLAGS	= -g -Wall -DDEBUG -O2 -march=native -Wno-deprecated -std=gnu++0x -I. -fopenmp
CXXFLAGS= ${CFLAGS}
LIBS 	= #

# BGSA_EX	= bgsa/bgsa
# BGSA_CPP= bgsa/bgsa.cpp
BGSA_DEP= $(OBJ_DIR)partialBWTReader.o \
	$(OBJ_DIR)BWTReader.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)q_interval.o \
	$(OBJ_DIR)joined_q_interval.o \
	$(OBJ_DIR)edge_joined_interval.o \
	$(OBJ_DIR)bgsa/bgsa.o

# LSG_EX	= lsg/lsg
# LSG_CPP	= lsg/lsg.cpp
LSG_DEP= $(OBJ_DIR)BWTReader.o \
	$(OBJ_DIR)GSAReader.o \
	$(OBJ_DIR)joined_q_interval.o \
	$(OBJ_DIR)search.o \
	$(OBJ_DIR)ext_sort.o \
	$(OBJ_DIR)edge_joined_interval.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)partialBWTReader.o \
	$(OBJ_DIR)q_interval.o \
	$(OBJ_DIR)lsg/lsg.o

all: lsg bgsa

${OBJ_DIR}%.o: $(SRC_DIR)%.cpp
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) ${STXXLINC} \
	-o $@ \
	-c $< -I$(SRC_DIR)

lsg: $(LSG_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $(BIN_DIR)$@	$^ $(LIBS) -I$(SRC_DIR)  ${STXXLLIB}

bgsa: $(BGSA_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $(BIN_DIR)$@	$^ $(LIBS) -I$(SRC_DIR) ${STXXLLIB}

clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
