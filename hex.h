#ifndef __HEX_H
#define __HEX_H
#include <cmath>
#include "graph.h"

class hexboard {
public:
	hexboard(int _size)
	{
		if(!_size) return;
		size = _size;
		empty = size*size;
		/* 4 more nodes for the sides of the board */
		board = new graph(size * size + 4);
	}
	~hexboard()
	{
		delete board;
	}
	/* if this node is uncolored, we may make a move there */
	bool is_valid_move(int i)
	{
		if(i > size*size) return false;
		return board->get_node_color(i) == 0;
	}
	void generate_connected_graph();
	void print_board();
	int convert_coords(int x, int y);
	int make_move(int node, int);
	double computer_move(int color, int start, int end);
	double montecarlo(int color, int start, int end);
	double computer_move_dijkstra(int color, int start, int end);
	int size;
	int empty;
	graph *board;
	int last;
	hexboard *clone();
	int get_random_move();
	int simulate(int i, int color, int other_player_color, int start, int end, int node);
	void sim_make_move(int node, int color, int current);
};

#endif
