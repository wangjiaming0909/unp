### boost:
    ./bjam --with-thread --with-system --with-timer

### filtering gtests:
    "args": ["--gtest_filter=select_reactor_impl_test.*"]



### COMPLETED
- 1. Io_handler refactoring: remove read_handler, read_write_handler, write_handler, combine into one, remove task
- 2. Add buffer into io_handler, add socket read and write into buffer, 需要注意buffer大小与socket缓冲区大小的关系
	从reactor中临时关闭某一个 fd 的某个 event
- 3. Refactoring acceptor and connector, cause they used thread_pool
- 4. Add TcpServer, TcpClient, add threadpool into TcpServer and TcpClient
    > 1. 需要更改acceptor 和connector，如何做到让新 make的 connection_handler 放到其他线程的reactor上
    > 2. 可以把acceptor放在第一线程上，其他所有connection_handler 都放在其他线程的 reactor 上
    > 3. 那么第一线程上的reactor只是注册acceptor事件，不做 make_handler, activate_handler等事件,可以将acceptor更加精简

### TODO LIST 2019-6-26
- 1. Delete eventfd in reactor, add eventfd to reactor implementation, cause every reactor need to be waken up
- 2. Add timer class
- 3. Add timer timeout handlers in select/poll/epoll
- 4. When to wake up reactor???
    > 1. When deleting events from the reactor
    > 2. When adding events into the reactor
- 5. Refactoring, could have SelectReactor/PollReactor/EpollReactor inherit from Reactor for different interface of reactor
- 6. Add signal events??
- 7. Like libevent, define an event struct for all events(io events, timeout events, signals)