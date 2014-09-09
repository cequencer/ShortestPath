#pragma once
#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "graph.h"

namespace graph
{	
	/**
	@brief Реализация эвристической функции оценки расстояния по умолчанию.
	@param graph граф
	@param start начальная вершина
	@param goal целевая вершина
	@return оценка расстояния
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	TEdgeWeight astar_heuristic(const Graph<TVertexValue, TEdgeWeight>& graph, size_t start, size_t goal)
	{
		return TEdgeWeight();
	}

	/**
	@brief Реализация эвристической функции оценки расстояния на плоскости.
	@param graph граф
	@param start начальная вершина
	@param goal целевая вершина
	@return оценка расстояния
	*/
	template<>
	inline double astar_heuristic<std::pair<double, double>, double>(
		const Graph<std::pair<double, double>, double>& graph, size_t start, size_t goal)
	{
		std::pair<double, double> start_point = graph.get_vertex_value(start);
		std::pair<double, double> goal_point = graph.get_vertex_value(goal);

		double x = start_point.first - goal_point.first;
		double y = start_point.second - goal_point.second;
		return sqrt(x*x + y*y);
	}
}

#endif