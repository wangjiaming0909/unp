/* 
 * File:   connection.cpp
 * Author: JWang284
 * 
 * Created on April 10, 2019, 9:29 AM
 */

#include "connection.h"

using namespace reactor;

connection::connection(Reactor& reactor) :event_handler(reactor)
{
}

connection::~connection()
{
}

int connection::handle_input(int )
{

}

int connection::handle_output(int )
{

}

int connection::handle_timeout(int ) 
{

}

int connection::handle_close(int )
{

}

int connection::handle_signal(int )
{

}

int connection::get_handle() const
{

}

void connection::set_handle(int )
{

}