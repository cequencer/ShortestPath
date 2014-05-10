#include <vector>
#include <stdexcept>

// Очередь с приоритетом, в которой можно удалять элементы.
// Реализована на основе двоичной кучи.
template<typename T>
class PriorityQueue
{
	std::vector<T> storage;
public:
	PriorityQueue();
	explicit PriorityQueue(size_t capacity);
	bool empty() const;
	T top() const;
	void push(const T& value);
	void pop();
	void remove(const T& value);
	bool contains(const T& value) const;
	size_t size() const;
};

template <typename T>
PriorityQueue<T>::PriorityQueue() { }

template <typename T>
PriorityQueue<T>::PriorityQueue(size_t capacity)
{
	storage.reserve(capacity);
}

template <typename T>
bool PriorityQueue<T>::empty() const
{
	return storage.empty();
}
template <typename T>
T PriorityQueue<T>::top() const
{
	if(storage.empty())
		throw std::out_of_range("Queue is empty.");
	return storage.front();
}

// Добавляет элемент в конец вектора и <<поднимает>> его до правильной позиции.
template <typename T>
void PriorityQueue<T>::push(const T& value)
{
	storage.push_back(value);

	if(storage.size() == storage.capacity())
		storage.reserve(2*storage.size());

	int current = static_cast<int>(storage.size())-1;
	int parent = static_cast<int>(storage.size())/2-1;
	while(current > 0 && storage[current] < storage[parent])
	{
		T temp = storage[current];
		storage[current] = storage[parent];
		storage[parent] = temp;
		current = parent;
		parent = (current + 1)/2 - 1;
	}
}

// Удаляет корень, на его место записывает последний элемент в векторе
// и <<опускает>> этот элемент до правильной позиции.
template <typename T>
void PriorityQueue<T>::pop()
{
	if(storage.empty())
		throw std::out_of_range("Queue is empty.");

	T last = storage.back();
	storage.pop_back();
	if(storage.empty())
		return;

	size_t current = 0;
	storage[current] = last;
	while (current+1 <= storage.size()/2)
	{
		size_t left_child = 2*(current + 1) - 1;
		size_t right_child = left_child+1;
		size_t swap_child;
		if(left_child+1 < storage.size() && storage[right_child] < storage[left_child])
			swap_child = right_child;
		else
			swap_child = left_child;
		if(storage[swap_child] < storage[current])
		{
			T temp = storage[current];
			storage[current] = storage[swap_child];
			storage[swap_child] = temp;
			current = swap_child;
		}
		else
			break;
	}
}

// Находит нужный элемент, <<поднимает>> его, пока тот не станет корнем, выполняет pop().
template <typename T>
void PriorityQueue<T>::remove(const T& value)
{
	size_t value_index;
	for(value_index=0; value_index < storage.size(); ++value_index)
		if(storage[value_index] == value)
			break;
	if(value_index == storage.size())
		return;

	int current = static_cast<int>(value_index);
	int parent = static_cast<int>(current + 1)/2 - 1;
	while(current > 0)
	{
		T temp = storage[current];
		storage[current] = storage[parent];
		storage[parent] = temp;
		current = parent;
		parent = (current + 1)/2 - 1;
	}

	pop();
}

template <typename T>
bool PriorityQueue<T>::contains(const T& value) const
{
	size_t value_index;
	for(value_index=0; value_index < storage.size(); ++value_index)
		if(storage[value_index] == value)
			return true;
	return false;
}

template <typename T>
size_t PriorityQueue<T>::size() const
{
	return storage.size();
}