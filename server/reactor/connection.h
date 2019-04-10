/* 
 * File:   connection.h
 * Author: JWang284
 *
 * Created on April 10, 2019, 9:29 AM
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "server/reactor/event_handler.h"

namespace reactor{


class connection : public event_handler{
public:
    connection(Reactor& reactor);
    virtual int handle_input(int ) override;
    virtual int handle_output(int ) override;
    virtual int handle_timeout(int ) override;
    virtual int handle_close(int ) override;
    virtual int handle_signal(int ) override;
    virtual int get_handle() const override;
    virtual void set_handle(int ) override;
    virtual ~connection();
private:
	net::sock_stream	stream_;
};

}
#endif /* CONNECTION_H */

