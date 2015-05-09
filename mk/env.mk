#
# Architecture
#

ARCH := $(shell arch)

#
# Commands
#

AR       := ar -cq
CC       := gcc
CP       := cp -f
CXX      := g++
DOXYGEN  := doxygen
MAKE     := make
MKDIR    := mkdir -p
MV       := mv -f
PYCLEAN  := pyclean
PYTHON   := python
RM       := rm -rf

#
# General compiler/linker flags.
#

CXXFLAGS += -g -pedantic -Wall -Werror -Winline -Woverloaded-virtual -Wnon-virtual-dtor -O3 -std=c++14 -MMD -fPIC -flto -fdiagnostics-color=auto
CFLAGS   += -g -pedantic -Wall -Werror -Winline -O3 -MMD -fPIC -flto -fdiagnostics-color=auto
LDFLAGS  += -rdynamic -fPIC -flto


#
# Suffix rules
#

.SUFFIXES:
.SUFFIXES: .cpp .c .o

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

