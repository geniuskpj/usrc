CC := g++

EXTRAINCDIRS += -I../ 

CFLAGS += -I include/ ${EXTRAINCDIRS}

LIB = lib/libAPIs.a

.PHONY: all
all: ${LIB}

.PHONY: clean
clean:
	rm -rf lib/* obj/*.o 

MYOBJS = obj/SharedMemModule.o obj/CommandQueue.o obj/SendReceiveThreads.o

${MYOBJS} : ${wildcard include/*.h}

${LIB} : ${MYOBJS}
	ar rvs $@ ${MYOBJS}

obj/%.o : src/%.cpp
	${CC} -c ${CFLAGS} $< -o $@

obj : 
	mkdir -p $@

lib :
	mkdir -p $@
 
	 
