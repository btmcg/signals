#
# Boilerplate binary makefile.
#

NAME        ?= $(shell basename $(shell pwd))
TARGET      ?= $(NAME)
C_SRC       ?= $(wildcard *.c)
CXX_SRC     ?= $(wildcard *.cpp)
OBJ         ?= $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o)
INSTALL_DIR ?= $(BUILD_ROOT)/bin


all:: $(TARGET)

clean::
	$(RM) $(OBJ) $(TARGET) *.d

distclean::
	$(RM) $(INSTALL_DIR)/*

install:: all
	$(CP) $(TARGET) $(INSTALL_DIR)

$(TARGET):: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

-include *.d
