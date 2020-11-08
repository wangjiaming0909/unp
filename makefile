CC = g++
BUILD_FLAGS = -Wall -Wextra -std=c++17 -c -ggdb -Wfatal-errors -O0 -fPIC
DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING#ELPP_DISABLE_DEBUG_LOGS
TESTDEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING
#DEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
#TESTDEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING

DEFINES = $(patsubst %, -D%, $(DEFS))
TESTDEFINES = $(patsubst %, -D%, $(TESTDEFS))
LIBS = /usr/local/lib

LDFLAGS = -pthread \
		/boost_1_72_0/stage/lib/libboost_system.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_timer.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_chrono.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_filesystem.so.1.72.0 \
		-lssl \
		-lcrypto\
		-lprotobuf\
		#-luv
		

TESTLDFLAG = -pthread \
		/boost_1_72_0/stage/lib/libboost_system.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_timer.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_chrono.so.1.72.0 \
		/boost_1_72_0/stage/lib/libboost_filesystem.so.1.72.0 \
		-lgtest\
		-lgmock\
		-lssl \
		-lcrypto\
		-lprotobuf\
		-luv
	

UNP_SOURCEDIR = src
BUILDDIR = build
UNP_OBJ_DIR = $(BUILDDIR)/unp

MKDIR = mkdir -p
RM = rm -rf

INCLUDES = $(UNP_SOURCEDIR) -I /usr/local/include -I /boost_1_72_0/ -I /usr/include

UNP_SOURCES = $(shell ./scripts/protoc.sh && find $(UNP_SOURCEDIR) -type f -name '*.cpp')
UNP_HEADERS = $(shell find $(UNP_SOURCEDIR) -type -f -name '*.h')
UNP_OBJECTS = $(patsubst $(UNP_SOURCEDIR)/%.cpp, $(UNP_OBJ_DIR)/%.o, $(UNP_SOURCES) )
UNP_CORE_SOURCES = $(filter-out ./src/main.cpp, $(UNP_SOURCES));
UNP_CORE_OBJECTS = $(filter-out ./build/obj/main.o, $(UNP_OBJECTS))
UNP_CORE = $(BUILDDIR)/unp_core.so

MAIN = $(BUILDDIR)/a.out

TEST_DIR = ./gtests
TEST_OBJ_DIR = $(BUILDDIR)/gtests
TEST_SOURCE = $(shell find $(TEST_DIR) -type f -name '*.cpp')
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SOURCE) )
TEST_TARGET = $(BUILDDIR)/test.out

SYNC_CLIENT_DIR = ./examples/syncclient
SYNC_CLIENT_OBJ_DIR = $(BUILDDIR)/examples/sync_client
SYNC_CLIENT_SOURCE = $(shell find $(SYNC_CLIENT_DIR) -type f -name '*.cpp')
SYNC_CLIENT_OBJS = $(patsubst $(SYNC_CLIENT_DIR)/%.cpp, $(SYNC_CLIENT_OBJ_DIR)/%.o, $(SYNC_CLIENT_SOURCE))
SYNC_CLIENT_MAIN_OBJ = ./build/examples/sync_client/main.o
SYNC_CLIENT_MAIN_SOURCE = $(SYNC_CLIENT_DIR)/main.cpp
SYNC_CLIENT_CORE_OBJS = $(filter-out $(SYNC_CLIENT_MAIN_OBJ), $(SYNC_CLIENT_OBJS))
SYNC_CLIENT_CORE = $(BUILDDIR)/sync_client_core.so
SYNC_CLIENT_TARGET = $(BUILDDIR)/sync_client

SYNC_SERVER_DIR = ./examples/syncserver
SYNC_SERVER_OBJ_DIR = $(BUILDDIR)/examples/sync_server
SYNC_SERVER_SOURCE = $(shell find $(SYNC_SERVER_DIR) -type f -name '*.cpp')
SYNC_SERVER_OBJS = $(patsubst $(SYNC_SERVER_DIR)/%.cpp, $(SYNC_SERVER_OBJ_DIR)/%.o, $(SYNC_SERVER_SOURCE))
SYNC_SERVER_MAIN_OBJ = ./build/examples/sync_server/main.o
SYNC_SERVER_CORE_OBJS = $(filter-out $(SYNC_SERVER_MAIN_OBJ), $(SYNC_SERVER_OBJS))
SYNC_SERVER_CORE = $(BUILDDIR)/sync_server_core.so
SYNC_SERVER_TARGET = $(BUILDDIR)/sync_server

all: $(MAIN) $(SYNC_CLIENT_TARGET) $(SYNC_SERVER_TARGET) $(TEST_TARGET)

examples: $(SYNC_CLIENT_TARGET) $(SYNC_SERVER_TARGET)

sync_client: $(SYNC_CLIENT_TARGET)

sync_server: $(SYNC_SERVER_TARGET)

sync_server_core: $(SYNC_SERVER_CORE)

sync_client_core: $(SYNC_CLIENT_CORE)

unp_core: $(UNP_CORE)

unp: $(MAIN)

test: $(TEST_TARGET)

sync_client_main: $(SYNC_CLIENT_MAIN_OBJ)

$(MAIN): $(UNP_CORE)
	$(CC) $(FLAGS) $(DEFINES) $< ./build/unp/main.o -L $(LIBS) $(LDFLAGS) -o $@

$(TEST_TARGET): $(TEST_OBJS) $(UNP_CORE) $(SYNC_CLIENT_CORE)
	$(CC) $(TEST_OBJS) $(UNP_CORE) $(SYNC_CLIENT_CORE) -L $(LIBS) $(TESTLDFLAG) -o $@

$(SYNC_CLIENT_TARGET): $(UNP_CORE) $(SYNC_CLIENT_CORE) $(SYNC_CLIENT_MAIN_OBJ)
	$(CC) $(FLAGS)  $(SYNC_CLIENT_MAIN_OBJ) $(SYNC_CLIENT_CORE) $(UNP_CORE) -L $(LIBS) $(LDFLAGS) -o $@

$(SYNC_CLIENT_CORE): $(SYNC_CLIENT_CORE_OBJS)
	$(CC) -shared $(SYNC_CLIENT_CORE_OBJS) -o $@

$(SYNC_CLIENT_CORE_OBJS): $(SYNC_CLIENT_OBJ_DIR)/%.o : $(SYNC_CLIENT_DIR)/%.cpp
	@$(MKDIR) $(dir $(SYNC_CLIENT_CORE_OBJS))
	$(CC) $(DEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -I examples -o $@

#$(SYNC_CLIENT_MAIN_OBJ): $(SYNC_CLIENT_MAIN_SOURCE)
#$(CC) $(DEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -I examples -o $@

$(SYNC_SERVER_TARGET): $(UNP_CORE) $(SYNC_SERVER_CORE) $(SYNC_SERVER_MAIN_OBJ)
	$(CC) $(FLAGS) $(SYNC_SERVER_MAIN_OBJ) $(SYNC_SERVER_CORE) $(UNP_CORE) -L $(LIBS) $(LDFLAGS) -o $@

$(SYNC_SERVER_CORE): $(SYNC_SERVER_CORE_OBJS)
	$(CC) -shared $(SYNC_SERVER_CORE_OBJS) -o $@

$(SYNC_SERVER_CORE_OBJS): $(SYNC_SERVER_OBJ_DIR)/%.o : $(SYNC_SERVER_DIR)/%.cpp
	@$(MKDIR) $(dir $(SYNC_SERVER_CORE_OBJS))
	$(CC) $(DEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -I examples -o $@

#$(SYNC_SERVER_MAIN_OBJ): $(SYNC_SERVER_DIR)/main.cpp
#$(CC) $(DEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -I examples -o $@

$(UNP_CORE): $(UNP_CORE_OBJECTS)
	$(CC) -shared $(UNP_CORE_OBJECTS) -o $@
	@echo "unp_core.so ok...."

$(UNP_CORE_OBJECTS): $(UNP_OBJ_DIR)/%.o : $(UNP_SOURCEDIR)/%.cpp
	@$(MKDIR) $(dir $(UNP_CORE_OBJECTS))
	$(CC) $(DEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -o $@

$(TEST_OBJS):$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@$(MKDIR) $(dir $(TEST_OBJS))
	$(CC) $(TESTDEFINES) $(BUILD_FLAGS) $< -I $(INCLUDES) -I examples -o $@


clean: FORCE
	@$(RM) $(UNP_CORE_OBJECTS)
	@echo "clean unp objects"
	$(RM) $(UNP_CORE)
	$(RM) $(MAIN)
	$(RM) $(TEST_TARGET)
	@$(RM) $(TEST_OBJS)
	@echo "clean test objects"
	@$(RM) $(SYNC_CLIENT_OBJS)
	@echo "clean sync_client objects"
	$(RM) $(SYNC_CLIENT_TARGET)
	$(RM) $(SYNC_CLIENT_CORE)
	@$(RM) $(SYNC_SERVER_OBJS)
	@echo "clean sync_server objects"
	$(RM) $(SYNC_SERVER_CORE)
	$(RM) $(SYNC_SERVER_TARGET)

examples_clean: FORCE
	@$(RM) $(SYNC_CLIENT_OBJS)
	@echo "clean sync_client objects"
	$(RM) $(SYNC_CLIENT_TARGET)
	$(RM) $(SYNC_CLIENT_CORE)
	@$(RM) $(SYNC_SERVER_OBJS)
	@echo "clean sync_server objects"
	$(RM) $(SYNC_SERVER_CORE)
	$(RM) $(SYNC_SERVER_TARGET)

unp_clean: FORCE
	@$(RM) $(UNP_CORE_OBJECTS)
	@echo "clean unp objects"
	$(RM) $(UNP_CORE)
	$(RM) $(MAIN)

test_clean: FORCE
	$(RM) $(TEST_TARGET)
	@$(RM) $(TEST_OBJS)
	@echo "clean test objects"



FORCE:
