#pragma once
#ifndef ASTAR_H
#define ASTAR_H

#include <set>
#include <list>
#include "graph.h"
#include "pqueue.h"

// Алгоритм поиска кратчайшего пути A*
template<typename TVertexValue, typename TEdgeWeight>
class AStarSearch
{	
public:
	struct AStarDefaultHeuristic;
	static bool find_shortest_path(
		const Graph<TVertexValue, TEdgeWeight>& graph, const std::set<int> start_group, const std::set<int> goal_group,
		const AStarDefaultHeuristic& heuristic, std::list<int>& shortest_path, TEdgeWeight& shortest_path_cost);
private :
	enum StatusCode { UNDISCOVERED, OPEN, CLOSED, UNDISCOVERED_GOAL, OPEN_GOAL };
	struct VertexStatus;
	static TEdgeWeight min_heuristic_cost(const Graph<TVertexValue, TEdgeWeight>& graph,
		const int start, const std::set<int> goal_group, const AStarDefaultHeuristic& heuristic);
};

template<typename TVertexValue, typename TEdgeWeight>
struct AStarSearch<TVertexValue, TEdgeWeight>::VertexStatus
{	
	int vertex;
	int parent;	
	StatusCode status_code;
	TEdgeWeight cost_from_start_to_this;
	TEdgeWeight heuristic_cost_from_this_to_goal;
	TEdgeWeight heuristic_cost_from_start_to_goal;

	VertexStatus() : vertex(-1), status_code(UNDISCOVERED), parent(-1), cost_from_start_to_this(TEdgeWeight()),
		heuristic_cost_from_this_to_goal(TEdgeWeight()), heuristic_cost_from_start_to_goal(TEdgeWeight()) {}

	bool operator<(const VertexStatus& rhs) const { return heuristic_cost_from_start_to_goal < rhs.heuristic_cost_from_start_to_goal; }
	bool operator==(const VertexStatus& rhs) const { return vertex == rhs.vertex; }
};

// Алгоритм A* использует для поиска кратчайшего пути эвристическую оценку расстояния от текущей вершины до целевой. 
// Эвристическая оценка не должна переоценивать расстояние до конечной вершины.
// Класс по умолчанию AStarDefaultHeuristic всегда возвращает нулевую эвристическую оценку.
// Для того, чтобы ввести свою оценку, необходимо создать производный от AStarDefaultHeuristic класс и переопределить метод get_cost().
// Трудоемкость алгоритма A* существенно зависит от используемой эвристичекой оценки.
// Если использовать нулевую оценку по умолчанию, то получим в сущности алгоритм Дейкстры (останавливающийся при достижении целевой вершины).
template<typename TVertexValue, typename TEdgeWeight>
struct AStarSearch<TVertexValue, TEdgeWeight>::AStarDefaultHeuristic
{
	virtual ~AStarDefaultHeuristic() { }
	virtual TEdgeWeight get_cost(const Graph<TVertexValue, TEdgeWeight> graph, int start, int goal) const { return TEdgeWeight(); }
};

// Алгоритм A* в исходном виде ищет кратчайший путь между двумя вершинами.
// В данной реализации алгоритм модифицирован: добавлена возможность поиска кратчайшего пути между двумя группами вершин.
// Идея модифицированного алгоритма состоит в том, что в граф добаляется некоторая <<виртуальная>> вершина,
// которая соединена ребрами со всеми вершинами из группы стартовых вершин.
// Далее осуществляется поиск кратчайшего пути с помощью A* от добавленной до ближайшей целевой вершины.
template<typename TVertexValue, typename TEdgeWeight>
bool AStarSearch<TVertexValue,TEdgeWeight>::find_shortest_path(
	const Graph<TVertexValue, TEdgeWeight>& graph, const std::set<int> start_group, const std::set<int> goal_group,
	const AStarDefaultHeuristic& heuristic,	std::list<int>& shortest_path, TEdgeWeight& shortest_path_cost)
{
	if(start_group.empty() || goal_group.empty())
		return false;

	std::vector<VertexStatus> vertices_status(graph.get_num_vertices(), VertexStatus());
	for(std::set<int>::iterator i=goal_group.begin(); i != goal_group.end(); ++i)
	{
		int vertex = *i;
		if(vertex > graph.get_num_vertices() || vertex < 0)
			return false;
		vertices_status[vertex].vertex = vertex;
		vertices_status[vertex].status_code = UNDISCOVERED_GOAL;
	}

	PriorityQueue<VertexStatus> open_vertices_queue;
	for(std::set<int>::iterator i=start_group.begin(); i != start_group.end(); ++i)
	{
		int start = *i;
		if(start > graph.get_num_vertices() || start < 0)
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
			int current_vertex = open_vertex.vertex;
			while (current_vertex != -1)
			{
				shortest_path.push_front(current_vertex);
				current_vertex = vertices_status[current_vertex].parent;
			}
			return true;
		}

		vertices_status[open_vertex.vertex].status_code = CLOSED;

		std::vector<Edge<TEdgeWeight>> neighbors;
		graph.get_neighbors(open_vertex.vertex, neighbors);
		for(size_t i=0; i < neighbors.size(); ++i)
		{
			int neighbor = neighbors[i].destination;
			if(vertices_status[neighbor].status_code == CLOSED)
			{
				continue;
			}

			TEdgeWeight edge_weight_to_neighbor;
			graph.get_edge_weight(open_vertex.vertex, neighbor, edge_weight_to_neighbor);
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

// Получает оценки расстояний от выбранной вершины до всех конечных вершин, возвращает минимальную оценку.
// Предполагается, что goal_group не пуст.
template<typename TVertexValue, typename TEdgeWeight>
TEdgeWeight AStarSearch<TVertexValue,TEdgeWeight>::min_heuristic_cost(const Graph<TVertexValue, TEdgeWeight>& graph, const int start,
	const std::set<int> goal_group,	const AStarDefaultHeuristic& heuristic)
{	
	TEdgeWeight current_weight;
	TEdgeWeight min_weight = heuristic.get_cost(graph, start, *goal_group.begin());
	for(std::set<int>::iterator i=++goal_group.begin(); i != goal_group.end(); ++i)
	{
		current_weight = heuristic.get_cost(graph, start, *i);
		min_weight = current_weight < min_weight ? current_weight : min_weight;
	}
	return min_weight;
}
#endif