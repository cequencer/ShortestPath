#pragma once
#ifndef GRAPHIO_H
#define GRAPHIO_H

#include <iostream>
#include <cmath>
#include "astar.h"
#include "heuristic.h"

namespace graphio
{
	using graph::Graph;
	using graph::astar_heuristic;

	typedef std::pair<double, double> point;

	class GraphIO
	{
		static const std::string DataStructureErrorText;
	public:
		static Graph<int,int> from_stream_int(std::istream& in_stream);
		static Graph<point,double> from_stream_double_double(std::istream& in_stream);
	};

	const std::string GraphIO::DataStructureErrorText = "Incorrect data structure in istream.";

	/**
	@brief Считывание графа Graph<int,int> из входного потока.
	@details Правильный формат данных: число вершин, список ребер.
	<br/><br/>
	ЧИСЛО_ВЕРШИН<br/>
	НАЧАЛЬНАЯ_ВЕРШИНА_РЕБРА	КОНЕЧНАЯ_ВЕРШИНА_РЕБРА	ВЕС_РЕБРА<br/>
	...	... ... ... ...<br/>
	НАЧАЛЬНАЯ_ВЕРШИНА_РЕБРА	КОНЕЧНАЯ_ВЕРШИНА_РЕБРА	ВЕС_РЕБРА
	<br/><br/>
	Граф неориентированный; не нужно дублировать ребро в списке, меняя местами начальную и конечную вершины.
	@param in_stream входной поток
	@return граф
	*/
	inline Graph<int,int> GraphIO::from_stream_int(std::istream& in_stream)
	{
		size_t num_vertices;
		if(!(in_stream >> num_vertices))
			throw std::ios_base::failure(DataStructureErrorText);
		Graph<int,int> graph = Graph<int,int>(num_vertices);

		size_t vertex_origin, vertex_destination;
		int weight;
		while(in_stream >> vertex_origin && in_stream >> vertex_destination && in_stream >> weight)
			graph.add_edge(vertex_origin, vertex_destination, weight);

		return graph;
	}

	/**
	@brief Считывание графа Graph<pair<double,double>,double> из входного потока.
	@details Правильный формат данных: число вершин, веса вершин, список ребер.
	<br/><br/>
	ЧИСЛО_ВЕРШИН<br/>
	НОМЕР_ВЕРШИНЫ	КООРДИНАТА_X	КООРДИНАТА_Y<br/>
	...	... ... ... ...<br/>
	НОМЕР_ВЕРШИНЫ	КООРДИНАТА_X	КООРДИНАТА_Y<br/>
	НАЧАЛЬНАЯ_ВЕРШИНА_РЕБРА	КОНЕЧНАЯ_ВЕРШИНА_РЕБРА<br/>
	...	... ... ... ...<br/>
	НАЧАЛЬНАЯ_ВЕРШИНА_РЕБРА	КОНЕЧНАЯ_ВЕРШИНА_РЕБРА
	<br/><br/>
	Граф неориентированный; не нужно дублировать ребро в списке, меняя местами начальную и конечную вершины.
	@param in_stream входной поток
	@return граф
	*/
	inline Graph<point,double> GraphIO::from_stream_double_double(std::istream& in_stream)
	{
		size_t num_vertices;
		if(!(in_stream >> num_vertices))
			throw std::ios_base::failure(DataStructureErrorText);
		Graph<point,double> graph = Graph<point,double>(num_vertices);

		size_t vertex; 
		double x, y;
		for(size_t i=1; i <= num_vertices; ++i)
		{
			if(in_stream >> vertex && in_stream >> x && in_stream >> y)
				graph.set_vertex_value(vertex, point(x,y));
			else 
				throw std::ios_base::failure(DataStructureErrorText);
		}	

		size_t vertex_origin, vertex_destination;
		while(in_stream >> vertex_origin && in_stream >> vertex_destination)
			graph.add_edge(vertex_origin, vertex_destination, astar_heuristic(graph, vertex_origin, vertex_destination));

		return graph;
	}
}
#endif