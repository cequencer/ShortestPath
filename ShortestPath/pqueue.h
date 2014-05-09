#include <list>

// Очередь с приоритетом, в которой можно удалять элементы
template<typename T>
class PriorityQueue
{
	std::list<T> list;

public:
	bool empty() const;
	T top() const;
	void push(const T& value);
	void pop();
	void remove(const T& value);
	size_t size() const;
};

template <typename T>
bool PriorityQueue<T>::empty() const
{
	return list.empty();
}
template <typename T>
T PriorityQueue<T>::top() const
{
	return list.front();
}

template <typename T>
void PriorityQueue<T>::push(const T& value)
{
	for(typename std::list<T>::const_iterator iterator = list.begin(); iterator != list.end(); ++iterator)
		if(value < *iterator)
		{
			list.insert(iterator, value);
			return;
		}
	list.push_back(value);
}

template <typename T>
void PriorityQueue<T>::pop()
{
	list.pop_front();
}

template <typename T>
void PriorityQueue<T>::remove(const T& value)
{
	list.remove(value);
}

template <typename T>
size_t PriorityQueue<T>::size() const
{
	return list.size();
}