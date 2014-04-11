# Makefile for LightStrinGraph

SRC_DIR	=src/
OBJ_DIR	=obj/
BIN_DIR	=bin/

CFLAGS	= -g -Wall -DDEBUG -O2 -march=native -Wno-deprecated
CXXFLAGS= ${CFLAGS}
LIBS 	= #
CFLAGS_EXTRA =  -m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
CFLAGS_DEBUG = -g  -lefence
CFLAGS_LIBS = `pkg-config --cflags glib-2.0` `pkg-config --cflags zlib`
LDLIBS = `pkg-config --libs glib-2.0` `pkg-config --libs zlib`

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

all: lsg bgsa find_triangles

${OBJ_DIR}%.o: $(SRC_DIR)%.cpp
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) \
	-o $@ \
	-c $< -I$(SRC_DIR)

${OBJ_DIR}%.o: $(SRC_DIR)%.c
	@echo '* Compiling $<'
	@mkdir -p $(dir $@)
	gcc  $(CFLAGS) $(CFLAGS_EXTRA) $(CFLAGS_DEBUG)   \
	-o $@ \
	-c $< -I$(SRC_DIR) $(CFLAGS_LIBS)

lsg: $(LSG_DEP)
	@echo 'Linking $@'
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) \
	-o $(BIN_DIR)$@	$^ $(LIBS) -I$(SRC_DIR)

bgsa: $(BGSA_DEP)
	@echo 'Linking $@'
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) \
	-o $(BIN_DIR)$@	$^ $(LIBS) -I$(SRC_DIR)

find_triangles: $(OBJ_DIR)find_triangles.o
	@echo 'Linking $@'
	@mkdir -p $(BIN_DIR)
	gcc  $(CFLAGS) $(CFLAGS_EXTRA) $(CFLAGS_DEBUG)  \
	-o $(BIN_DIR)$@ $^ -I$(SRC_DIR) $(LDLIBS)




clean:
	@echo "Cleaning..."
	rm -rf ${OBJ_DIR} ${BIN_DIR}
