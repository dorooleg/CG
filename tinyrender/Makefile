PROJECT_NAME := tinyrender
CPP_COMPILER := clang++
CPP_FLAGS 	 := -MMD -pthread -Werror -Wall -std=c++1z -O3 -Itests -Isrc
BUILD_DIR 	 := bin
SRC_DIR 	   := src
TIDY         := clang-tidy-4.0
FORMAT       := clang-format

CPP_FILES 	 := $(wildcard ${SRC_DIR}/*.cpp)
H_FILES 	   := $(wildcard ${SRC_DIR}/*.h)
OBJ_FILES 	 := $(addprefix ${BUILD_DIR}/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES 	 := $(OBJ_FILES:.cc=.o)

all: mkdir format ${BUILD_DIR}/${PROJECT_NAME}

mkdir:
	mkdir -p ${BUILD_DIR}

clean:
	rm -r -- ${BUILD_DIR}
	rm output.tga

${BUILD_DIR}/${PROJECT_NAME}: ${OBJ_FILES}
	${CPP_COMPILER} ${CPP_FLAGS} -o $@ $^

${BUILD_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CPP_COMPILER} ${CPP_FLAGS} -c $< -o $@


format: ${CPP_FILES} ${H_FILES}
	$(foreach f, $^, ${FORMAT} -i -style=Chromium $(f);)

tidy: ${CPP_FILES}
	$(foreach f, $^, echo $(f); ${TIDY} $(f) -checks=*,-clang-analyzer-alpha.*,-llvm-include-order,clang-analyzer-*,-clang-analyzer-cplusplus* -- ${CPP_FLAGS};)

-include ${BUILD_DIR}/*.d

.PHONY: clean all mkdir format tidy
