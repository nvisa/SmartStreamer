#ifndef SIMPLEBUFFER_H
#define SIMPLEBUFFER_H

#include <algorithm>
#include <utility>
#include <memory>
#include <vector>

#include <QDebug>

template <typename T>
class simple_buffer
{
public:
	simple_buffer(unsigned long long size)
		: length(size),
		  buffer_data()
	{
		buffer_data.reserve(size);
	}
	std::pair<bool, T const*> try_add(T const* data, unsigned long long& size)
	{
		auto inserter_iter = std::back_inserter(buffer_data);
		if (used_portion + size < length) {
			std::copy(data, data + size, inserter_iter);
			used_portion += size;
			size = 0;
			return std::pair<bool, T const*>{true, nullptr};
		} else {
			unsigned long long diff = length - used_portion;
			std::copy(data, data + diff, inserter_iter);
			used_portion = length;
			size -= diff;
			return std::pair<bool, T const*>{false, data + diff};
		}
	}
	std::pair<std::unique_ptr<T[]>, unsigned long long> flush()
	{
		T* data = new T[used_portion];
		std::copy(buffer_data.begin(), buffer_data.begin() + used_portion, data);
		std::unique_ptr<T[]> ptr(data);
		unsigned long long size = used_portion;
		buffer_data.clear();
		used_portion = 0;
		return std::pair<std::unique_ptr<T[]>, unsigned long long>(std::move(ptr), size);
	}
	~simple_buffer() {}
private:
	unsigned long long length;
	unsigned long long used_portion = 0;
	std::vector<T> buffer_data;
};

#endif // SIMPLEBUFFER_H
