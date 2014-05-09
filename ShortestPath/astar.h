#include <set>
#include "graph.h"
#include "pqueue.h"

template<typename TVertexValue, typename TEdgeWeight>
class AStarSearch
{
	struct VertexStatus;
public:
	struct AStarDefaultHeuristic;
	static bool find_shortest_path(
		const Graph<TVertexValue, TEdgeWeight>& graph, const int start, const int goal, const AStarDefaultHeuristic& heuristic,
		std::list<int>& shortest_path, TEdgeWeight& shortest_path_cost);
};

template<typename TVertexValue, typename TEdgeWeight>
struct AStarSearch<typename TVertexValue, TEdgeWeight>::VertexStatus
{	
	int vertex;
	int status_code;
	int parent;
	TEdgeWeight cost_from_start_to_this;
	TEdgeWeight heuristic_cost_from_this_to_goal;
	TEdgeWeight heuristic_cost_from_start_to_goal;
	VertexStatus() : vertex(-1), status_code(0), parent(-1), cost_from_start_to_this(TEdgeWeight()),
		heuristic_cost_from_this_to_goal(TEdgeWeight()), heuristic_cost_from_start_to_goal(TEdgeWeight()) {}
	bool operator<(const VertexStatus& right) const { return heuristic_cost_from_start_to_goal < right.heuristic_cost_from_start_to_goal; }
	bool operator==(const VertexStatus& right) const { return vertex == right.vertex; }
};

template<typename TVertexValue, typename TEdgeWeight>
struct AStarSearch<typename TVertexValue, TEdgeWeight>::AStarDefaultHeuristic
{
	virtual ~AStarDefaultHeuristic() {}
	virtual TEdgeWeight get_cost(const Graph<TVertexValue, TEdgeWeight> graph, int start, int goal) const { return TEdgeWeight(); }
};

template<typename TVertexValue, typename TEdgeWeight>
bool AStarSearch<typename TVertexValue,TEdgeWeight>::find_shortest_path(
	const Graph<TVertexValue, TEdgeWeight>& graph, const int start, const int goal, const AStarDefaultHeuristic& heuristic,
	std::list<int>& shortest_path, TEdgeWeight& shortest_path_cost)
{
	std::vector<VertexStatus> vertices_status(graph.get_num_vertices(), VertexStatus());

	vertices_status[start].vertex = start;
	vertices_status[start].status_code = 1;
	vertices_status[start].cost_from_start_to_this = TEdgeWeight();
	vertices_status[start].heuristic_cost_from_this_to_goal = heuristic.get_cost(graph, start, goal);
	vertices_status[start].heuristic_cost_from_start_to_goal =	vertices_status[start].heuristic_cost_from_this_to_goal;

	PriorityQueue<VertexStatus> open_vertices_queue;
	open_vertices_queue.push(vertices_status[start]);

	while (!open_vertices_queue.empty())
	{
		VertexStatus open_vertex = open_vertices_queue.top();
		open_vertices_queue.pop();
		vertices_status[open_vertex.vertex].status_code = 2;

		if(open_vertex.vertex == goal)
		{			
			shortest_path_cost = open_vertex.cost_from_start_to_this;
			shortest_path.clear();
			int current_vertex = goal;
			while (current_vertex != -1)
			{
				shortest_path.push_front(current_vertex);
				current_vertex = vertices_status[current_vertex].parent;
			}
			return true;
		}		

		std::vector<Edge<TEdgeWeight>> neighbors;
		graph.get_neighbors(open_vertex.vertex, neighbors);
		for(size_t i=0; i < neighbors.size(); ++i)
		{
			int neighbor = neighbors[i].destination;
			if(vertices_status[neighbor].status_code == 2)
				continue;

			TEdgeWeight edge_weight_to_neighbor;
			graph.get_edge_weight(open_vertex.vertex, neighbor, edge_weight_to_neighbor);
			TEdgeWeight cost_from_start_to_neighbor = open_vertex.cost_from_start_to_this + edge_weight_to_neighbor;

			if(vertices_status[neighbor].status_code != 0 &&
				cost_from_start_to_neighbor >= vertices_status[neighbor].cost_from_start_to_this)
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

			vertices_status[neighbor].parent = open_vertex.vertex;
			vertices_status[neighbor].cost_from_start_to_this = cost_from_start_to_neighbor;
			vertices_status[neighbor].heuristic_cost_from_this_to_goal = heuristic.get_cost(graph, neighbor, goal);
			vertices_status[neighbor].heuristic_cost_from_start_to_goal =
				vertices_status[neighbor].cost_from_start_to_this + vertices_status[neighbor].heuristic_cost_from_this_to_goal;
			open_vertices_queue.push(vertices_status[neighbor]);
		}
	}

	return false;
}