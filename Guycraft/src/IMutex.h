#pragma once
#include <mutex>
class IMutex {
public:
	std::mutex mutex;
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
};