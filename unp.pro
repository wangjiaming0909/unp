# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = unp

HEADERS = \
   $$PWD/server/config/configoption.h \
   $$PWD/server/config/json11.hpp \
   $$PWD/server/config/optionValidator.h \
   $$PWD/server/config/ServerConfig.h \
   $$PWD/server/net/addr.h \
   $$PWD/server/net/handle_set.h \
   $$PWD/server/net/inet_addr.h \
   $$PWD/server/net/inet_sock.h \
   $$PWD/server/net/macros.h \
   $$PWD/server/net/sock_acceptor.h \
   $$PWD/server/net/sock_connector.h \
   $$PWD/server/net/sock_stream.h \
   $$PWD/server/net/unp.h \
   $$PWD/server/reactor/acceptor.h \
   $$PWD/server/reactor/buffer.h \
   $$PWD/server/reactor/connection_handler.h \
   $$PWD/server/reactor/connector.h \
   $$PWD/server/reactor/event_handler.h \
   $$PWD/server/reactor/io_handler.h \
   $$PWD/server/reactor/poll_reactor_impl.h \
   $$PWD/server/reactor/reactor.h \
   $$PWD/server/reactor/reactor_acceptor.h \
   $$PWD/server/reactor/reactor_implementation.h \
   $$PWD/server/reactor/reactor_sever.h \
   $$PWD/server/reactor/read_handler.h \
   $$PWD/server/reactor/read_write_handler.h \
   $$PWD/server/reactor/select_reactor_impl.h \
   $$PWD/server/reactor/write_handler.h \
   $$PWD/server/thread/condition_base.h \
   $$PWD/server/thread/currentthread.h \
   $$PWD/server/thread/latch.h \
   $$PWD/server/thread/lock_guard.h \
   $$PWD/server/thread/message_queue.h \
   $$PWD/server/thread/mutex_base.h \
   $$PWD/server/thread/pcondition.h \
   $$PWD/server/thread/pmutex.h \
   $$PWD/server/thread/stdmutex.h \
   $$PWD/server/thread/task.h \
   $$PWD/server/thread/thread.h \
   $$PWD/server/thread/thread_data_base.h \
   $$PWD/server/thread/thread_pool.h \
   $$PWD/server/util/easylogging++.h \
   $$PWD/server/util/FileUtil.h \
   $$PWD/server/util/min_heap.h \
   $$PWD/server/util/unp_date.h \
   $$PWD/server/util/unp_time.h \
   $$PWD/server/util/XString.h \
   $$PWD/server/Client.h \
   $$PWD/server/main_helper.h \
   $$PWD/server/os.h \
   $$PWD/server/reactor_server.h \
   $$PWD/server/Server.h \
   $$PWD/server/server_status.h \
   $$PWD/server/Socketops.h \
    server/reactor/tcp_client.h \
    server/reactor/tcp_server.h

SOURCES = \
   $$PWD/server/config/configoption.cpp \
   $$PWD/server/config/json11.cpp \
   $$PWD/server/config/optionValidator.cpp \
   $$PWD/server/config/ServerConfig.cpp \
   $$PWD/server/net/addr.cpp \
   $$PWD/server/net/inet_addr.cpp \
   $$PWD/server/net/sock_acceptor.cpp \
   $$PWD/server/net/sock_stream.cpp \
   $$PWD/server/net/unp.cpp \
   $$PWD/server/reactor/acceptor.cpp \
   $$PWD/server/reactor/buffer.cpp \
   $$PWD/server/reactor/connection_handler.cpp \
   $$PWD/server/reactor/connector.cpp \
   $$PWD/server/reactor/io_handler.cpp \
   $$PWD/server/reactor/poll_reactor_impl.cpp \
   $$PWD/server/reactor/reactor_acceptor.cpp \
   $$PWD/server/reactor/reactor_sever.cpp \
   $$PWD/server/reactor/select_reactor_impl.cpp \
   $$PWD/server/thread/message_queue.cpp \
   $$PWD/server/thread/task.cpp \
   $$PWD/server/thread/thread_pool.cpp \
   $$PWD/server/util/easylogging++.cpp \
   $$PWD/server/util/FileUtil.cpp \
   $$PWD/server/util/min_heap.cpp \
   $$PWD/server/util/unp_date.cpp \
   $$PWD/server/util/unp_time.cpp \
   $$PWD/server/Client.cpp \
   $$PWD/server/main.cpp \
   $$PWD/server/reactor_server.cpp \
   $$PWD/server/Server.cpp \
   $$PWD/server/Socketops.cpp \
    server/reactor/tcp_client.cpp \
    server/reactor/tcp_server.cpp

INCLUDEPATH = \
    $$PWD/server \
    $$PWD/server/config \
    $$PWD/server/net \
    $$PWD/server/reactor \
    $$PWD/server/thread \
    $$PWD/server/util

#DEFINES = 

QMAKE_CXXFLAGS += -std=c++17
