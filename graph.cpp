#include <vector>
#include <climits>
#include <set>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include <cfloat>
#include <queue>
#include "graph.h"

using namespace std;

double graph::dijkstra(int start, int end, int num_real, int color, vector<int> &prev, vector<double> &dist)
{
	dist.clear();
	dist.assign(num, INFINITY);
	std::set<int>::iterator it;
	
	dist[start] = 0;
	prev.clear();
	prev.assign(num, -1);
	int count=0;
	set<int> unvisited;
	for(int i=0;i<num;i++) {
		unvisited.insert(i);
	}
	
	while(!unvisited.empty())
	{
		/* get lowest value in dist */
		int min=-1;
		for(int i=0;i<num;i++)
		{
			if(((min == -1) || dist[i] < dist[min]) && (unvisited.find(i) != unvisited.end())) min = i;
		}
		/* get and remove the minimum of dist from the unvisited set */
		int current = *(it = unvisited.find(min));
		unvisited.erase(it);
		if(current == end)
			break;
		/* no point in continuing from here */
		if(dist[current] == INFINITY)
			break;
		/* loop through all nodes to find neighbors */
		for (int i=0;i<num;i++)
		{
			
			double length = 0;
			if(node_color[i] == 0)
				length = 1;
			else if(node_color[i] != color)
				length = INFINITY;
			if(conn[i][current] == INFINITY || conn[current][i] == INFINITY)
				length = INFINITY;
			/* if we are connected, and we aren't ignoring this path... */
			if(length != INFINITY && ((i < num_real) || i == end) && i != current)
			{
				/* calculate the distance from here */
				double newdist = dist[current] + length;
				if(newdist < dist[i])
				{
					/* swap it out if we've beaten the previous distance */
					dist[i] = newdist;
					prev[i] = current;
				}
			}
		}
		
	}
	return dist[end];
}

int graph::bfs(int start, int end, int num_real, int color)
{
	if(node_color[start] != node_color[end] && node_color[end] != color) return 0;
	std::set<int>::iterator it;
	queue<int> nodes;
	vector<int> markings;
	markings.assign(num, 0);
	nodes.push(start);
	markings[start] = 1;
	while(!nodes.empty()) {
		int t = nodes.front();
		nodes.pop();
		if(t == end)
			return 1;
		for(int i=0;i<num;i++)
		{
 			if(conn[i][t] != INFINITY && node_color[i] == color && !markings[i]) {
				markings[i] = 1;
				nodes.push(i);
			}
		}
	}
	return 0;
}

/* register these two nodes as connected. Path length doesn't matter, that's calculated via 
 * the colors of the nodes */
void graph::connect_nodes(int a, int b)
{
	conn[a][b] = 1;
 	conn[b][a] = 1;
}

