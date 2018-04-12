#include<iostream>
#include<fstream>
#include<unistd.h>
#include<string>
#include<cstdio>
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

void printHelp();
void initMaze();
string playing(int direction);
int pipe(int pipefd[2]);

int main(int argc, char* argv[]) {

	int pipefd1[2],pipefd2[2];
    int pid;
    pipe(pipefd1);
	pipe(pipefd2);
    pid = fork();
	
	char c;
	while((c=getopt(argc, argv, "hf:")) != -1){
		switch(c)
    	{
		case'f':
		
		if (pid < 0)            
			printf("error in fork!");
    	else if (pid == 0) {
    		int direction;
			char result[20];
    		while (result != "Finish!") {
				cin >> direction;
				close(pipefd1[0]);
	   			write(pipefd1[1], &direction, sizeof(direction));
				
				close(pipefd2[1]);
        		read(pipefd2[0], result, sizeof(result));
				
				cout << result << endl;
			}
		}
    	else{
			int direction=0;
			string result;
			initMaze();                 
			while (result != "Finish!") {
				close(pipefd1[1]);
        		read(pipefd1[0], &direction, sizeof(direction));
				
				result.assign(playing((int)direction));
				
				close(pipefd2[0]);
				write(pipefd2[1], result.c_str(), result.length()+1);		//pipe can not transmit C++ string, so transform to char array
			}
			sleep(0.5);
   		}
		
		break;

        case'h':
        
        if(pid != 0)
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
void initMaze() {
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
}
string playing(int direction) {
	
	string result;
	switch (direction)
	{
	case 1:
		if (room[Entrance->x][Entrance->y].status[0] == 2){
			Entrance->x -= 1;
			result.assign("Turn up!");
		}
		else
			result.assign("Wall!");
		break;
	case 2:
		if (room[Entrance->x][Entrance->y].status[1] == 2){
			Entrance->y -= 1;
			result.assign("Turn left!");
		}
		else
			result.assign("Wall!");
		break;
	case 3:
		if (room[Entrance->x][Entrance->y].status[2] == 2){
			Entrance->y += 1;
			result.assign("Turn right!");
		}
		else
			result.assign("Wall!");
		break;
	case 4:
		if (room[Entrance->x][Entrance->y].status[3] == 2){
			Entrance->x += 1;
			result.assign("Turn down!");
		}
		else
			result.assign("Wall!");
		break;
	default:
		break;
	}
	if (Entrance->x == Exit->x && Entrance->y == Exit->y)
		result.assign("Finish!");

	return result;
}
