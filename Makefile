# Makefile for LightStrinGraph

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/

ifneq ($(MAKECMDGOALS), clean)

ifdef USESTXXL
ifndef PAGE_SIZE
$(info PAGE_SIZE not defined. Forced to 4.)
PAGE_SIZE=4
endif

ifndef PAGE_NUM
$(info PAGE_NUM not defined. Forced to 4.)
PAGE_NUM=4
endif

ifndef BLOCK_SIZE
$(info BLOCK_SIZE not defined. Forced to 4 MiB.)
BLOCK_SIZE=4
endif

ifndef STXXLPATH
$(info STXXLPATH not defined. Forced to /usr.)
STXXLIB=/usr
endif
endif

ifndef BUFFERSIZE
$(info BUFFERSIZE not defined. Forced to 1024)
BUFFERSIZE=1024
endif

ifndef IM_BUFFERSIZE
$(info IM_BUFFERSIZE not defined. Forced to 1024)
IM_BUFFERSIZE=1024
endif
endif

ifndef OPTDEF
OPTDEF:=-O3 -DNDEBUG -march=native
$(info No custom optimization flags are found: using '${OPTDEF}')
endif

#####################
# TCMalloc detection
# TCMalloc is a (generally faster) drop-in replacement for the standard malloc
# See: http://code.google.com/p/gperftools/
#
HAS_TCMALLOC:=/$(shell echo "void main() {}" | $(CC) -x c -o /tmp/test - -ltcmalloc_minimal 2> /dev/null && echo yes || echo no)/
#####################

#####################
# zlib detection
# Used for reading the BWT file
#
HAS_ZLIB:=/$(shell echo "void main() {}" | $(CC) -x c -o /tmp/test - -lz 2> /dev/null && echo yes || echo no)/
#####################


DEFINES = -DBUFFERSIZE=${BUFFERSIZE} -DIM_BUFFERSIZE=${IM_BUFFERSIZE}

ifdef USESTXXL
DEFINES += -DPAGE_SIZE=${PAGE_SIZE} -DPAGE_NUM=${PAGE_NUM} -DBLOCK_SIZE=${BLOCK_SIZE}
STXXLLIB=-L${STXXLPATH}/lib -lstxxl -lpthread
STXXLINC=-I${STXXLPATH}/include
else
STXXLLIB=
STXXLINC=
endif

CFLAGS	= -g -Wall ${DEFINES} ${DEBUGDEF} ${OPTDEF} -Wno-deprecated -std=gnu++0x -I. -I$(SRC_DIR) #-fopenmp
CXXFLAGS= ${CFLAGS}
LIBS 	= #

ifeq ($(HAS_TCMALLOC), /yes/)
$(info Using TCMalloc)
LIBS+=-ltcmalloc_minimal
endif

ifeq ($(HAS_ZLIB), /yes/)
$(info Using zlib)
DEFINES+=-DHAS_ZLIB
LIBS+=-lz
endif


BGSA_DEP= $(OBJ_DIR)partialBWTReader.o \
	$(OBJ_DIR)BWTReader.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)joined_q_interval.o \
	$(OBJ_DIR)edge_joined_interval.o \
	$(OBJ_DIR)edgeLabelInterval.o \
	$(OBJ_DIR)bgsa/bgsa.o

LSG_DEP= $(OBJ_DIR)BWTReader.o \
	$(OBJ_DIR)partialLCPReader.o \
	$(OBJ_DIR)LCPReader.o \
	$(OBJ_DIR)LCPIterator.o \
	$(OBJ_DIR)GSAReader.o \
	$(OBJ_DIR)GSAIterator.o \
	$(OBJ_DIR)search.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)partialBWTReader.o \
	$(OBJ_DIR)edgeLabelIntervalManager.o \
	$(OBJ_DIR)extend_symbol_pile.o \
	$(OBJ_DIR)edgeLabelInterval.o \
	$(OBJ_DIR)lsg/lsg.o

.PHONY: all
all: lsg bgsa

${OBJ_DIR}%.o: $(SRC_DIR)%.cpp
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) ${STXXLINC} \
	-o $@ \
	-c $<

.PHONY: lsg
lsg: $(BIN_DIR)lsg
	@echo '* $@ OK!'

$(BIN_DIR)lsg: $(LSG_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) ${STXXLLIB}

.PHONY: bgsa
bgsa: $(BIN_DIR)bgsa
	@echo '* $@ OK!'

$(BIN_DIR)bgsa: $(BGSA_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) ${STXXLLIB}

clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
