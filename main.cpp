#include<iostream>
#include<fstream>
#include<unistd.h>
#include<pthread.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int direction;
bool isClear = false;
extern char *optarg;
struct Room {
	short status[5];
};
struct Point {
	int x;
	int y;
};

Room **room = NULL;
Point *Entrance = new Point, *Exit = new Point;
void *command_Thread_func(void* i);
void *main_Thread_func(void* i);
void printHelp();
void initMaze(Room **&room, Point *Entrance, Point *Exit);
bool playing(Room **room, Point *Entrance, Point *Exit, int direction);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

int main(int argc, char* argv[]) {

	char c;
	while((c=getopt(argc, argv, "hf:")) != -1){
		switch(c)
    		{
		case'f':

		pthread_t command_Thread, main_Thread;
	
		pthread_create(&command_Thread, NULL, &command_Thread_func, NULL);
		pthread_create(&main_Thread, NULL, &main_Thread_func, NULL);
		
		pthread_join(command_Thread, NULL);
		pthread_join(main_Thread, NULL);
		
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
void *command_Thread_func(void* i){
	
	initMaze(room, Entrance, Exit);
	while (1) {
		pthread_mutex_lock(&mutex);
		
		cin >> direction;
		
		pthread_cond_wait(&condition_var, &mutex);
		
		if(isClear){
			cout << "Finish" << endl;
			return NULL;
		}
		
		pthread_mutex_unlock(&mutex);
	}
}
void *main_Thread_func(void* i){
	pthread_mutex_lock(&mutex);
	
	isClear = playing(room, Entrance, Exit, direction);
	pthread_cond_signal(&condition_var);
		
	pthread_mutex_unlock(&mutex);
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
		if (room[Entrance->x][Entrance->y].status[0] == 2){
			Entrance->x -= 1;
			cout << "Turn up!" << endl;
		}
		else
			cout << "Wall!" << endl;
		break;
	case 2:
		if (room[Entrance->x][Entrance->y].status[1] == 2){
			Entrance->y -= 1;
			cout << "Turn left!" << endl;
		}
		else
			cout << "Wall!" << endl;
		break;
	case 3:
		if (room[Entrance->x][Entrance->y].status[2] == 2){
			Entrance->y += 1;
			cout << "Turn right!" << endl;
		}
		else
			cout << "Wall!" << endl;
		break;
	case 4:
		if (room[Entrance->x][Entrance->y].status[3] == 2){
			Entrance->x += 1;
			cout << "Turn down!" << endl;
		}
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
