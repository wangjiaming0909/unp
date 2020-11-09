#include "fileutil_test.h"
TEST::FILEUTIL_TEST::FILEUTIL_TEST()
{
    util::string configFileName = "server_conf.json";
    char *cwd;
    try{
        cwd = get_current_dir_name();
    }catch(std::exception e){
        std::cout << strerror(errno) << std::endl;
        throw;
    }
    if(long(strlen(cwd) + configFileName.size()) > pathconf(cwd, _PC_NAME_MAX)){
        std::cout << "error file name too long" << std::endl;
        throw;
    }
    util::string fileFullName = cwd;
    fileFullName.append("/");
    fileFullName.append(configFileName);
    m_file_util_ptr = new config::FileUtil(fileFullName);
    free(cwd);
//    CONSOLE_LOG(fileFullName.as_std_string());
}

void TEST::FILEUTIL_TEST::read_to_string()
{
    util::string buffer;
    int maxSize = 512;
    m_file_util_ptr->readToString(maxSize, &buffer);
    const char* ptr = buffer.ptr();
    assert(*ptr == 123);
//    std::cout << ptr << std::endl;
    ASSERT_OK;
}
