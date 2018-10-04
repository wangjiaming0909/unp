#include "server_test.h"
#include <string.h>

TEST::SERVER_TEST::SERVER_TEST()
{
}

void TEST::SERVER_TEST::if_non_valid_config_file_passed_in()
{
    config::ServerConfig* config_ptr = nullptr;
    try {
        m_server_ptr = new server::Server(config_ptr);
    } catch (const std::string& e) {
        assert(e == "config_ptr is nullptr");
    } catch (const char*const e){
        int result = strcmp(e, "config_ptr is nullptr");
        assert(result == 0);
    }catch(...){
        ASSERT_FAILED;
        throw;
    }
    ASSERT_OK;
}

void TEST::SERVER_TEST::should_initialized_well()
{
    config::ServerConfig* config_ptr = new config::ServerConfig("server_conf.json");
    m_server_ptr = new server::Server(config_ptr);
    assert(m_server_ptr->initialized == true);
    assert(m_server_ptr->binded ==true);
    assert(m_server_ptr->started == false);
    ASSERT_OK;
}

void TEST::SERVER_TEST::should_be_started()
{
    m_server_ptr->start();
    assert(m_server_ptr->started == true);
    ASSERT_OK;
}



