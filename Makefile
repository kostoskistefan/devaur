CC := /usr/bin/gcc
CFLAGS := -l git2 -Wall

C_SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

OBJS = $(addprefix ${BUILD_DIR}/,$(notdir $(C_SOURCES:.c=.o)))

BUILD_DIR := build

all: build_dir devaur
	chmod 755 ${BUILD_DIR}/devaur

build_dir:
	mkdir -p ${BUILD_DIR}

devaur: ${OBJS}
	${CC} ${OBJS} -o ${BUILD_DIR}/$@ ${CFLAGS}

${BUILD_DIR}/%.o: %.c ${HEADERS}
	${CC} -c $< -o $@
