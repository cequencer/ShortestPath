#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <ostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace graph
{
	class vertex_out_of_range : public std::logic_error
	{
	public:
		explicit vertex_out_of_range (const std::string& what_arg) : logic_error(what_arg) {}
		explicit vertex_out_of_range (const char* what_arg) : logic_error(what_arg) {}
	};

	class nonexistent_edge_error : public std::logic_error
	{
	public:
		explicit nonexistent_edge_error (const std::string& what_arg) : logic_error(what_arg) {}
		explicit nonexistent_edge_error (const char* what_arg) : logic_error(what_arg) {}
	};

	/**
	@brief Класс, представляющий ребро графа. Экземпляр класса хранит только вес и конечную вершину.
	*/	
	template<typename TEdgeWeight>
	struct Edge
	{
		size_t destination;
		TEdgeWeight weight;
		explicit Edge(size_t destination = 0, TEdgeWeight weight = TEdgeWeight()) : 
			destination(destination), weight(weight) { }
	};

	/**
	@brief Класс, позволяющий сохранять дополнительную информацию о вершине (координаты и т.п.).
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	struct Vertex
	{
		TVertexValue value;
		std::vector<Edge<TEdgeWeight>> neighbors;
		Vertex() : value(TVertexValue()) { }
	};

	/**
	@brief Класс, представляющий граф. Граф реализован в виде списка смежности на основе std::vector
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	class Graph
	{
		std::vector<Vertex<TVertexValue, TEdgeWeight>> adjacency_list;

		size_t num_vertices;

		bool is_edge_valid(const size_t vertex_origin, const size_t vertex_destination) const;

		typename std::vector<Edge<TEdgeWeight>>::const_iterator find_edge(typename std::vector<Edge<TEdgeWeight>>::const_iterator begin,
			typename std::vector<Edge<TEdgeWeight>>::const_iterator end, size_t destination) const;

		typename std::vector<Edge<TEdgeWeight>>::iterator find_edge(typename std::vector<Edge<TEdgeWeight>>::iterator begin,
			typename std::vector<Edge<TEdgeWeight>>::iterator end, size_t destination);

		static const std::string VertexOutOfRangeText;

		static const std::string NonexistentEdgeErrorText;

	public:
		typedef typename std::vector<Edge<TEdgeWeight>>::const_iterator neighbor_iterator;

		explicit Graph(size_t num_vertices);
				
		void add_edge(const size_t vertex_origin, const size_t vertex_destination, const TEdgeWeight& weight);

		void remove_edge(const size_t vertex_origin, const size_t vertex_destination);	

		std::pair<neighbor_iterator, neighbor_iterator> get_neighbors(const size_t vertex) const;

		size_t get_num_vertices() const;

		bool contains_edge(const size_t vertex_origin, const size_t vertex_destination) const;

		TEdgeWeight get_edge_weight(const size_t vertex_origin, const size_t vertex_destination) const;

		void set_edge_weight(const size_t vertex_origin, const size_t vertex_destination, const TEdgeWeight& weight);

		TVertexValue get_vertex_value(const size_t vertex) const;

		void set_vertex_value(const size_t vertex, const TVertexValue& value);

		void print(std::ostream& out_stream) const;
	};

	template<typename TVertexValue, typename TEdgeWeight>
	const std::string Graph<TVertexValue, TEdgeWeight>::VertexOutOfRangeText = "Vertex number is out of range.";

	template<typename TVertexValue, typename TEdgeWeight>
	const std::string Graph<TVertexValue, TEdgeWeight>::NonexistentEdgeErrorText = "Graph doesn't contain requested edge.";

	template<typename TVertexValue, typename TEdgeWeight>
	inline bool Graph<TVertexValue, TEdgeWeight>::is_edge_valid(
		const size_t vertex_origin, const size_t vertex_destination) const
	{
		return !(vertex_origin >= num_vertices || vertex_destination >= num_vertices);
	}

	template<typename TVertexValue, typename TEdgeWeight>
	typename std::vector<Edge<TEdgeWeight>>::const_iterator Graph<TVertexValue, TEdgeWeight>::find_edge(typename std::vector<Edge<TEdgeWeight>>::const_iterator begin,
			typename std::vector<Edge<TEdgeWeight>>::const_iterator end, size_t destination) const
	{
		return std::find_if(begin, end,	[destination](const Edge<TEdgeWeight>& v) { return v.destination == destination; });
	}

	template<typename TVertexValue, typename TEdgeWeight>
	typename std::vector<Edge<TEdgeWeight>>::iterator Graph<TVertexValue, TEdgeWeight>::find_edge(typename std::vector<Edge<TEdgeWeight>>::iterator begin,
			typename std::vector<Edge<TEdgeWeight>>::iterator end, size_t destination)
	{
		return std::find_if(begin, end,	[destination](const Edge<TEdgeWeight>& v) { return v.destination == destination; });
	}

	template<typename TVertexValue, typename TEdgeWeight>
	Graph<TVertexValue, TEdgeWeight>::Graph(size_t num_vertices) : 
		num_vertices(num_vertices), adjacency_list(num_vertices)
	{
	}

	/**
	@brief Добавление нового ребра в граф.
	@details Граф неориентированный, поэтому метод добавляет сразу два ребра
	в список смежности: прямое и обратное с тем же весом.
	@param vertex_origin начальная вершина ребра
	@param vertex_destination конечная вершина ребра
	@param weight вес ребра
	*/
	template<typename TVertexValue, typename TEdgeWeight>
	void Graph<TVertexValue, TEdgeWeight>::add_edge(
		const size_t vertex_origin, const size_t vertex_destination, const TEdgeWeight& weight)
	{
		if(!is_edge_valid(vertex_origin, vertex_destination))
			throw vertex_out_of_range(VertexOutOfRangeText);
		
		if(find_edge(adjacency_list[vertex_origin].neighbors.begin(), adjacency_list[vertex_origin].neighbors.end(),
			vertex_destination)	!= adjacency_list[vertex_origin].neighbors.end())
			return;

		adjacency_list[vertex_origin].neighbors.push_back(Edge<TEdgeWeight>(vertex_destination, weight));
		adjacency_list[vertex_destination].neighbors.push_back(Edge<TEdgeWeight>(vertex_origin, weight));
	}

	template<typename TVertexValue, typename TEdgeWeight>
	void Graph<TVertexValue, TEdgeWeight>::remove_edge(
		const size_t vertex_origin, const size_t vertex_destination)
	{
		if(!is_edge_valid(vertex_origin, vertex_destination))
			throw vertex_out_of_range(VertexOutOfRangeText);

		std::remove_if(adjacency_list[vertex_origin].neighbors.begin(), adjacency_list[vertex_origin].neighbors.end(),
			[vertex_destination](const Edge<TEdgeWeight>& v) { return v.destination == vertex_destination; });

		std::remove_if(adjacency_list[vertex_destination].neighbors.begin(), adjacency_list[vertex_destination].neighbors.end(),
			[vertex_origin](const Edge<TEdgeWeight>& v) { return v.destination == vertex_origin; });
	}

	template<typename TVertexValue, typename TEdgeWeight>
	inline std::pair<typename Graph<TVertexValue, TEdgeWeight>::neighbor_iterator, typename Graph<TVertexValue, TEdgeWeight>::neighbor_iterator>
		Graph<TVertexValue, TEdgeWeight>::get_neighbors(const size_t vertex) const
	{
		if(vertex >= num_vertices)
			throw vertex_out_of_range(VertexOutOfRangeText);
		return std::make_pair(adjacency_list[vertex].neighbors.cbegin(), adjacency_list[vertex].neighbors.cend());
	}

	template<typename TVertexValue, typename TEdgeWeight>
	inline size_t Graph<TVertexValue, TEdgeWeight>::get_num_vertices() const
	{
		return num_vertices;
	}

	template<typename TVertexValue, typename TEdgeWeight>
	bool Graph<TVertexValue, TEdgeWeight>::contains_edge(
		const size_t vertex_origin, const size_t vertex_destination) const
	{
		if(!is_edge_valid(vertex_origin, vertex_destination))
			throw vertex_out_of_range(VertexOutOfRangeText);

		return find_edge(adjacency_list[vertex_origin].neighbors.begin(), adjacency_list[vertex_origin].neighbors.end(),
			vertex_destination)	!= adjacency_list[vertex_origin].neighbors.end();
	}

	template<typename TVertexValue, typename TEdgeWeight>
	TEdgeWeight Graph<TVertexValue, TEdgeWeight>::get_edge_weight(
		const size_t vertex_origin, const size_t vertex_destination) const
	{
		if(!is_edge_valid(vertex_origin, vertex_destination))
			throw vertex_out_of_range(VertexOutOfRangeText);

		auto edge_it = find_edge(adjacency_list[vertex_origin].neighbors.begin(), adjacency_list[vertex_origin].neighbors.end(),
			vertex_destination);

		if(edge_it == adjacency_list[vertex_origin].neighbors.end())
			throw nonexistent_edge_error(NonexistentEdgeErrorText);
	
		return edge_it->weight;
	}

	template<typename TVertexValue, typename TEdgeWeight>
	void Graph<TVertexValue, TEdgeWeight>::set_edge_weight(
		const size_t vertex_origin, const size_t vertex_destination, const TEdgeWeight& weight)
	{
		if(!is_edge_valid(vertex_origin, vertex_destination))
			throw vertex_out_of_range(VertexOutOfRangeText);

		auto edge_it = find_edge(adjacency_list[vertex_origin].neighbors.begin(), adjacency_list[vertex_origin].neighbors.end(),
			vertex_destination);

		if(edge_it == adjacency_list[vertex_origin].neighbors.end())
			throw nonexistent_edge_error(NonexistentEdgeErrorText);

		edge_it->weight = weight;
	}

	template<typename TVertexValue, typename TEdgeWeight>
	TVertexValue Graph<TVertexValue, TEdgeWeight>::get_vertex_value(
		const size_t vertex) const
	{
		if(vertex >= num_vertices)
			throw vertex_out_of_range(VertexOutOfRangeText);
		return adjacency_list[vertex].value;
	}

	template<typename TVertexValue, typename TEdgeWeight>
	void Graph<TVertexValue, TEdgeWeight>::set_vertex_value(
		const size_t vertex, const TVertexValue& value)
	{
		if(vertex >= num_vertices)
			throw vertex_out_of_range(VertexOutOfRangeText);
		adjacency_list[vertex].value = value;
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
}
#endif