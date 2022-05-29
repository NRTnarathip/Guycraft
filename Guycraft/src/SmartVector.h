#pragma once
#include <vector>
#include <IMutex.h>

template<typename T>
class SmartVector : public IMutex{
public:
	std::vector<T> m_vector;
	void push_back(T data) {
		m_vector.push_back(data);
	}
	void transferData(std::vector<T>& other) {
		other = m_vector;
	}
	void isEmpty() {
		return m_vector.size() == 0;
	}
	int size() {
		return m_vector.size();
	}
	int saftSize() {
		lock();
		int s = size();
		unlock();
		return s;
	}
};