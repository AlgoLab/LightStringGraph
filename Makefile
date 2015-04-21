# Makefile for LightStrinGraph

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/

ifneq ($(MAKECMDGOALS), clean)

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
ifeq ($(origin HAS_TCMALLOC), undefined)
HAS_TCMALLOC:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -ltcmalloc_minimal 2> /dev/null && echo yes || echo no)/
endif
#####################

#####################
# zlib detection
# Used for reading the BWT file
#
HAS_ZLIB:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -lz 2> /dev/null && echo yes || echo no)/
ifneq ($(HAS_ZLIB), /yes/)
$(error zlib not found)
endif
#####################

#####################
# Boost iostreams detection
# Used for reading lexicographic permutation of the reads.
#
HAS_BOOST_IOSTREAMS_MT:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -lboost_iostreams-mt 2> /dev/null && echo yes || echo no)/
HAS_BOOST_IOSTREAMS:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -lboost_iostreams 2> /dev/null && echo yes || echo no)/
#####################


DEFINES = -DBUFFERSIZE=${BUFFERSIZE} -DIM_BUFFERSIZE=${IM_BUFFERSIZE}

CFLAGS	= -g -Wall ${DEFINES} ${DEBUGDEF} ${OPTDEF} -Wno-deprecated -std=gnu++0x -I. -I$(SRC_DIR) #-fopenmp
CXXFLAGS= ${CFLAGS}
LIBS 	= -lz

ifeq ($(HAS_TCMALLOC), /yes/)
$(info Using TCMalloc)
LIBS+=-ltcmalloc_minimal
endif

ifeq ($(HAS_BOOST_IOSTREAMS), /yes/)
$(info Using Boost iostreams)
DEFINES+=-DUSE_BOOST_IOSTREAMS
ifeq ($(HAS_BOOST_IOSTREAMS_MT), /yes/)
LIBS+=-lboost_iostreams-mt
else
LIBS+=-lboost_iostreams
endif
endif


LSG_DEP= $(OBJ_DIR)BWTReader.o \
	$(OBJ_DIR)LCPIterator.o \
	$(OBJ_DIR)GSAIterator.o \
	$(OBJ_DIR)search.o \
	$(OBJ_DIR)util.o \
	$(OBJ_DIR)partialBWTReader.o \
	$(OBJ_DIR)edgeLabelIntervalManager.o \
	$(OBJ_DIR)extend_symbol_pile.o \
	$(OBJ_DIR)lsg/lsg.o

REDBUILD_DEP= $(OBJ_DIR)sgbuild/redbuild.o

GRAPH2ASQG_DEP= $(OBJ_DIR)sgbuild/graph2asqg.o \
	$(OBJ_DIR)sgbuild/asqg_fmt.o

.PHONY: all
all: lsg redbuild graph2asqg

${OBJ_DIR}%.o: $(SRC_DIR)%.cpp
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) \
	-o $@ \
	-c $<

.PHONY: lsg
lsg: $(BIN_DIR)lsg
	@echo '* $@ OK!'

$(BIN_DIR)lsg: $(LSG_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS)

.PHONY: redbuild
redbuild: $(BIN_DIR)redbuild
	@echo '* $@ OK!'

$(BIN_DIR)redbuild: $(REDBUILD_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) $(LDFLAGS)

.PHONY: graph2asqg
graph2asqg: $(BIN_DIR)graph2asqg
	@echo '* $@ OK!'

$(BIN_DIR)graph2asqg: $(GRAPH2ASQG_DEP)
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) $(LDFLAGS)

clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
