#include<iostream>
#include<fstream>
#include<unistd.h>
#include<string>
#include<cstdio>
#include<sys/ipc.h>
#include<sys/msg.h>
using namespace std;

struct Room {
	short status[5];
};
struct Point {
	int x;
	int y;
};
struct msgbuf_direction
{
    long mtype;
    int direction;
};
struct msgbuf_result
{
    long mtype;
    string result;
};

Room **room = NULL;
Point *Entrance = NULL, *Exit = NULL;
extern char *optarg;
bool isInitial = false;

void printHelp();
bool initMaze();
string playing(int direction);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
void *command_Thread_func(void* i);
void *main_Thread_func(void* i);
int direction;
bool isClear = false, isEnter = false;
string result;

int main(int argc, char* argv[]) {

    int pid;
	int flag;
	//pid = fork();
	
	char c;
	while((c=getopt(argc, argv, "hf:tp")) != -1){
		switch(c)
    	{
    	case't':
    	
    	if(isInitial){
			pthread_t command_Thread, main_Thread;
	
			pthread_create(&command_Thread, NULL, &command_Thread_func, NULL);
			pthread_create(&main_Thread, NULL, &main_Thread_func, NULL);
		
			pthread_join(command_Thread, NULL);
			pthread_join(main_Thread, NULL);
		}
		else
			cout << "Please load the mazefile before playing!" << endl;
    	
    	break;
    	
    	case'p':
    	
    	break;
    	
		case'f':
		
		isInitial = initMaze();
		/*if (pid < 0)            
			printf("error in fork!");
    	else if (pid == 0) {
			struct msgbuf_direction buf_direction;
			struct msgbuf_result buf_result;
			int msqid;
			msqid = msgget((key_t)12345, 0666|IPC_CREAT);
    		if ( msqid < 0 )
    		{
				cout << "msgget error!" << endl;
        		return -1;
    		}

    		while (buf_result.result != "Finish!") {				

				buf_direction.mtype = 1;
    			cin >> buf_direction.direction;
    			flag = msgsnd( msqid, &buf_direction, sizeof(struct msgbuf_direction), 0);
    			if (flag < 0)
    			{
					cout << "send direction error" << endl;
        			return -1;
    			}

				flag = msgrcv( msqid, &buf_result, sizeof(struct msgbuf_result) ,2,0 ) ;
				if (flag < 0)
    			{
					cout << "recieve result error" << endl;
        			return -1;
    			}

				cout << buf_result.result << endl;
			}
		}
    	else{
			struct msgbuf_direction buf_direction;
			struct msgbuf_result buf_result;
			int msqid;
			msqid = msgget((key_t)12345, 0666|IPC_CREAT);
    		if ( msqid < 0 )
    		{
				cout << "msgget error!" << endl;
        		return -1;
    		}

			initMaze();                 
			while (buf_result.result != "Finish!") {
				flag = msgrcv( msqid, &buf_direction, sizeof(struct msgbuf_direction) ,1,0 );
				if (flag < 0)
    			{
					cout << "recieve direction error" << endl;
        			return -1;
    			}

				buf_result.mtype = 2;
    			buf_result.result.assign(playing(buf_direction.direction));
				flag = msgsnd( msqid, &buf_result, sizeof(struct msgbuf_result), 0);
    			if (flag < 0)
    			{
					cout << "send result error" << endl;
        			return -1;
    			}
			}
			sleep(0.5);
   		}*/
		
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

void *command_Thread_func(void* i){
	
	//initMaze();
	while (result != "Finish!") {
		pthread_mutex_lock(&mutex);
		
		cin >> direction;
		isEnter = true;
		pthread_cond_wait(&condition_var, &mutex);
		
		cout << result << endl;
		if(result == "Finish!") break;
		
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
void *main_Thread_func(void* i){

	while (result != "Finish!") {
		pthread_mutex_lock(&mutex);
	
		if(isEnter){

			result = playing(direction);
			isEnter = false;
			pthread_cond_signal(&condition_var);
		}
		
		pthread_mutex_unlock(&mutex);
		
		//if(result == "Finish!")	
	}
	return NULL;
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
