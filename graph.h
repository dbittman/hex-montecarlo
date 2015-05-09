#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <cmath>

const int COLOR_RED   = 1;
const int COLOR_BLUE  = 2;

class graph
{
	public:
		graph(int n) {
			num = n;
			conn.assign(n, std::vector<double>(n));
			node_color.assign(n, 0);
			node_data.assign(n, -1);
			/* nothing connected */
			for(int i=0;i<n;i++) {
				for(int j=0;j<n;j++) {
					conn[i][j] = conn[j][i] = INFINITY;
				}
			}
		}
		int size() { return num; }
		void connect_nodes(int i, int arg2);
		int get_node_color(int i)
		{
			return node_color[i];
		}
		int set_node_color(int i, int c)
		{
			node_color[i] = c;
		}
		double dijkstra(int start, int end, int, int, std::vector<int> &prev, std::vector<double> &dist);
		int bfs(int start, int end, int num_real, int color);
		int num;
		/* 2-D matrix of path lengths */
		std::vector< std::vector<double> > conn;
		/* colors of nodes */
		std::vector< int > node_color;
		std::vector< int > node_data;
};

#endif
