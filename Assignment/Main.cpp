#include <iostream>
#include <cstdlib> // for rand and srand functions
#include <ctime> // for time functions
#include <list> // for list
#include <chrono> // clock
#include <thread> // clock
#include <algorithm>
#include <functional>
#include <fstream> // writing to csv file
#include <vector>
#include <queue>
using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
typedef std::chrono::steady_clock the_clock;

// Size of map
#define WIDTH 750
#define HEIGHT 750

bool skip = false;
int grid[WIDTH][HEIGHT];

// Node for a* algorithim
struct Node
{
	int x, y; // X and Y of node in map
	int f, g, h; // Heuristic values - defualt to zero
	Node* parentNode; // Pointer to the node that created this node - Backpropagation
};



// Coords for lee algorithim
struct Coord
{
	int x;
	int y;
};
bool lowestF(Node* a, Node* b)
{
	return a->f < b->f;
}

// Create set of visited nodes - CLOSED SET
list <Node*> closedSet;
list <Node*> openSet;

// Functions that will be used within both algorithims

void intialisegrid()
{
	for (int coloumn = 0; coloumn < HEIGHT; coloumn++)
	{
		for (int row = 0; row < WIDTH; row++)
		{
			grid[row][coloumn] = -1;
		}
	}
}

void randobstacles(int x, int y)
{
	srand(time(0)); //intialise random num generator using time
	int r;
	for (int coloumn = 0; coloumn < HEIGHT; coloumn++)
	{
		for (int row = 0; row < WIDTH; row++)
		{
			r = rand() % 10 + 1;
			if (r > 2)
			{
				grid[row][coloumn] = -1;
			}
			else
			{
				grid[row][coloumn] = -2;
			}
		}
	}
	grid[0][0] = -1;
	grid[0][1] = -1; // So grid has more chance of having a path
	grid[x][y] = -1;
}

// Lee algorithim
void leephase1(struct Coord start, struct Coord end)
{
	grid[start.x][start.y] = 0; // Marks start point
	grid[end.x][end.y] = -3; // Marks end point
	int it = 0;
	{
		bool foundEnd = false;
		
		while (!foundEnd)
		{
			bool foundEmpty = false;
			for (int x = 0; x < WIDTH && !foundEnd; ++x)
			{
				for (int y = 0; y < HEIGHT; ++y)
				{
					if (grid[x][y] == it)
					{
						// Right
						if (x < WIDTH - 1)
						{
							if (grid[x + 1][y] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x + 1][y] == -1)
							{
								grid[x + 1][y] = it + 1;
								foundEmpty = true;
							}
						}

						// Left
						if (x > 0)
						{
							if (grid[x - 1][y] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x - 1][y] == -1)
							{
								grid[x - 1][y] = it + 1;
								foundEmpty = true;
							}
						}

						// Down
						if (y < HEIGHT - 1)
						{
							if (grid[x][y + 1] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x][y + 1] == -1)
							{
								grid[x][y + 1] = it + 1;
								foundEmpty = true;
							}
						}

						// Up
						if (y > 0)
						{
							if (grid[x][y - 1] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x][y - 1] == -1)
							{
								grid[x][y - 1] = it + 1;
								foundEmpty = true;
							}
						}
					}
				}
			}

			if (!foundEnd && !foundEmpty)
			{
				std::cout << "This maze can't be solved!" << std::endl << std::endl;
				break;
			}
			it++;
		}
		grid[end.x][end.y] = it;
	}
}

void leephase2(struct Coord start, struct Coord end)
{
	list<Coord> path;
	Coord current;
	bool foundStart = false;
	current.x = end.x;
	current.y = end.y;
	int distance = grid[end.x][end.y];
	path.push_front(current);
	clock_t s = clock(); // To ensure lee doesn't get stuck
	clock_t n;
	while (!foundStart)
	{
		n = clock();
		if (((n - s) / CLOCKS_PER_SEC) >= 1) // Sometimes the lee algortihim gets stuck so if it last for more than 1000ms the algorithm will be skipped
		  {
			cout << "This maze cant be solved" << endl;
			skip = true;
			return;
		  }
		// Right
		if (grid[current.x + 1][current.y] == distance - 1)
		{
			current.x = current.x + 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}

		// Left
		if (grid[current.x - 1][current.y] == distance - 1)
		{
			current.x = current.x - 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}

		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}

		// Up
		if (grid[current.x][current.y + 1] == distance - 1)
		{
			current.y = current.y + 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{

			path.push_front(current);
			foundStart = true;
			break;
		}

		// Down
		if (grid[current.x][current.y - 1] == distance - 1)
		{
			current.y = current.y - 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}
	}
}

void lee()
{
	struct Coord start = { 0, 0 };
	struct Coord end = { 9, 9 };
	leephase1(start, end);
	leephase2(start, end);
}

// A* algorithim
bool samePos(Node* current, Node* end)
{
	if (current->x == end->x && current->y == end->y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool validUp(Node* current)
{
	if (grid[current->x][current->y - 1] == -1)
	{
		return true;
	}
	if (current->y - 1 <= -1)
	{
		return false;
	}
	else
	{
		return false;
	}
}
	
bool validDown(Node* current)
{

	if (grid[current->x][current->y + 1] == -1)
	{
		return true;
	}
	if (current->y + 1 >= HEIGHT)
	{
		return false;
	}
	else
	{
		return false;
	}
	
}

bool validRight(Node* current)
{
	if (grid[current->x + 1][current->y] == -1)
	{
		return true;
	}
	if (current->x + 1 >= WIDTH)
	{
		return false;
	}
	else
	{
		return false;
	}
}
	

bool validLeft(Node* current)
{
	if (grid[current->x - 1][current->y] == -1)
	{
		return true;
	}
	if (current->x - 1 <= -1)
	{
		return false;
	}
	else
	{
		return false;
	}
}

int heuristic(Node* newNode, Node* end)
{
	int dx = end->x - newNode->x;
	int dy = end->y - newNode->y;
	return dx + dy;
}

void backTrack(Node* endNode, Node* startNode)
{
	if (endNode->parentNode == nullptr)
	{
		cout << "A path is not possible for this maze" << endl;
		return;
	}

	Node* i = endNode;
	while (i->x != startNode->x && i->y != startNode->y)
	{
		
		if (i->parentNode == nullptr)
		{
			cout << "A path is not possible for this maze" << endl;
			break;
		}
		i = i->parentNode;
	}
}

void astar()
{
	Node* endNode = new Node;
	endNode->x = 9;
	endNode->y = 9;
	
	Node* startNode = new Node;
	startNode->x = 0;
	startNode->y = 0;
	startNode->g = 0;
	startNode->h = heuristic(startNode, endNode);;
	startNode->f = startNode->g + startNode->h;;
	startNode->parentNode = NULL;
	
	// Add start node to available nodes
	openSet.push_back(startNode);

	intialisegrid();
	
	while (!openSet.empty())
	{
		// Get current node at the top of the set
		openSet.sort(lowestF);
		Node* currentNode = openSet.front();
		openSet.pop_front();

		if (samePos(currentNode, endNode))
		{
			endNode = currentNode;
			break;
		}

		if (validUp(currentNode))
		{
			int d = 0; // Checks to see if node should be pushed to open 
			Node* newNode = new Node();
			newNode->x = currentNode->x;
			newNode->y = currentNode->y - 1;
			newNode->g = currentNode->g + 1; // Cost of the path
			newNode->h = heuristic(newNode, endNode); // heuristic
			newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
			newNode->parentNode = currentNode;

			for (auto item : closedSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					d = 1;
				}
			}

			for (auto item : openSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					if (item->f < newNode->f)
					{
						d = 1;
					}
				}
			}
			if (d == 0)
			{
				openSet.push_back(newNode);
			}
		}

		if (validDown(currentNode))
		{
			int d = 0; // Checks to see if node should be pushed to open 
			Node* newNode = new Node();
			newNode->x = currentNode->x;
			newNode->y = currentNode->y + 1;
			newNode->g = currentNode->g + 1; // Cost of the path
			newNode->h = heuristic(newNode, endNode); // heuristic
			newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
			newNode->parentNode = currentNode;

			for (auto item : closedSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					d = 1;
				}
			}
			for (auto item : openSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					if (item->f < newNode->f)
					{
						d = 1;
					}
				}
			}
			if (d == 0)
			{
				openSet.push_back(newNode);
			}
		}

		if (validRight(currentNode))
		{
			int d = 0; // Checks to see if node should be pushed to open 
			Node* newNode = new Node();
			newNode->x = currentNode->x + 1;
			newNode->y = currentNode->y;
			newNode->g = currentNode->g + 1; // Cost of the path
			newNode->h = heuristic(newNode, endNode); // heuristic
			newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
			newNode->parentNode = currentNode;
			for (auto item : closedSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					d = 1;
				}
			}
			for (auto item : openSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					if (item->f < newNode->f)
					{
						d = 1;
					}
				}
			}
			if (d == 0)
			{
				openSet.push_back(newNode);
			}

		}

		if (validLeft(currentNode))
		{
			int d = 0; // Checks to see if node should be pushed to open 
			Node* newNode = new Node();
			newNode->x = currentNode->x - 1;
			newNode->y = currentNode->y;
			newNode->g = currentNode->g + 1; // Cost of the path
			newNode->h = heuristic(newNode, endNode); // heuristic
			newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
			newNode->parentNode = currentNode;

			for (auto item : closedSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					d = 1;
				}
			}
			for (auto item : openSet)
			{
				if (item->x == newNode->x && item->y == newNode->y)
				{
					if (item->f < newNode->f)
					{
						d = 1;
					}
				}
			}
			if (d == 0)
			{
				openSet.push_back(newNode);
			}
		}
		closedSet.push_back(currentNode);
		openSet.sort();
		openSet.unique();
		closedSet.sort();
		closedSet.unique();
	}
	backTrack(endNode, startNode);
}

int main()
{
	for (int i = 0; i < 20; i++)
	{
		skip = false;
		intialisegrid();
		randobstacles(9, 9);
		the_clock::time_point leeStart = the_clock::now();
		lee();
		the_clock::time_point leeEnd = the_clock::now();
		auto leeTime = duration_cast<milliseconds>(leeEnd - leeStart).count();
		if (skip == false)
		{
			the_clock::time_point aStart = the_clock::now();
			astar();
			the_clock::time_point aEnd = the_clock::now();
			auto aTime = duration_cast<milliseconds>(aEnd - aStart).count();
			
			cout << "The time it took for Lee was - " << leeTime << endl;
			cout << "The time it took for A* was - " << aTime << endl;
		}
		openSet.clear();
		closedSet.clear();
	}
	system("pause");
	return 0;
}