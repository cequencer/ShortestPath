#include <fstream>
#include "graphalg.h"
#include <iostream>

using namespace std;

Graph<pair<int,int>,int> graph_from_stream(istream& in_stream)
{
	int num_vertices;
	if(!(in_stream >> num_vertices))
		return Graph<pair<int,int>,int>(0);
	Graph<pair<int,int>,int> g = Graph<pair<int,int>,int>(num_vertices);

	int vertex_origin, vertex_destination, weight;
	while(in_stream >> vertex_origin && in_stream >> vertex_destination && in_stream >> weight)
		g.add_edge(vertex_origin, vertex_destination, weight);

	return g;
}

int main()
{
	ifstream file_stream("graph.txt");
	Graph<pair<int,int>,int> g = graph_from_stream(file_stream);

	g.print(cout);

	list<int> sp;
	int p = GraphAlgorithms::A_star_search_algorythm(g, 1, 14, sp);
	cout << p << endl;

	for(list<int>::const_iterator i=sp.begin(); i != sp.end(); ++i)
		cout << *i << " ";
	cout << endl;

	return 0;	
}