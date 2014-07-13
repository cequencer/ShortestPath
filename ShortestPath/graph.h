#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <ostream>
#include <vector>

// Класс, представляющий ребро графа.
// Экземпляр класса хранит только вес и конечную вершину.
template<typename TEdgeWeight>
struct Edge
{
	int destination;
	TEdgeWeight weight;
	Edge(int destination, TEdgeWeight weight): destination(destination), weight(weight)	{ }
};

// Класс, позволяющий сохранять дополнительную информацию о вершине (координаты и т.п.).
template<typename TVertexValue, typename TEdgeWeight>
struct Vertex
{
	TVertexValue value;
	std::vector<Edge<TEdgeWeight>> neighbors;
	Vertex() : value(TVertexValue()) { }
};

// Граф реализован в виде списка смежности.
// Большинство методов возращают булево значение - статус выполнения метода: успешно/неуспешно, а
// выходные данные (если таковые в методе имеются) в этом случае возвращаются через параметры метода.
template<typename TVertexValue, typename TEdgeWeight>
class Graph
{
	std::vector<Vertex<TVertexValue, TEdgeWeight>> adjacency_list;
	int num_vertices;
	bool is_edge_valid(const int vertex_origin, const int vertex_destination) const;

public:
	explicit Graph(int num_vertices);
	bool add_edge(const int vertex_origin, const int vertex_destination, const TEdgeWeight& weight);
	bool remove_edge(const int vertex_origin, const int vertex_destination);
	int get_num_vertices() const;
	bool get_neighbors(const int vertex, std::vector<Edge<TEdgeWeight>>& neighbors) const;
	bool contains_edge(const int vertex_origin, const int vertex_destination) const;
	bool get_edge_weight(const int vertex_origin, const int vertex_destination, TEdgeWeight& weight) const;
	bool set_edge_weight(const int vertex_origin, const int vertex_destination, const TEdgeWeight& weight);
	bool get_vertex_value(const int vertex, TVertexValue& value) const;
	bool set_vertex_value(const int vertex, const TVertexValue& value);
	void print(std::ostream& out_stream) const;
};

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::is_edge_valid(
	const int vertex_origin, const int vertex_destination) const
{
	if(vertex_origin >= num_vertices || vertex_destination >= num_vertices
		|| vertex_origin < 0 || vertex_destination < 0)
		return false;
	return true;
}

template<typename TVertexValue, typename TEdgeWeight>
Graph<TVertexValue, TEdgeWeight>::Graph(int num_vertices) : num_vertices(num_vertices)
{
	adjacency_list.resize(num_vertices);
}

// Граф неориентированный, поэтому метод добавляет сразу две записи
// в список смежности: ВЕРШИНА1 ВЕРШИНА2 ВЕС и ВЕРШИНА2 ВЕРШИНА1 ВЕС.
template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::add_edge(
	const int vertex_origin, const int vertex_destination, const TEdgeWeight& weight)
{
	if(!is_edge_valid(vertex_origin, vertex_destination))
		return false;

	for(size_t i=0; i < adjacency_list[vertex_origin].neighbors.size(); ++i)
		if(adjacency_list[vertex_origin].neighbors[i].destination == vertex_destination)
			return false;

	adjacency_list[vertex_origin].neighbors.push_back(Edge<TEdgeWeight>(vertex_destination, weight));
	adjacency_list[vertex_destination].neighbors.push_back(Edge<TEdgeWeight>(vertex_origin, weight));
	return true;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::remove_edge(
	const int vertex_origin, const int vertex_destination)
{
	if(!is_edge_valid(vertex_origin, vertex_destination))
		return false;

	for(size_t i=0; i < adjacency_list[vertex_origin].neighbors.size(); ++i)
		if(adjacency_list[vertex_origin].neighbors[i].destination == vertex_destination)
		{
			adjacency_list[vertex_origin].neighbors.erase(
				                             adjacency_list[vertex_origin].neighbors.begin()+i);
			for(size_t j=0; j < adjacency_list[vertex_destination].neighbors.size(); ++j)
				if(adjacency_list[vertex_destination].neighbors[j].destination == vertex_origin)
				{
					adjacency_list[vertex_destination].neighbors.erase(
						                                  adjacency_list[vertex_destination].neighbors.begin()+j);
					break;
				}
			return true;
		}

	return false;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::get_neighbors(
	const int vertex, std::vector<Edge<TEdgeWeight>>& neighbors) const
{
	if(vertex >= num_vertices || vertex < 0)
		return false;
	neighbors = std::vector<Edge<TEdgeWeight>>(adjacency_list[vertex].neighbors);
	return true;
}

template<typename TVertexValue, typename TEdgeWeight>
int Graph<TVertexValue, TEdgeWeight>::get_num_vertices() const
{
	return num_vertices;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::contains_edge(
	const int vertex_origin, const int vertex_destination) const
{
	TEdgeWeight weight;
	return get_edge_weight(vertex_origin, vertex_destination, weight);
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::get_edge_weight(
	const int vertex_origin, const int vertex_destination, TEdgeWeight& weight) const
{
	if(!is_edge_valid(vertex_origin, vertex_destination))
		return false;

	for(size_t i=0; i < adjacency_list[vertex_origin].neighbors.size(); ++i)
		if (adjacency_list[vertex_origin].neighbors[i].destination == vertex_destination)
		{
			weight = adjacency_list[vertex_origin].neighbors[i].weight;
			return true;
		}

	return false;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::set_edge_weight(
	const int vertex_origin, const int vertex_destination, const TEdgeWeight& weight)
{
	if(!is_edge_valid(vertex_origin, vertex_destination))
		return false;

	for(size_t i=0; i < adjacency_list[vertex_origin].neighbors.size(); ++i)
		if (adjacency_list[vertex_origin].neighbors[i].destination == vertex_destination)
		{
			adjacency_list[vertex_origin].neighbors[i].weight = weight;
			return true;
		}

	return false;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::get_vertex_value(
	const int vertex, TVertexValue& value) const
{
	if(vertex >= num_vertices || vertex < 0)
		return false;
	value = adjacency_list[vertex].value;
	return true;
}

template<typename TVertexValue, typename TEdgeWeight>
bool Graph<TVertexValue, TEdgeWeight>::set_vertex_value(
	const int vertex, const TVertexValue& value)
{
	if(vertex >= num_vertices || vertex < 0)
		return false;
	adjacency_list[vertex].value = value;
	return true;
}

template<typename TVertexValue, typename TEdgeWeight>
inline void Graph<TVertexValue, TEdgeWeight>::print(std::ostream& out_stream) const
{
	for(size_t i=0; i < adjacency_list.size(); ++i)
	{
		out_stream << i << " <--> ";
		for(size_t j=0; j < adjacency_list[i].neighbors.size(); ++j)
			out_stream << adjacency_list[i].neighbors[j].destination << " ";
		out_stream << std::endl;
	}
}
#endif