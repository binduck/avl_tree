CC=gcc
CFLAGS=-O3
LFLAGS=-O3
BUILD_DIR=build/

PLATFORM=$(shell uname)

# msys build configuration
ifneq (,$(findstring MSYS,$(PLATFORM)))
    $(info using MSYS build)
    TARGET=avl.dll
    CC=gcc
    LFLAGS=-m64 -shared -Wl,--subsystem,windows
    CFLAGS=-m64 -DMS_WIN64 -D ADD_EXPORTS -c
# linux build configuration
else ifneq (,$(findstring LINUX,$(PLATFORM)))
    $(info using UNIX build)
    TARGET=avl.so
    CC=gcc
# unknown configuration
else
    $(error uname gave unexpected result. maybe your platform isn\'t supported.)
endif

test: ${BUILD_DIR}${TARGET} tests.py
	python tests.py

${BUILD_DIR}${TARGET}: ${BUILD_DIR}avl.o
	${CC} $< ${LFLAGS} -o $@

${BUILD_DIR}avl.o: avl.c avl.h
	${CC} ${CFLAGS} $< -o $@

clean:
	rm ${BUILD_DIR}*
