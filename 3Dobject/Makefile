PROJECT_NAME := scene
CPP_COMPILER := clang++
C_COMPILER   := clang
CPP_FLAGS 	 := -MMD -Werror -Wall -std=c++14 -O3 -Itests -I3Dobject/headers
C_FLAGS 	   := -MMD -Werror -Wall -O3 -I3Dobject/headers
BUILD_DIR 	 := Debug
SRC_DIR 	   := 3Dobject/src

CPP_FILES 	 := $(wildcard ${SRC_DIR}/*.cpp)
OBJ_FILES 	 := $(addprefix ${BUILD_DIR}/,$(notdir $(CPP_FILES:.cpp=.o)))
C_FILES 	   := $(wildcard ${SRC_DIR}/*.c)
OBJ_FILES    += $(addprefix ${BUILD_DIR}/,$(notdir $(C_FILES:.c=.o)))
OBJ_FILES 	 := $(OBJ_FILES:.cc=.o)


all: mkdir ${BUILD_DIR}/${PROJECT_NAME}

mkdir :
	mkdir -p ${BUILD_DIR}

clean:
	rm -r -- ${BUILD_DIR}

${BUILD_DIR}/${PROJECT_NAME}: ${OBJ_FILES}
	${CPP_COMPILER} ${CPP_FLAGS} -lglfw -ldl -o $@ $^

${BUILD_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CPP_COMPILER} ${CPP_FLAGS} -c $< -o $@

${BUILD_DIR}/%.o: ${SRC_DIR}/%.c
	${C_COMPILER} ${C_FLAGS} -c $< -o $@

-include ${BUILD_DIR}/*.d

.PHONY: clean all mkdir
