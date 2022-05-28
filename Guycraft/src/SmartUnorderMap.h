#pragma once
#include <unordered_map>
#include <mutex>
template<typename Key, typename T>
class SmartUnorderMap {
private:
	std::mutex mutex;
public:
	std::unordered_map<Key, T> m_map;
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	void add(Key key, T value) {
		m_map.emplace(key, value);
	}
	void addNoneExist(Key k, T v) {
		if (not has(k)) {
			add(k, v);
		}
	}
	bool has(Key key) {
		return m_map.find(key) != m_map.end();
	}
};