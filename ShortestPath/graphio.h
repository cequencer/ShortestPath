#pragma once
#ifndef GRAPHIO_H
#define GRAPHIO_H

#define IOEXCEPTION "Incorrect data structure in istream."

#include <iostream>
#include <math.h>
#include "astar.h"

class GraphIO;
class AStarEuclidianHeuristic;

typedef std::pair<double, double> point;

class GraphIO
{
public:
	static Graph<int,int> from_stream_int(std::istream& in_stream);
	static Graph<point,double> from_stream_double_double(std::istream& in_stream);
};

// ������������� ������ ��� ��������� A* �� ������ ��������� ���������� ����� ������� ���������.
class AStarEuclidianHeuristic : public AStarSearch<point, double>::AStarDefaultHeuristic
{
public:
	virtual double get_cost(const Graph<point, double> graph, int start, int goal) const override;
};

// ���������� ������ ������: ����� ������, ������ �����.
//
// �����_������
// ���������_�������_�����	��������_�������_�����	���_�����
//			...						...					...
// ���������_�������_�����	��������_�������_�����	���_�����
//
// ���� �����������������; �� ����� ����������� ����� � ������, ����� ������� ��������� � �������� �������.
inline Graph<int,int> GraphIO::from_stream_int(std::istream& in_stream)
{
	int num_vertices;
	if(!(in_stream >> num_vertices))
		throw std::ios_base::failure(IOEXCEPTION);
	Graph<int,int> graph = Graph<int,int>(num_vertices);

	int vertex_origin, vertex_destination, weight;
	while(in_stream >> vertex_origin && in_stream >> vertex_destination && in_stream >> weight)
		graph.add_edge(vertex_origin, vertex_destination, weight);

	return graph;
}

// ���������� ������ ������: ����� ������, ���� ������, ������ �����.
//
// �����_������
// �����_�������	����������_X	����������_Y
//		...				...				...
// �����_�������	����������_X	����������_Y
// ���������_�������_�����	��������_�������_�����
//			...						...
// ���������_�������_�����	��������_�������_�����
//
// ���� �����������������; �� ����� ����������� ����� � ������, ����� ������� ��������� � �������� �������.
inline Graph<point,double> GraphIO::from_stream_double_double(std::istream& in_stream)
{
	int num_vertices;
	if(!(in_stream >> num_vertices))
		throw std::ios_base::failure(IOEXCEPTION);
	Graph<point,double> graph = Graph<point,double>(num_vertices);

	int vertex; 
	double x, y;
	for(int i=1; i <= num_vertices; ++i)
	{
		if(in_stream >> vertex && in_stream >> x && in_stream >> y)
			graph.set_vertex_value(vertex, point(x,y));
		else 
			throw std::ios_base::failure(IOEXCEPTION);
	}	

	AStarEuclidianHeuristic heuristic;
	int vertex_origin, vertex_destination;
	while(in_stream >> vertex_origin && in_stream >> vertex_destination)
		graph.add_edge(vertex_origin, vertex_destination, heuristic.get_cost(graph, vertex_origin, vertex_destination));

	return graph;
}

inline double AStarEuclidianHeuristic::get_cost(const Graph<point, double> graph, int start, int goal) const
{
	point start_point, goal_point;
	graph.get_vertex_value(start, start_point);
	graph.get_vertex_value(goal, goal_point);

	double x = start_point.first - goal_point.first;
	double y = start_point.second - goal_point.second;
	return sqrt(x*x + y*y);
}
#endif