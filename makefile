CC = g++
BUILD_FLAGS = -Wall -Wextra -std=c++17 -c -ggdb -Wfatal-errors -O0 -fPIC
DEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING ELPP_DISABLE_DEBUG_LOGS
TESTDEFS = ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE USING_PTHREAD TESTING
#DEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING
#TESTDEFS = ELPP_DISABLE_LOGS USING_PTHREAD TESTING

DEFINES = $(patsubst %, -D%, $(DEFS))
TESTDEFINES = $(patsubst %, -D%, $(TESTDEFS))
LIBS = /usr/local/lib

LDFLAGS = -pthread \
		-lboost_system\
		-lboost_timer\
		-lboost_chrono\
		-lboost_filesystem \
		-lssl \
		-lcrypto \
		-lprotobuf \
		-lmysqlclient
		#-luv
		

TESTLDFLAG = -pthread \
		-lboost_system\
		-lboost_timer\
		-lboost_chrono\
		-lboost_filesystem \
		-lgtest\
		-lgmock\
		-lssl \
		-lcrypto\
		-lprotobuf\
		-lmysqlclient
	

UNP_SOURCEDIR = src
BUILDDIR = build
UNP_OBJ_DIR = $(BUILDDIR)/unp

MKDIR = mkdir -p
RM = rm -rf

INCLUDES = $(UNP_SOURCEDIR) -I /usr/local/include -I /boost_1_72_0/ -I /usr/include -I /usr/src/googletest/googlemock/include

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


all: $(MAIN) $(TEST_TARGET)

unp_core: $(UNP_CORE)

unp: $(MAIN)

test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(UNP_CORE) $(SYNC_CLIENT_CORE)
	$(CC) $(TEST_OBJS) $(UNP_CORE) -L $(LIBS) $(TESTLDFLAG) -o $@


$(MAIN): $(UNP_CORE)
	$(CC) $(FLAGS) $(DEFINES) $< ./build/unp/main.o -L $(LIBS) $(LDFLAGS) -o $@


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
