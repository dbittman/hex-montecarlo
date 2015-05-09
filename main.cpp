#include <iostream>
#include <cmath>
#include <vector>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <cfloat>
#include <cstring>
#include <set>
#include <cstdio>
#include "graph.h"
#include "hex.h"
using namespace std;
/* which sides are which based on the length of a side of a board */
#define SIDE_TOP(n) n*n
#define SIDE_RIGHT(n) n*n+1
#define SIDE_BOTTOM(n) n*n+2
#define SIDE_LEFT(n) n*n+3
/* set to 1 for TRUE */
int debug = 0;
int num_iterations = 2000;

void hexboard::generate_connected_graph()
{
	/* the sides of the board are nodes from size*size through size*size+3 (aka, the 4 nodes
	 * that are greater than 0-n*n).
	 */
	int n = size;
	/*
	 * 0 1 2 3
	 *  4 5 6 7
	 *   8 9 10 11
	 *     12 13 14 15
	 * a node will connect to nodes (N=node, RW=row width)
	 *    N-RW
	 *    N-RW+1
	 *    N-1 
	 *    N+1
	 *    N+RW-1
	 *    N+RW
	 */
	/* Connect each node with it's neighbors */
	for (int i=0;i<n*n;i++)
	{
		if(!((i+1) % n == 0) && !(i < n))
			board->connect_nodes(i, i-n + 1);
		if(!(i%n == 0) && !(i+n >= n*n)/*not on left edge and not bottom row*/)
			board->connect_nodes(i, i+n - 1);
		if(!(i < n)/*not top row*/)
			board->connect_nodes(i, i-n);
		else
			board->connect_nodes(i, SIDE_TOP(n));
		if(!((i+1) % n == 0)/*not on right edge */)
			board->connect_nodes(i, i+1);
		else
			board->connect_nodes(i, SIDE_RIGHT(n));
		if(!(i%n == 0)/*not on left edge */)
			board->connect_nodes(i, i-1);
		else
			board->connect_nodes(i, SIDE_LEFT(n));
		if(!(i+n >= n*n)/*not bottom row*/)
			board->connect_nodes(i, i+n);
		else
			board->connect_nodes(i, SIDE_BOTTOM(n));
		
	}
	/* 'fake' nodes that are the colored sides of the board */
	board->set_node_color(SIDE_BOTTOM(n), COLOR_RED);
	board->set_node_color(SIDE_TOP(n), COLOR_RED);
	board->set_node_color(SIDE_LEFT(n), COLOR_BLUE);
	board->set_node_color(SIDE_RIGHT(n), COLOR_BLUE);
	/* all other nodes are white */
}

/* this ugly-ass function prints out the board in a nice hex-format
 * type of thing. Lots of formatting loops */
void hexboard::print_board()
{
	printf("     ");
	for(int i=0;i<size;i++)
	{
		printf("%-3d ", i);
	}
	printf("\n     ");
	for(int i=0;i<size;i++)
	{
		printf("\x1B[1;31mR   ");
	}
	cout << endl;
	printf("\x1B[37m");
	for(int i=0;i<size;i++)
	{
		for(int j=0;j<i;j++)
			cout << "  ";
		printf("%2d \x1B[34mB", i);
		for(int j=0;j<size;j++)
		{
			cout << " ";
			
			switch(board->get_node_color(convert_coords(j, i)))
			{
				case 0:
					printf("\x1B[37m");
					cout << ".";
					break;
				case COLOR_BLUE:
					printf("\x1B[34mB");
					break;
				case COLOR_RED:
					if(convert_coords(j, i) == last)
						printf("\x1B[4;31mR\x1B[0;1m");
					else
						printf("\x1B[31mR");
					break;
			}
			printf("\x1B[37m");
			cout << "  ";
		}
		printf("\x1B[34mB");
		printf("\x1B[37m %d", i);
		cout << endl;
	}
	
	for(int i=0;i<size-1;i++)
	{
		cout << "  ";
	}
	printf("     ");	
	for(int i=0;i<size;i++)
	{
		printf("\x1B[31mR   ");
	}
	cout << endl;
	printf("\x1B[37m");
	for(int i=0;i<size-1;i++)
	{
		cout << "  ";
	}
	printf("     ");	
	for(int i=0;i<size;i++)
	{
		printf("%-3d ", i);
	}
	printf("\n");
}

int hexboard::convert_coords(int x, int y)
{
 	/* convert from cartesian to array index */
	return x + y*size;
}

int hexboard::make_move(int node, int color)
{
	/* if it's a valid move, set the color of the node */
	if(!is_valid_move(node)) return 0;
	board->set_node_color(node, color);
	board->node_data[node]=0;
	last = node;
	empty--;
	return 1;
}

void hexboard::sim_make_move(int node, int color, int current)
{
	/* if it's a valid move, set the color of the node. here we check if
	 * the "node data" has not been set by 'real' moves or by the current
	 * simulation. This optimizes it quite a bit, since we don't have to 
	 * clone a board for each iteration of the simulation */
	if(!board->node_data[node] || board->node_data[node] == current) return;
	board->set_node_color(node, color);
	board->node_data[node]=0;
	empty--;
}

int hexboard::get_random_move()
{
	vector<int> potential_moves;
	for(int i=0;i<size*size;i++)
	{
		if(is_valid_move(i))
			potential_moves.insert(potential_moves.begin(), i);
	}
	int x = rand() % potential_moves.size();
	int node = potential_moves[x];
	return node;
}

hexboard *hexboard::clone()
{
	/* copy all of the data and return a new hexboard */
	hexboard *h = new hexboard(0);
	h->size = size;
	h->empty = empty;
	h->board = new graph(size * size + 4);
	for (int i = 0;i<size*size+4;i++)
	{
		for (int j = 0;j<size*size+4;j++)
		{
			h->board->conn[i][j] = board->conn[i][j];
		}
		h->board->node_color[i] = board->node_color[i];
		h->board->node_data[i] = board->node_data[i];
	}
	return h;
}

int hexboard::simulate(int sim_num, int color, int other_player_color, int start, int end, int node)
{
	vector<int> potential_moves;
	/* a list of empty spaces on the board */
	for(int i=0;i<size*size;i++)
	{
		/* don't include the initial move */
		if(board->node_data[i] != 0 && i != node)
			potential_moves.insert(potential_moves.begin(), i);
	}
	/* make the initial move */
	sim_make_move(node, color, sim_num);
 	while(potential_moves.size() > 0 && empty > 0)
	{
		/* pick a random move */
		int m = rand() % potential_moves.size();
		node = potential_moves[m];
		vector<int>::iterator it;
		it = potential_moves.begin() + m;
		/* remove it from the empty spaces list */
		potential_moves.erase(it);
		/* other player makes a move */
		sim_make_move(node, other_player_color, sim_num);
		/* record that this move was done by this iteration of the simulation */
		board->node_data[node] = sim_num;
		/* filled up yet? */
		if(empty == 0 || potential_moves.size() == 0) break;
		/* pick a random move for the computer */
		m = rand() % potential_moves.size();
		node = potential_moves[m];
		it = potential_moves.begin() + m;
		potential_moves.erase(it);
		/* make the computer's random move, and record that it was done by this iteration */
		sim_make_move(node, color, sim_num);
		board->node_data[node] = sim_num;
	}
	/* search if we've won or not */
	int result = board->bfs(start, end, size*size, color);
	if(result == 1) 
		return 1;
	else
		return -1;
}

double hexboard::montecarlo(int color, int start, int end)
{
	vector<int> list;
	vector<double> dist;
	/* Make sure that the game wasn't won by the human player yet */
	double ret = board->dijkstra(start, end, size*size, color, list, dist);
	if(ret == INFINITY) return -1;
	else if(ret == 0) return 1;
	/* human's color */
	int other_player_color = (color == COLOR_BLUE ? COLOR_RED : COLOR_BLUE);
	
	/* make a list of all the potential moves */
	set<int> potential_moves;
	for(int i=0;i<size*size;i++)
	{
		if(is_valid_move(i))
			potential_moves.insert(i);
	}
	
	/* go through all of them and find the one with the highest winning probability */
	set<int>::iterator it;
	int best_move=*(potential_moves.begin());
	double best_move_val=0;
	for(it = potential_moves.begin(); it != potential_moves.end(); ++it)
	{
		int node = *it;
		int wins=0, losses=0;
		/* clone the board, and make a a backup of some data so that
		 * we can re-use this cloned board without making a new one
		 * for each simulation */
		hexboard *sim = clone();
		int old_empty = sim->empty;
		for(int i=1;i<=num_iterations;i++) {
			sim->empty = old_empty;
			int res = sim->simulate(i, color, other_player_color, start, end, node);
			if(res == 1) wins++;
			else losses++;
		}
		delete sim;
		double p = ((double)wins*100 / ((double)wins+losses));
		
		cout << "[" << node << " / " << size*size << "]: " << p;
		if(debug) cout << endl;
		else cout << "                                  \r";
		fflush(0);
		/* better than before? */
		if(p > best_move_val) {
			best_move_val = p;
			best_move = node;
		}
	}
	cout << "                                  \rmoving: " << best_move << " with confidence " << best_move_val << endl;
	/* actually make the move */
	make_move(best_move, color);
	/* Use dijkstra to get a lot more information on if someone has won */
	ret = board->dijkstra(start, end, size*size, color, list, dist);
	if(ret == INFINITY) return -1;
	else if(ret == 0) return 1;
	return 0;
}

double hexboard::computer_move(int color, int start, int end)
{
	return montecarlo(color, start, end);
}

int main(int argc, char **argv)
{
	int size = 11;
	if(argc > 1)
		size = atoi(argv[1]);
	srand(time(NULL));
	
	int computer_color = COLOR_RED, computer_start_node = SIDE_TOP(size), computer_end_node = SIDE_BOTTOM(size);
	int player_color = COLOR_BLUE;
	hexboard h(size);
	h.generate_connected_graph();
	cout << "hex! Player is BLUE, computer is RED" << endl;
	cout << "board size is: " << size << endl;
	cout << "connections are as follows:" << endl;
	cout << "  *   *" << endl;
	cout << "   \\ /" << endl;
	cout << "* - * - *" << endl;
	cout << "   / \\" << endl;
	cout << "  *   *" << endl;
	cout << "to move, type coordinates of form \"i j\", where i is the vertical axis and j is the horizontal axis" << endl;
	cout << "who will go first (C/h)? ";
	char input;
	cin >> input;
	/* who's going first? */
	int computer_first = ((input == 'C' || input == 'c') ? 1 : 0);
	cout << "ok, " << (computer_first ? "computer" : "player") << " will go first" << endl;
	
	if(computer_first)
	{
		h.computer_move(computer_color, computer_start_node, computer_end_node);
		h.print_board();
		cout << "pie rule! Would you like to switch sides (Y/n)? ";
		cin >> input;
		if(input == 'Y' || input == 'y')
		{
			/* switch the computer's start and end sides to the others. Also switch
			 * the color of the computer */
			computer_color = COLOR_BLUE;
			computer_end_node = SIDE_LEFT(size);
			computer_start_node = SIDE_RIGHT(size);
			/* player switches sides too */
			player_color = COLOR_RED;
			/* computer is now 'second player' so it gets it's move now */
			h.computer_move(computer_color, computer_start_node, computer_end_node);
		}
		/* notify the user that they've changed colors */
		if(input == 'Y' || input == 'y')
			cout << "you are now RED" << endl;
	}
	/* now, print the board here so that we don't needlessly print it later */
	h.print_board();
	while(true) {
		cout << "Enter your move: ";
		char move[16];
		int x, y;
		/* theres probably a C++-y nice clean way of doing this, but I'm tired and
		 * this works quite well. */
		cin >> move;
		if(cin.eof()) break;
		char *sep = strchr(move, ',');
		if(!sep) {
			char move2[16];
			cin >> move2;
			y = atoi(move2);
		} else {
			*sep = 0;
			y = atoi(sep+1);
		}
		x = atoi(move);
		/* continue if it's an invalid move */
		if(!h.make_move(h.convert_coords(y, x), player_color)) {
			cout << "invalid move!" << endl;
			continue;
		}
		/* ...and now it's the computer's turn */
		double ret = h.computer_move(computer_color, computer_start_node, computer_end_node);
		h.print_board();
		/* the computer's move function calculates the end of the game for us */
		if(ret == -1)
		{
			cout << "you won!!" << endl;
			break;
		} else if(ret == 1)
		{
			cout << "computer won!!" << endl;
			break;
		}
		
	}
	return 0;
}
