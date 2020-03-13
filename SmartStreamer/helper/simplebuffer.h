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
		: size(size),
		  index(0),
		  alloc(),
		  begin(alloc.allocate(size)),
		  end(begin + size) {}

	T const* try_add(T const* data, unsigned long long& data_size)
	{
		T * const relative_begin = begin + index;
		T const * data_end;
		if (data_size + index < size) {
			index += data_size;
			data_end = data + data_size;
			data_size = 0;
		} else {
			unsigned long long available = size - index;
			index = size;
			data_end = data + available;
			data_size -= available;
		}
		std::copy(data, data_end, relative_begin);
		return data_end;
	}
	using data_pair = std::pair<T const* const, unsigned long long>;
	data_pair flush()
	{
		unsigned long long data_size = index;
		index = 0;
		return data_pair{begin, data_size};
	}
	~simple_buffer()
	{
		alloc.deallocate(begin, size);
	}
private:
	unsigned long long size;
	unsigned long long index;
	std::allocator<T> alloc;
	T * const begin;
	T * const end;
};

#endif // SIMPLEBUFFER_H
