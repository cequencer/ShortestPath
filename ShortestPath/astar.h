#pragma once
#ifndef ASTAR_H
#define ASTAR_H

#include <set>
#include <list>
#include <limits>
#include "graph.h"
#include "pqueue.h"

namespace graph
{
	/**
	Алгоритм поиска кратчайшего пути A*
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	class AStarSearch
	{	
		struct VertexStatus;

		enum StatusCode { UNDISCOVERED, OPEN, CLOSED, UNDISCOVERED_GOAL, OPEN_GOAL };

		template<typename THeuristic>
		static TEdgeWeight min_heuristic_cost(const Graph<TVertexValue, TEdgeWeight>& graph,
			const size_t start, const std::set<size_t>& goal_group, THeuristic heuristic);

	public:
		template<typename THeuristic>
		static bool find_shortest_path(
			const Graph<TVertexValue, TEdgeWeight>& graph, const std::set<size_t>& start_group, const std::set<size_t>& goal_group,
			THeuristic heuristic, std::list<size_t>& shortest_path, TEdgeWeight& shortest_path_cost);
	};

	template<typename TVertexValue, typename TEdgeWeight>
	struct AStarSearch<TVertexValue, TEdgeWeight>::VertexStatus
	{	
		size_t vertex;
		size_t parent;	
		StatusCode status_code;
		TEdgeWeight cost_from_start_to_this;
		TEdgeWeight heuristic_cost_from_this_to_goal;
		TEdgeWeight heuristic_cost_from_start_to_goal;

		VertexStatus() : vertex(std::numeric_limits<size_t>::max()), status_code(UNDISCOVERED), parent(std::numeric_limits<size_t>::max()),
			cost_from_start_to_this(TEdgeWeight()),	heuristic_cost_from_this_to_goal(TEdgeWeight()), heuristic_cost_from_start_to_goal(TEdgeWeight()) {}

		bool operator<(const VertexStatus& rhs) const { return heuristic_cost_from_start_to_goal < rhs.heuristic_cost_from_start_to_goal; }
		bool operator==(const VertexStatus& rhs) const { return vertex == rhs.vertex; }
	};

	/**
	@brief Поиск кратчайшего пути между двумя множествами вершин.
	@details Алгоритм A* в исходном виде ищет кратчайший путь между двумя вершинами.
	В данной реализации алгоритм модифицирован: добавлена возможность поиска кратчайшего пути между двумя множествами вершин.
	Идея модифицированного алгоритма состоит в том, что в граф добаляется некоторая "виртуальная" вершина,
	которая соединена ребрами со всеми вершинами из множества стартовых вершин.
	Далее осуществляется поиск кратчайшего пути с помощью A* от добавленной до ближайшей целевой вершины.
	@param graph граф
	@param start_group множество начальных вершин
	@param goal_group множество целевых вершин
	@param heuristic функция, которая делает эвристическую оценку расстояния между парой вершин,
	имеет сигнатуру TEdgeWeight (const Graph<TVertexValue, TEdgeWeight>& graph, size_t start, size_t goal)
	@param shortest_path выходной параметр: кратчайший путь
	@param shortest_path_cost выходной параметр: длина кратчайшего пути
	@return если функция завершила работу успешно, то возвращается значение true, иначе false
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	template<typename THeuristic>
	bool AStarSearch<TVertexValue,TEdgeWeight>::find_shortest_path(
		const Graph<TVertexValue, TEdgeWeight>& graph, const std::set<size_t>& start_group, const std::set<size_t>& goal_group,
		THeuristic heuristic, std::list<size_t>& shortest_path, TEdgeWeight& shortest_path_cost)
	{
		if(start_group.empty() || goal_group.empty())
			return false;

		std::vector<VertexStatus> vertices_status(graph.get_num_vertices(), VertexStatus());
		for(auto i=goal_group.begin(); i != goal_group.end(); ++i)
		{
			size_t vertex = *i;
			if(vertex > graph.get_num_vertices())
				return false;
			vertices_status[vertex].vertex = vertex;
			vertices_status[vertex].status_code = UNDISCOVERED_GOAL;
		}

		PriorityQueue<VertexStatus> open_vertices_queue;
		for(auto i=start_group.begin(); i != start_group.end(); ++i)
		{
			size_t start = *i;
			if(start > graph.get_num_vertices())
				return false;

			vertices_status[start].vertex = start;
			vertices_status[start].status_code = OPEN;
			vertices_status[start].cost_from_start_to_this = TEdgeWeight();
			vertices_status[start].heuristic_cost_from_this_to_goal = min_heuristic_cost(graph, start, goal_group, heuristic);
			vertices_status[start].heuristic_cost_from_start_to_goal = vertices_status[start].heuristic_cost_from_this_to_goal;

			open_vertices_queue.push(vertices_status[start]);
		}

		while (!open_vertices_queue.empty())
		{
			VertexStatus open_vertex = open_vertices_queue.top();
			open_vertices_queue.pop();

			if(vertices_status[open_vertex.vertex].status_code == OPEN_GOAL)
			{			
				shortest_path_cost = open_vertex.cost_from_start_to_this;
				shortest_path.clear();
				size_t current_vertex = open_vertex.vertex;
				while (current_vertex != std::numeric_limits<size_t>::max())
				{
					shortest_path.push_front(current_vertex);
					current_vertex = vertices_status[current_vertex].parent;
				}
				return true;
			}

			vertices_status[open_vertex.vertex].status_code = CLOSED;

			auto neighbor_iter = graph.get_neighbors(open_vertex.vertex);
			for(auto i = neighbor_iter.first; i < neighbor_iter.second; ++i)
			{
				size_t neighbor = (*i).destination;
				if(vertices_status[neighbor].status_code == CLOSED)
				{
					continue;
				}

				TEdgeWeight edge_weight_to_neighbor = graph.get_edge_weight(open_vertex.vertex, neighbor);
				TEdgeWeight cost_from_start_to_neighbor = open_vertex.cost_from_start_to_this + edge_weight_to_neighbor;

				if((vertices_status[neighbor].status_code == OPEN || vertices_status[neighbor].status_code == OPEN_GOAL) &&
					cost_from_start_to_neighbor >= vertices_status[neighbor].cost_from_start_to_this)
				{
					continue;
				}

				if(vertices_status[neighbor].status_code == UNDISCOVERED)
				{
					vertices_status[neighbor].vertex = neighbor;
					vertices_status[neighbor].status_code = OPEN;				
				}
				else if(vertices_status[neighbor].status_code == UNDISCOVERED_GOAL)
				{
					vertices_status[neighbor].status_code = OPEN_GOAL;				
				}
				else /*if(vertices_status[neighbor].status_code == OPEN || vertices_status[neighbor].status_code == OPEN_GOAL)*/
				{
					open_vertices_queue.remove(vertices_status[neighbor]);
				}

				vertices_status[neighbor].parent = open_vertex.vertex;
				vertices_status[neighbor].cost_from_start_to_this = cost_from_start_to_neighbor;
				vertices_status[neighbor].heuristic_cost_from_this_to_goal = min_heuristic_cost(graph, neighbor, goal_group, heuristic);
				vertices_status[neighbor].heuristic_cost_from_start_to_goal =
					vertices_status[neighbor].cost_from_start_to_this + vertices_status[neighbor].heuristic_cost_from_this_to_goal;
				open_vertices_queue.push(vertices_status[neighbor]);
			}
		}

		return false;
	}

	// 
	// 
	/**
	@brief Получает оценки расстояний от выбранной вершины до всех конечных вершин, возвращает минимальную оценку.
	@details Предполагается, что goal_group не пуст.
	@param graph граф
	@param strat начальная вершина
	@param goal_group множество целевых вершин
	@param heuristic функция, которая делает эвристическую оценку расстояния между парой вершин,
	имеет сигнатуру TEdgeWeight (const Graph<TVertexValue, TEdgeWeight>& graph, size_t start, size_t goal)
	@return минимальная эвристическая оценка длины пути
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	template<typename THeuristic>
	TEdgeWeight AStarSearch<TVertexValue,TEdgeWeight>::min_heuristic_cost(const Graph<TVertexValue, TEdgeWeight>& graph, const size_t start,
		const std::set<size_t>& goal_group,	THeuristic heuristic)
	{	
		TEdgeWeight current_weight;
		TEdgeWeight min_weight = heuristic(graph, start, *goal_group.begin());
		for(auto i=++goal_group.begin(); i != goal_group.end(); ++i)
		{
			current_weight = heuristic(graph, start, *i);
			min_weight = current_weight < min_weight ? current_weight : min_weight;
		}
		return min_weight;
	}
}
#endif