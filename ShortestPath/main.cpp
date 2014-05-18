#include <fstream>
#include <iostream>
#include "graphio.h"

using namespace std;

void example_graph()
{
	ifstream file_stream("graph.txt");
	if(!file_stream.is_open())
	{
		cout << "File open failed." << endl << endl;
		return;
	}

	Graph<int, int> graph =
		GraphIO::from_stream_int(file_stream);
	file_stream.close();

	graph.print(cout);

	set<int> start_group;
	set<int> goal_group;

	start_group.insert(5);
	start_group.insert(14);
	goal_group.insert(8);
	goal_group.insert(16);

	int shortest_path_cost;
	list<int> shortest_path;
	AStarSearch<int, int>::find_shortest_path(
		graph, start_group, goal_group, AStarSearch<int,int>::AStarDefaultHeuristic(),
		shortest_path, shortest_path_cost);

	cout << shortest_path_cost << endl;
	for(list<int>::const_iterator i=shortest_path.begin(); i != shortest_path.end(); ++i)
		cout << *i << " ";
	cout << endl << endl;
}

void example_graph2()
{
	ifstream file_stream("graph2.txt");
	if(!file_stream.is_open())
	{
		cout << "File open failed." << endl << endl;
		return;
	}

	Graph<pair<double,double>, double> graph =
		GraphIO::from_stream_double_double(file_stream);
	file_stream.close();

	graph.print(cout);

	set<int> start_group;
	set<int> goal_group;

	start_group.insert(0);
	goal_group.insert(4);

	double shortest_path_cost;
	list<int> shortest_path;
	AStarSearch<pair<double,double>, double>::find_shortest_path(
		graph, start_group, goal_group, AStarEuclidianHeuristic(), shortest_path, shortest_path_cost);

	cout << shortest_path_cost << endl;
	for(list<int>::const_iterator i=shortest_path.begin(); i != shortest_path.end(); ++i)
		cout << *i << " ";
	cout << endl << endl;
}

int main()
{
	example_graph();
	example_graph2();

	cin.get();
	return 0;
}