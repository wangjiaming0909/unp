CC = g++
FLAGS = -Wall -Wextra -std=c++17 -c -ggdb3 -Wfatal-errors
#HEADERS = include/unp.h

DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD
DEFINES = $(patsubst %, -D%, $(DEFS))

SOURCEDIR = ./server
BUILDDIR = ./build
OBJDIR = $(BUILDDIR)/obj

MKDIR = mkdir -p
RM = rm -rf

SOURCES=$(shell find $(SOURCEDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES) )


TARGET = $(BUILDDIR)/a.out
LIBS = /boost_1_68_0/stage/lib
INCLUDES = /boost_1_68_0 -I /home/jwang284/codes/unp
# LDFLAGS = -pthread -lboost_thread -lboost_system
LDFLAGS = -pthread \
		/boost_1_68_0/stage/lib/libboost_thread.a \
		/boost_1_68_0/stage/lib/libboost_system.a \
		/usr/local/lib/libgtest.a \
		/usr/local/lib/libgmock.a
		

all: $(BUILDDIR) $(TARGET)

TEST_DIR = ./gtests
TEST_OBJ_DIR = $(TEST_DIR)
TEST_SOURCE = $(shell find $(TEST_DIR) -type f -name '*.cpp')
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SOURCE) )
TEST_TARGET = $(TEST_DIR)/test.out
TEST_USED_SOURCE = $(filter-out main.cpp, $(SOURCES))
TEST_USED_OBJECTS = $(filter-out ./build/obj/main.o, $(OBJECTS))

test: $(TEST_TARGET)


$(TEST_TARGET): $(TEST_USED_OBJECTS) $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(TEST_USED_OBJECTS) -L $(LIBS) $(LDFLAGS) -o $@ 

$(TEST_OBJS):$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp 
	$(CC) $(DEFINES) $(FLAGS) $< -I $(INCLUDES) -o $@

$(BUILDDIR):
	$(MKDIR) $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -L $(LIBS) $(LDFLAGS) -o $@
	@echo "OK......"

$(OBJECTS): $(OBJDIR)/%.o : $(SOURCEDIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CC) $(DEFINES) $(FLAGS) $< -I $(INCLUDES) -o $@

testclean:FORCE
	$(RM) $(TEST_OBJS) $(TEST_TARGET)

clean:FORCE
	$(RM) $(OBJECTS) $(TARGET)
	$(RM) $(TEST_OBJS) $(TEST_TARGET)
FORCE:
