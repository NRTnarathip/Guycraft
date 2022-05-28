#pragma once
#include <mutex>
#include <deque>

//for safety cross threading
template<typename T>
class SmartQueue {
private:
	std::mutex mutex;
	std::deque<T> m_queue;
public:
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
	//will push back
	size_t size() {
		auto m_size = m_queue.size();
		return m_size;
	}
	bool empty() {
		return size() == 0;
	}
	void pushFront(T data) {
		m_queue.push_front(data);
	}
	void pushFrontLock(T data) {
		lock();
		pushFront(data);
		unlock();
	}
	void pushLock(T d) {
		lock();
		push(d);
		unlock();
	}
	void push(T data) {
		m_queue.push_back(data);
	}
	T front() {
		return m_queue.front();
	}
	T getFront() {
		auto data = m_queue.front();
		m_queue.pop_front();
		return data;
	}
};