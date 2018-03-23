#include<iostream>
#include<fstream>
#include <unistd.h>
using namespace std;

extern char *optarg;
struct Room {
	short status[5];
};
struct Point {
	int x;
	int y;
};
void printHelp();
void initMaze(Room **&room, Point *Entrance, Point *Exit);
bool playing(Room **room, Point *Entrance, Point *Exit, int direction);
int main(int argc, char* argv[]) {

	
	Room **room = NULL;
	Point *Entrance = new Point, *Exit = new Point;

	char c;
	while((c=getopt(argc, argv, "hf:")) != -1){
		switch(c)
    		{
		case'f':
		initMaze(room, Entrance, Exit);
		int direction;
		while (1) {

			cin >> direction;

			if (playing(room, Entrance, Exit, direction)) {

				cout << "Finish" << endl;

				break;

			}

		}
		break;

        	case'h':
		printHelp();
		break;

		default:
		cout << "Error command" << endl;
		break;
    		}
	}

	return 0;
}
void printHelp() {
	cout << "<<Leave the maze>>" << endl;
	cout <<	"Rule:" << endl;
	cout << "1 moves to upper." << endl;
	cout << "2 moves to left." << endl;
	cout << "3 moves to right." << endl;
	cout << "4 moves to below." << endl;
}
void initMaze(Room **&room, Point *Entrance, Point *Exit) {
	int x, y;
	ifstream inputFile(optarg);

	inputFile >> x >> y;
	room = new Room*[x];
	for (int i = 0; i < y; i++)
	{
		room[i] = new Room[y];
	}


	for (int i = 0; i < x*y; i++)
	{
		for (int j = 0; j < 5; j++) {
			inputFile >> room[i / x][i%x].status[j];
			if (room[i / x][i%x].status[j] == 4)
			{
				Entrance->x = i / x;
				Entrance->y = i % x;
			}
			else if (room[i / x][i%x].status[j] == 3)
			{
				Exit->x = i / x;
				Exit->y = i % x;
			}
		}
	}
}
bool playing(Room **room, Point *Entrance, Point *Exit, int direction) {
	switch (direction)
	{
	case 1:
		if (room[Entrance->x][Entrance->y].status[0] == 2)
			Entrance->x -= 1;
		else
			cout << "Wall!" << endl;
		break;
	case 2:
		if (room[Entrance->x][Entrance->y].status[1] == 2)
			Entrance->y -= 1;
		else
			cout << "Wall!" << endl;
		break;
	case 3:
		if (room[Entrance->x][Entrance->y].status[2] == 2)
			Entrance->y += 1;
		else
			cout << "Wall!" << endl;
		break;
	case 4:
		if (room[Entrance->x][Entrance->y].status[3] == 2)
			Entrance->x += 1;
		else
			cout << "Wall!" << endl;
		break;
	default:
		break;
	}
	if (Entrance->x == Exit->x && Entrance->y == Exit->y)
		return true;
	else
		return false;
}
