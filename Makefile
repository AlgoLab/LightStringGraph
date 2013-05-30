#Makefile

SRC_DIR:=src
OBJ_DIR:=obj
BIN_DIR:=bin

CFLAGS+= -g -Wall -O2 -DDEBUG -march=native -Wno-deprecated
CXXFLAGS+= ${CFLAGS}
LIBS = #-l boost_graph

.PHONY: low_mem
low_mem: CXXFLAGS=${CFLAGS} -D LOW_MEM_USG
low_mem: all

.PHONY: all
all:action string_graph bgsa

string_graph: action read_input
build_gsa:action bgsa

.PHONY: action
action:
	@echo "Compiling..."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@echo '* Compiling $<'; \
	mkdir -pv $(dir $@) ; \
	$(CXX) $(CXXFLAGS) -o $@ -c $< -Isrc

read_input_OBJS= \
	${OBJ_DIR}/q_interval.o \
	${OBJ_DIR}/edge_joined_interval.o \
	${OBJ_DIR}/joined_q_interval.o \
	${OBJ_DIR}/util.o \
	${OBJ_DIR}/partialBWTReader.o \
	${OBJ_DIR}/BWTReader.o \
	${OBJ_DIR}/GSAReader.o \
	${OBJ_DIR}/search.o \
	${OBJ_DIR}/main.o

input_bgsa_OBJS= \
	${OBJ_DIR}/reconstruct_gsa.o \
	${OBJ_DIR}/partialBWTReader.o \
	${OBJ_DIR}/BWTReader.o \
	${OBJ_DIR}/util.o \
	${OBJ_DIR}/q_interval.o \
	${OBJ_DIR}/edge_joined_interval.o \
	${OBJ_DIR}/joined_q_interval.o

${BIN_DIR}/stringGraph: ${read_input_OBJS}
	@echo 'Linking $@'; \
	mkdir -p ${BIN_DIR}; \
	${CXX} ${CXXFLAGS} -o $@ $^ ${LIBS}

${BIN_DIR}/bgsa: ${input_bgsa_OBJS}
	@echo 'Linking $@'; \
	mkdir -p ${BIN_DIR}; \
	${CXX} ${CXXFLAGS} -o $@ $^ ${LIBS}

.PHONY: read_input
read_input: ${BIN_DIR}/stringGraph
bgsa: ${BIN_DIR}/bgsa

.PHONY: clean
clean:
	@echo "Cleaning..."; \
	rm -f ${OBJ_DIR}/* ${BIN_DIR}/*
