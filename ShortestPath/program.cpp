#include <fstream>
#include "astar.h"
#include <iostream>
#include <math.h>

using namespace std;

Graph<int,int> graph_from_stream(istream& in_stream, const int fake)
{
	int num_vertices;
	if(!(in_stream >> num_vertices))
		return Graph<int,int>(0);
	Graph<int,int> g = Graph<int,int>(num_vertices);

	int vertex_origin, vertex_destination, weight;
	while(in_stream >> vertex_origin && in_stream >> vertex_destination && in_stream >> weight)
		g.add_edge(vertex_origin, vertex_destination, weight);

	return g;
}

class AStarEuclidianHeuristic : public AStarSearch<pair<double,double>, double>::AStarDefaultHeuristic
{
	typedef pair<double,double> point;
public:
	virtual ~AStarEuclidianHeuristic() {}
	virtual double get_cost(const Graph<point, double> graph, int start, int goal) const
	{
		point start_point, goal_point;
		graph.get_vertex_value(start, start_point);
		graph.get_vertex_value(goal, goal_point);

		double x = start_point.first - goal_point.first;
		double y = start_point.second - goal_point.second;
		return sqrt(x*x + y*y);
	}
};

Graph<pair<double,double>,double> graph_from_stream(istream& in_stream, const double fake)
{
	int num_vertices;
	if(!(in_stream >> num_vertices))
		return Graph<pair<double,double>,double>(0);
	Graph<pair<double,double>,double> g = Graph<pair<double,double>,double>(num_vertices);

	int vertex; 
	double x, y;
	for(int i=1; i <= num_vertices; ++i)
	{
		in_stream >> vertex;
		in_stream >> x;
		in_stream >> y;
		g.set_vertex_value(vertex, pair<double,double>(x,y));
	}
	

	AStarEuclidianHeuristic heuristic;
	int vertex_origin, vertex_destination;
	while(in_stream >> vertex_origin && in_stream >> vertex_destination)
		g.add_edge(vertex_origin, vertex_destination, heuristic.get_cost(g, vertex_origin, vertex_destination));

	return g;
}

int main()
{
	ifstream file_stream("graph2.txt");
	double p = 0;
	Graph<pair<double,double>, double> g = graph_from_stream(file_stream, p);

	g.print(cout);

	list<int> sp;
	AStarSearch<pair<double,double>, double>::find_shortest_path(
		g, 0, 4, AStarEuclidianHeuristic(), sp, p);
	cout << p << endl;

	for(list<int>::const_iterator i=sp.begin(); i != sp.end(); ++i)
		cout << *i << " ";
	cout << endl;

	return 0;	
}