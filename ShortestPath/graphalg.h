#include <set>
#include "graph.h"
#include "pqueue.h"

class GraphAlgorithms
{
	struct VertexStatus;

public:
	/*static int A_star_search_algorythm(const Graph<std::pair<int, int>,int>& graph, const std::set<int>& first_vertice_group,
	                             const std::set<int>& second_vertice_group, std::vector<int> shortest_path);*/
	static int A_star_search_algorythm(
		const Graph<std::pair<int, int>,int>& graph, const int start, const int goal, std::vector<int> shortest_path);
};

struct GraphAlgorithms::VertexStatus
{	
	int vertex;
	int status_code;
	int g;
	int h;
	int f;
	explicit VertexStatus(const int vertex) : vertex(vertex), status_code(0), g(0), h(0), f(0)  {}
	bool operator<(const VertexStatus& right) {	return f < right.f;	}
	bool operator==(const VertexStatus& right) { return vertex < right.vertex; }

private:
	VertexStatus() : vertex(0), status_code(0), g(0), h(0), f(0)  {}
};

inline int GraphAlgorithms::A_star_search_algorythm(
	const Graph<std::pair<int, int>,int>& graph, const int start, const int goal, std::vector<int> shortest_path)
{
	std::vector<VertexStatus> vertices_status(graph.get_num_vertices(), VertexStatus(0));

	vertices_status[start].vertex = start;
	vertices_status[start].status_code = 1;
	vertices_status[start].g = 0;
	vertices_status[start].h = 0;
	vertices_status[start].f = vertices_status[start].g + vertices_status[start].h;

	PriorityQueue<VertexStatus> open_vertices_queue;
	open_vertices_queue.push(vertices_status[start]);

	while (!open_vertices_queue.empty())
	{
		VertexStatus open_vertex = open_vertices_queue.top();
		open_vertices_queue.pop();

		if(open_vertex.vertex == goal)
			return open_vertex.g;
		vertices_status[open_vertex.vertex].status_code = 2;

		std::vector<Edge<int>> neighbors;
		graph.get_neighbors(open_vertex.vertex, neighbors);
		for(size_t i=0; i < neighbors.size(); ++i)
		{
			int neighbor = neighbors[i].destination;
			if(vertices_status[neighbor].status_code == 2)
				continue;

			int edge_weight;
			graph.get_edge_weight(open_vertex.vertex, neighbor, edge_weight);
			int g = open_vertex.g + edge_weight;

			if(vertices_status[neighbor].status_code != 0 && g >= vertices_status[neighbor].g)
				continue;

			if(vertices_status[neighbor].status_code == 0)
			{
				vertices_status[neighbor].vertex = neighbor;
				vertices_status[neighbor].status_code = 1;				
			}
			else
			{
				open_vertices_queue.remove(vertices_status[neighbor]);
			}

			vertices_status[neighbor].g = g;
			vertices_status[neighbor].h = 0;
			vertices_status[neighbor].f = vertices_status[neighbor].g + vertices_status[neighbor].h;
			open_vertices_queue.push(vertices_status[neighbor]);
		}
	}
}