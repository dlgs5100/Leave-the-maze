#include<iostream>
#include<fstream>
#include<unistd.h>
#include<error.h>
#include<string.h>
#include<string>
#include<cstdio>
#include<cstdlib>
#include"rpc.h"
using namespace std;

struct Room {
	short status[5];
};
struct Point {
	int x;
	int y;
};

Room **room = NULL;
Point *Entrance = NULL, *Exit = NULL;
extern char *optarg;
bool isInitial = false;

void printHelp();
bool initMaze();
void client_func();

int main(int argc, char* argv[]) {

    int pid;
	int flag;
	
	char c;
	while((c=getopt(argc, argv, "hf:")) != -1){
		switch(c)
    	{
    	
		case'f':
		
		isInitial = initMaze();
		client_func();
		
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

void client_func(){

	CLIENT *cl;
	char server[10];
	
	struct msg message, *result;
	int direction;

	strcpy(server,"localhost");
	while(1){
		cin >> direction;

       	message.type = 1;
       	message.cmd = direction;
       	message.x = Entrance->x;
       	message.y = Entrance->y;
		for(int i=0; i<5; i++)
			message.status[i] = room[Entrance->x][Entrance->y].status[i];
       	
       	cl = clnt_create(server, RPC_PROG, RPC_VERS, "tcp");
       	if( cl == NULL ) {
      		clnt_pcreateerror(server);
      		exit(1);
      	}
	 
	 	result = proc_1(&message,cl); 
	   	if( result == NULL ){
		  clnt_perror(cl, server);
		  exit(1);
		}
		
		if(result->x == Entrance->x && result->y == Entrance->y)
			cout << "Wall!" << endl;
		else if(result->x == Exit->x && result->y == Exit->y){
			cout << "Finish!" << endl;
			break;
		}
		else{
			Entrance->x = result->x;
			Entrance->y = result->y;
			if(direction == 1)
				cout << "Turn up!" << endl;
			else if(direction == 2)
				cout << "Turn left!" << endl;
			else if(direction == 3)
				cout << "Turn right!" << endl;
			else if(direction == 4)
				cout << "Turn down!" << endl;
		}
    }
}

void printHelp() {
	cout << "<<Leave the maze>>" << endl;
	cout <<	"Rule:" << endl;
	cout << "1 moves to upper." << endl;
	cout << "2 moves to left." << endl;
	cout << "3 moves to right." << endl;
	cout << "4 moves to below." << endl;
}
bool initMaze() {
	int x, y;
	ifstream inputFile(optarg);
	
	Entrance = new Point;
	Exit = new Point;

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
	
	return true;
}
