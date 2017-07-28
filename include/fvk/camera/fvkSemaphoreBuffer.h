#pragma once
#ifndef fvkSemaphoreBuffer_h__
#define fvkSemaphoreBuffer_h__

/*********************************************************************************
created:	2016/11/06   01:54AM
modified:	2017/02/09   11:26PM
filename: 	fvkSemaphoreBuffer.h
file base:	fvkSemaphoreBuffer
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	creating a thread-safe synchronized queue and to achieve
			semaphore type functionality.
			Semaphores in C++
			The C++ standard does not define a semaphore type. You can write your own
			with an atomic counter, a mutex and a condition variable if you need, but
			most uses of semaphores are better replaced with mutexes and/or condition
			variables anyway.
			Unfortunately, for those cases where semaphores really are what you want,
			using a mutex and a condition variable adds overhead, and there is nothing
			in the C++ standard to help.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkSemaphore.h"

#include <queue>
#include <mutex>
#include <condition_variable>

namespace R3D
{

template <typename _T>
class FVK_EXPORT fvkSemaphoreBuffer
{
public:
	fvkSemaphoreBuffer() : m_sema_put(1), m_sema_get(0)
	{
	}
	fvkSemaphoreBuffer(const fvkSemaphoreBuffer& _other)
	{
		std::lock_guard<std::mutex> lk(_other.m_mutex);
		m_data = _other.m_data;
	}
	void put(const _T& _item, bool _sync_and_block_thread = false)
	{
		// In this case, camera thread needs notify from the processing thread to 
		// run as well as to put item in the data.
		// Meaning that if processing thread does not call get() method, camera can not
		// grab a new frame.
		// Camera thread is fully dependent on the processing thread notify.
		// In other words, this will do the perfect synchronization between two threads,
		// first have to wait until to get notify from the second, and
		// second as well have to wait until to get notify from the first.
		if (_sync_and_block_thread)
		{
			m_sema_put.wait();			// wait (block the thread) until you get notify from get() method.
			m_mutex.lock();
			m_data.push(_item);			// protect the queue data and push item to queue.
			m_mutex.unlock();
			m_sema_get.notify();		// notify get() method to pop data.
		}
		// In this case, camera thread will keep continue capturing,
		// there is no notify needed from the processing thread, but
		// in order to put item in the data, it needs notify from the processing thread.
		// Camera thread is not dependent on the processing thread notify.
		else
		{
			if (m_sema_put.try_wait())	// do not block thread, just wait to get notify from get() method to return true and run the following code.
			{
				m_mutex.lock();
				m_data.push(_item);		// protect the queue data and push item to queue.
				m_mutex.unlock();
				m_sema_get.notify();	// notify get() method to pop data.
			}
		}
	}

	_T get()
	{
		m_sema_get.wait();				// wait until you get notify from put() method.
		m_mutex.lock();
		_T _value = m_data.front();		// protect the queue data and pop item.
		m_data.pop();
		m_mutex.unlock();
		m_sema_put.notify();			// notify put() method to add item in the queue.
		return _value;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		return m_data.empty();
	}

private:
	mutable std::mutex m_mutex;		// protect queue mutex
	fvkSemaphore m_sema_put;
	fvkSemaphore m_sema_get;
	std::queue<_T> m_data;
};

}

/*
int n = 0;
fvkSemaphoreBuffer<std::string> data_queue;

void sender_thread()
{
	while (true)
	{
		if (n == 7) n = 0;
		std::string data = "Pattern # " + std::to_string(n++);
		std::cout << data << std::endl;
		data_queue.put(data);
		// very important note:
		// frame delay or sleep of the sender's thread must be higher than the 
		// receiver's thread frame delay for perfect synchronization.
		// In general, there is no need to set frame delay or sleep in the 
		// receiver's thread because of using fvkQThreadSafeQueue.
		// In this example: sender thread is projection()
		// and receiver thread is capture.
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
}
void receiver_thread()
{
	while (true)
	{
		std::string data = data_queue.get();
		if (data == "Pattern # 0")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 1")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 2")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 3")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 4")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 5")
			std::cout << "Received: " << data << std::endl;
		else if (data == "Pattern # 6")
			std::cout << "Received: " << data << std::endl;
	}
}

int main()
{
	std::thread t(sender_thread);
	std::thread t2(receiver_thread);
	t.join();
	t2.join();

	return 0;
}
*/

#endif // fvkSemaphoreBuffer_h__
