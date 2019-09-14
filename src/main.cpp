#include "util/easylogging++.h"
#include "main_helper.h"

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv){

    server_scoped_helper s_h{argc, argv};
}
