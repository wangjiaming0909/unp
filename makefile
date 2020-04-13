CC = g++
FLAGS = -Wall -Wextra -std=c++17 -c -ggdb -Wfatal-errors -O0
#HEADERS = include/unp.h

DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD
TESTDEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING
#DEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
#TESTDEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
DEFINES = $(patsubst %, -D%, $(DEFS))
TESTDEFINES = $(patsubst %, -D%, $(TESTDEFS))

SOURCEDIR = ./src
BUILDDIR = ./build
OBJDIR = $(BUILDDIR)/obj

MKDIR = mkdir -p
RM = rm -rf

SOURCES = $(shell find $(SOURCEDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES) )


TARGET = $(BUILDDIR)/a.out
LIBS = /usr/local/lib
INCLUDES = $(SOURCEDIR) -I /usr/local/include -I /boost_1_72_0/ -I /usr/include 
# /boost_1_70_0  
# LDFLAGS = -pthread -lboost_thread -lboost_system

# /boost_1_70_0/stage/lib/libboost_thread.a 

#/usr/local/lib/libboost_system.so.1.72.0 \
#/usr/local/lib/libboost_timer.so.1.72.0 \
#/usr/local/lib/libboost_chrono.so.1.72.0 \

TESTLDFLAG = -pthread \
		/boost_1_72_0/stage/lib/libboost_system.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_timer.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_chrono.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_filesystem.so.1.72.0 \
		-lgtest\
		-lgmock\
		-lssl \
		-lcrypto\
		/usr/local/lib/libprotobuf.so.22
	

# /boost_1_70_0/stage/lib/libboost_thread.a 
#/usr/local/lib/libboost_timer.so.1.72.0 \
#/usr/local/lib/libboost_chrono.so.1.72.0 \
#/usr/local/lib/libboost_system.so.1.72.0 \

LDFLAGS = -pthread \
		/boost_1_72_0/stage/lib/libboost_system.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_timer.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_chrono.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_filesystem.so.1.72.0 \
		-lssl \
		-lcrypto\
		/usr/local/lib/libprotobuf.so.22
		

all: $(BUILDDIR) $(TARGET) $(SYNC_CLIENT_TARGET)

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

SYNC_CLIENT_DIR = ./examples/syncclient
SYNC_CLIENT_OBJ_DIR = $(BUILDDIR)/examples/sync_client
SYNC_CLIENT_SOURCE = $(shell find $(SYNC_CLIENT_DIR) -type f -name '*.cpp')
SYNC_CLIENT_OBJS = $(patsubst $(SYNC_CLIENT_DIR)/%.cpp, $(SYNC_CLIENT_OBJ_DIR)/%.o, $(SYNC_CLIENT_SOURCE))
SYNC_CLIENT_TARGET = $(SYNC_CLIENT_OBJ_DIR)/sync_client
SYNC_CLIENT_USED_SOURCE = $(filter-out main.cpp, $(SOURCES))
SYNC_CLIENT_USED_OBJECTS = $(filter-out ./build/obj/main.o, $(OBJECTS))

sync_client: $(SYNC_CLIENT_TARGET)

$(SYNC_CLIENT_TARGET): $(SYNC_CLIENT_USED_OBJECTS) $(SYNC_CLIENT_OBJS)
	$(CC) $(SYNC_CLIENT_OBJS) $(SYNC_CLIENT_USED_OBJECTS) -L $(LIBS) $(LDFLAGS) -o $@
	@echo "ok ....."

$(SYNC_CLIENT_OBJS): $(SYNC_CLIENT_OBJ_DIR)/%.o: $(SYNC_CLIENT_DIR)/%.cpp
	$(CC) $(DEFINES) $(FLAGS) $< -I $(INCLUDES) -I examples -o $@

#sync server
SYNC_SERVER_DIR = ./examples/syncserver
SYNC_SERVER_OBJ_DIR = $(BUILDDIR)/examples/sync_server
SYNC_SERVER_SOURCE = $(shell find $(SYNC_SERVER_DIR) -type f -name '*.cpp')
SYNC_SERVER_OBJS = $(patsubst $(SYNC_SERVER_DIR)/%.cpp, $(SYNC_SERVER_OBJ_DIR)/%.o, $(SYNC_SERVER_SOURCE))
SYNC_SERVER_TARGET = $(SYNC_SERVER_OBJ_DIR)/sync_server
SYNC_SERVER_USED_SOURCE = $(filter-out main.cpp, $(SOURCES))
SYNC_SERVER_USED_OBJECTS = $(filter-out ./build/obj/main.o, $(OBJECTS))

sync_server: $(SYNC_SERVER_TARGET)

examples:  $(SYNC_CLIENT_OBJ_DIR) $(SYNC_SERVER_OBJ_DIR) $(SYNC_SERVER_TARGET) $(SYNC_CLIENT_TARGET)

$(SYNC_CLIENT_OBJ_DIR):
	$(MKDIR) $@

$(SYNC_SERVER_OBJ_DIR):
	$(MKDIR) $@

$(SYNC_SERVER_TARGET): $(SYNC_SERVER_USED_OBJECTS) $(SYNC_SERVER_OBJS)
	$(CC) $(SYNC_SERVER_OBJS) $(SYNC_SERVER_USED_OBJECTS) -L $(LIBS) $(LDFLAGS) -o $@
	@echo "ok ....."

$(SYNC_SERVER_OBJS): $(SYNC_SERVER_OBJ_DIR)/%.o: $(SYNC_SERVER_DIR)/%.cpp
	$(CC) $(DEFINES) $(FLAGS) $< -I $(INCLUDES) -I examples -o $@

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

exampleclean:FORCE
	$(RM) $(SYNC_CLIENT_OBJS) $(SYNC_CLIENT_TARGET)
	$(RM) $(SYNC_SERVER_OBJS) $(SYNC_SERVER_TARGET)


FORCE:
