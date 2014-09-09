#pragma once
#ifndef PQUEUE_H
#define PQUEUE_H

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace graph
{
	class empty_queue_error : public std::logic_error
	{
	public:
		explicit empty_queue_error (const std::string& what_arg) : logic_error(what_arg) {}
		explicit empty_queue_error (const char* what_arg) : logic_error(what_arg) {}
	};

	/**
	@brief Очередь с приоритетом, в которой можно удалять элементы.
	@details Реализована на основе двоичной кучи.
	*/
	template<typename T>
	class PriorityQueue
	{
		std::vector<T> storage;
		static const std::string EmptyQueueErrorText;
	public:
		PriorityQueue();
		explicit PriorityQueue(size_t capacity);
		bool empty() const;
		const T& top() const;
		void push(const T& value);
		void pop();
		void remove(const T& value);
		bool contains(const T& value) const;
		size_t size() const;
	};

	template <typename T>
	const std::string PriorityQueue<T>::EmptyQueueErrorText = "Priority queue is empty.";

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
	const T& PriorityQueue<T>::top() const
	{
		if(storage.empty())
			throw empty_queue_error(EmptyQueueErrorText);
		return storage.front();
	}

	/**
	@brief Добавление нового элемента в очередь.
	@details Добавляет элемент в конец вектора и "поднимает" его до правильной позиции.
	@param value добавляемый элемент
	*/
	template <typename T>
	void PriorityQueue<T>::push(const T& value)
	{
		storage.push_back(value);
		size_t current = storage.size()-1;
		size_t parent;
		while(current > 0)
		{
			parent = (current + 1)/2 - 1;
			if(!(storage[current] < storage[parent]))
				break;
			T temp = storage[current];
			storage[current] = storage[parent];
			storage[parent] = temp;
			current = parent;			
		}
	}
	
	/**
	@brief Удаление элемента с наивысшим приоритетом
	@details Удаляет корень, на его место записывает последний элемент в векторе
	и "опускает"этот элемент до правильной позиции
	*/
	template <typename T>
	void PriorityQueue<T>::pop()
	{
		if(storage.empty())
			throw empty_queue_error(EmptyQueueErrorText);

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

	template <typename T>
	void PriorityQueue<T>::remove(const T& value)
	{
		auto value_iterator = find(storage.begin(), storage.end(), value);
		if(value_iterator == storage.end())
			return;

		size_t current = distance(storage.begin(), value_iterator);
		size_t parent;
		while(current > 0)
		{
			parent = (current + 1)/2 - 1;
			T temp = storage[current];
			storage[current] = storage[parent];
			storage[parent] = temp;
			current = parent;			
		}

		pop();
	}

	template <typename T>
	bool PriorityQueue<T>::contains(const T& value) const
	{
		return (storage.end() != find(storage.begin(), storage.end(), value));
	}

	template <typename T>
	size_t PriorityQueue<T>::size() const
	{
		return storage.size();
	}
}
#endif