CAN_BAUDRATE := 500000
CC := g++


EXTRAINCDIRS += -I/usr/xenomai/include -I./APIs/include -I./
EXTRALDFLAGS += -L/usr/xenomai/lib -L./APIs/lib
EXTRALIBS += -lAPIs -lpcan -lpthread -lrtdm -lnative -lxenomai -lrt 

CFLAGS += -D CAN_BAUDRATE=${CAN_BAUDRATE} ${EXTRAINCDIRS}

.PHONY: all	
all: bin/IMU_test bin/elmo_test_kpj

.PHONY: clean
clean:
	rm -rf bin/* obj/*.o

MYOBJS = obj/Device.o obj/Comm.o obj/ElmoDriver.o

${MYOBJS}: ${wildcard *.h}
 
bin/test: obj/test.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

bin/serial_test: obj/serial_test.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

bin/IMU_test: obj/IMU_test.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

bin/abs_encoder_test: obj/abs_encoder_test.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}
	
bin/elmo_driver_test: obj/elmo_driver_test.o ${MYOBJS} 
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

bin/elmo_test_kpj: obj/elmo_test_kpj.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

bin/FT_Sensor_test: obj/FT_Sensor_test.o ${MYOBJS}
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS}

obj/%.o: %.cpp
	${CC} -c $< ${CFLAGS} -o $@



