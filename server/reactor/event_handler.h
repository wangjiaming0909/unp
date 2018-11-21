#ifndef _UNP_REACTOR_EVENT_HANDLER_H_
#define _UNP_REACTOR_EVENT_HANDLER_H_


namespace reactor
{
class event_handler{
public:
    typedef unsigned int Event_Type;
    enum{
        READ_EVENT = 01,
        ACCEPT_EVENT = 02,
        WRITE_EVENT = 03,
        TIMEOUT_EVENT = 04,
        SIGNAL_EVENT = 010,
        CLOSE_EVENT = 020
    };
    event_handler(int handle) : handle_(handle){};
    virtual void handle_input(int handle) = 0;
    virtual void handle_output(int handle) = 0;
    virtual void handle_timeout(int handle) = 0;
    virtual void handle_close(int handle) = 0;
    virtual void handle_signal(int handle) = 0;
    virtual int get_handle() const = 0;
protected:
    virtual ~event_handler();
private:
    int handle_;
};
} // reactor



#endif // _UNP_REACTOR_EVENT_HANDLER_H_