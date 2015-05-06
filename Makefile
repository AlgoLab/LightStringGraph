# Makefile for LightStrinGraph

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/
THIRDPARTY_DIR	=thirdparty/

ifneq ($(MAKECMDGOALS), clean)

ifndef BUFFERED_FILE_BUFFERSIZE
$(info BUFFERED_FILE_BUFFERSIZE not defined. Forced to 8192)
BUFFERED_FILE_BUFFERSIZE=8192
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
# zlib configuration
# We need zlib >= 1.2.5, thus we ship the latest version (1.2.8)
#
ZLIB_VER=1.2.8
ZLIB_DIR=$(THIRDPARTY_DIR)zlib-$(ZLIB_VER)
#####################

#####################
# Boost iostreams detection
# Used for reading lexicographic permutation of the reads.
#
HAS_BOOST_IOSTREAMS_MT:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -lboost_iostreams-mt 2> /dev/null && echo yes || echo no)/
HAS_BOOST_IOSTREAMS:=/$(shell echo "void main() {}" | $(CC) -x c -o /dev/null - -lboost_iostreams 2> /dev/null && echo yes || echo no)/
#####################

DEFINES = -DBUFFERED_FILE_BUFFERSIZE=${BUFFERED_FILE_BUFFERSIZE} -DIM_BUFFERSIZE=${IM_BUFFERSIZE}

CFLAGS	= -g -Wall ${DEFINES} ${DEBUGDEF} ${OPTDEF} -Wno-deprecated -std=gnu++0x -I. -I$(SRC_DIR) -I$(ZLIB_DIR) #-fopenmp
CXXFLAGS= ${CFLAGS}
LIBS 	=

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


LSG_DEP= $(OBJ_DIR)search.o \
	$(OBJ_DIR)util.o \
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

$(ZLIB_DIR)/zlib.h: $(THIRDPARTY_DIR)/zlib-$(ZLIB_VER).tar.gz
	@echo '* Uncompressing zlib'; \
	cd $(THIRDPARTY_DIR); \
	tar xzf zlib-$(ZLIB_VER).tar.gz

$(ZLIB_DIR)/libz.a: $(ZLIB_DIR)/zlib.h
	@echo '* Building zlib'; \
	cd $(ZLIB_DIR); \
	./configure --static; \
	make libz.a

.PHONY: lsg
lsg: $(ZLIB_DIR)/libz.a $(BIN_DIR)lsg
	@echo '* $@ OK!'

$(BIN_DIR)lsg: $(LSG_DEP) $(ZLIB_DIR)/libz.a
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS)

.PHONY: redbuild
redbuild: $(ZLIB_DIR)/libz.a $(BIN_DIR)redbuild
	@echo '* $@ OK!'

$(BIN_DIR)redbuild: $(REDBUILD_DEP) $(ZLIB_DIR)/libz.a
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) $(LDFLAGS)

.PHONY: graph2asqg
graph2asqg: $(ZLIB_DIR)/libz.a $(BIN_DIR)graph2asqg
	@echo '* $@ OK!'

$(BIN_DIR)graph2asqg: $(GRAPH2ASQG_DEP) $(ZLIB_DIR)/libz.a
	@echo '* Linking $@'
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) \
	-o $@ $^ $(LIBS) $(LDFLAGS)

clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
