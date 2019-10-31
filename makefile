CC = g++
FLAGS = -Wall -Wextra -std=c++17 -c -ggdb -Wfatal-errors -O0 
#HEADERS = include/unp.h

DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD
TESTDEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING
# DEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
# TESTDEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
DEFINES = $(patsubst %, -D%, $(DEFS))
TESTDEFINES = $(patsubst %, -D%, $(TESTDEFS))

SOURCEDIR = ./src
BUILDDIR = ./build
OBJDIR = $(BUILDDIR)/obj

MKDIR = mkdir -p
RM = rm -rf

SOURCES=$(shell find $(SOURCEDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES) )


TARGET = $(BUILDDIR)/a.out
LIBS = /boost_1_70_0/stage/lib
INCLUDES = /boost_1_70_0 -I $(SOURCEDIR)
# LDFLAGS = -pthread -lboost_thread -lboost_system

# /boost_1_70_0/stage/lib/libboost_thread.a 
TESTLDFLAG =  -pthread \
		/boost_1_70_0/stage/lib/libboost_system.so.1.70.0 \
		/boost_1_70_0/stage/lib/libboost_timer.so.1.70.0 \
		/boost_1_70_0/stage/lib/libboost_chrono.so.1.70.0 \
		/usr/local/lib/libgtest.a \
		/usr/local/lib/libgmock.a \
		-lssl \
		-lcrypto

# /boost_1_70_0/stage/lib/libboost_thread.a 
LDFLAGS = -pthread \
		/boost_1_70_0/stage/lib/libboost_timer.so.1.70.0 \
		/boost_1_70_0/stage/lib/libboost_chrono.so.1.70.0 \
		/boost_1_70_0/stage/lib/libboost_system.so.1.70.0 \
		-lssl \
		-lcrypto
		

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
	$(CC) $(TEST_OBJS) $(TEST_USED_OBJECTS) -L $(LIBS) $(TESTLDFLAG) -o $@ 

$(TEST_OBJS):$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp 
	$(CC) $(TESTDEFINES) $(FLAGS) $< -I $(INCLUDES) -o $@

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
