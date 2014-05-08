#include <list>

// Очередь с приоритетом, в которой можно удалять элементы
template<typename T>
class PriorityQueue
{
	std::list<T> list;

public:
	bool empty();
	T top();
	void push(T value);
	void pop();
	void remove(T value);
	size_t size();
};

template <typename T>
bool PriorityQueue<T>::empty()
{
	return list.empty();
}
template <typename T>
T PriorityQueue<T>::top()
{
	return list.front();
}

template <typename T>
void PriorityQueue<T>::push(T value)
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
void PriorityQueue<T>::remove(T value)
{
	list.remove(value);
}

template <typename T>
size_t PriorityQueue<T>::size()
{
	return list.size();
}