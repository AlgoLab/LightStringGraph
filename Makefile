#Makefile

SRC_DIR:=src
OBJ_DIR:=obj
BIN_DIR:=bin

CFLAGS+= -g -Wall -O2 -UNDEBUG -march=native -Wno-deprecated
CXXFLAGS+= ${CFLAGS}
LIBS = #-l boost_graph

.PHONY: low_mem
low_mem: CXXFLAGS=${CFLAGS} -D LOW_MEM_USG
low_mem: all

.PHONY: all
all:action read_input

.PHONY: action
action:
	@echo "Compiling..."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@echo '* Compiling $<'; \
	mkdir -pv $(dir $@) ; \
	$(CXX) $(CXXFLAGS) -o $@ -c $< -Isrc

read_input_OBJS= \
	${OBJ_DIR}/main.o \
	${OBJ_DIR}/q_interval.o \
	${OBJ_DIR}/joined_q_interval.o

${BIN_DIR}/build_RNA_seq_graph: ${read_input_OBJS}
	@echo 'Linking $@'; \
	mkdir -p ${BIN_DIR}; \
	${CXX} ${CXXFLAGS} -o $@ $^ ${LIBS}

.PHONY: read_input
read_input: ${BIN_DIR}/build_RNA_seq_graph

.PHONY: clean
clean:
	@echo "Cleaning..."; \
	rm -f ${OBJ_DIR}/* ${BIN_DIR}/*
