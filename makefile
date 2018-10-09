CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
#HEADERS = include/unp.h

DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE
DEFINES = $(patsubst %, -D%, $(DEFS))

SOURCEDIR = ./server
BUILDDIR = ./build
OBJDIR = $(BUILDDIR)/obj

MKDIR = mkdir -p
RM = rm -rf

SOURCES=$(shell find $(SOURCEDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES) )


TARGET = $(BUILDDIR)/a.out
LIBS = 
INCLUDES = /boost_1_68_0
LDFLAGS = 

all: $(BUILDDIR) $(TARGET)

TEST_DIR = ./tests
TEST_OBJ_DIR = $(TEST_DIR)
TEST_SOURCE = $(shell find $(TEST_DIR) -type f -name '*.cpp')
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SOURCE) )
TEST_TARGET = $(TEST_DIR)/test.out
TEST_USED_SOURCE = $(filter-out main.cpp, $(SOURCES))
TEST_USED_OBJECTS = $(filter-out ./build/obj/main.o, $(OBJECTS))

test: $(TEST_TARGET)


$(TEST_TARGET): $(TEST_USED_OBJECTS) $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(TEST_USED_OBJECTS) -o $@ 

$(TEST_OBJS):$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp 
	$(CC) $(DEFINES) $(FLAGS) $< -o $@ -I $(INCLUDES)

$(BUILDDIR):
	$(MKDIR) $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ 

$(OBJECTS): $(OBJDIR)/%.o : $(SOURCEDIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CC) $(DEFINES) $(FLAGS) $< -o $@ -I $(INCLUDES)

testclean:FORCE
	$(RM) $(TEST_OBJS) $(TEST_TARGET)

clean:FORCE
	$(RM) $(OBJECTS) $(TARGET)
	$(RM) $(TEST_OBJS) $(TEST_TARGET)
FORCE:
