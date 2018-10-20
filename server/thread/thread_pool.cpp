#include "thread_pool.h"
using namespace thread;
Thread_Pool::Thread_Pool(const util::string& name) 
	: m_mutex(), m_running(false), m_thread_name(name), m_maxQueueSize(0)
{ }

Thread_Pool::~Thread_Pool(){
	
}
bool Thread_Pool::isFull() const{

}

//$this is const, so m_mutex is also const 
//const mutex can't pass to lock_guard<std::mutex>(std::mutex&)
//so here m_mutex need to be defiend as mutable
size_t Thread_Pool::queueSize() const{
	std::lock_guard<std::mutex> _(m_mutex);
	return m_maxQueueSize;
}

void Thread_Pool::runInThread(){

}
