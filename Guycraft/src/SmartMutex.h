#pragma once
#include <mutex>

template<typename T>
class SmartMutex {
private:
	std::mutex mutex;
	T m_value;
public:
	void lock() {
		mutex.lock();
	}
	T& get() {
		return m_value;
	}
	void unlock() {
		mutex.unlock();
	}
};