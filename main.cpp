#include<iostream>
#include<fstream>
#include<unistd.h>
#include<error.h>
#include<string.h>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#define SIZE 1024
using namespace std;

struct Room {
	short status[5];
};
struct Point {
	int x;
	int y;
};
struct msgbuf_direction {
    long mtype;
    int direction;
};
struct msgbuf_result {
    long mtype;
    string result;
};
union semun {
    int setval;
    struct semid_ds *buf;
    unsigned short *array;
};

Room **room = NULL;
Point *Entrance = NULL, *Exit = NULL;
extern char *optarg;
bool isInitial = false;

void printHelp();
bool initMaze();
string playing(int direction);
//For Thread
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
void *command_Thread_func(void* i);
void *main_Thread_func(void* i);
int direction;
bool isClear = false, isEnter = false;
string result;
//For Proccess
void command_Proccess_func();
void main_Proccess_func();

int main(int argc, char* argv[]) {

    int pid;
	int flag;
	
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
    		
		if(isInitial){
			
			pid = fork();

			if (pid < 0)            
				perror("error in fork!");
    		else if (pid == 0) {
				command_Proccess_func();
			}
			else{
				main_Proccess_func();
			}
		}
		else
			cout << "Please load the mazefile before playing!" << endl;

    	break;
    	
		case'f':
		
		isInitial = initMaze();
		
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
	}
	return NULL;
}

void command_Proccess_func(){
	char* shm_ptr;

	int shm_id = shmget((key_t)12345, SIZE, 0666 | IPC_CREAT);
	if (shm_id == -1) perror("shmget error");

	shm_ptr = (char*)shmat(shm_id, (void *)0, 0);
    if (shm_ptr == (void *)-1) perror("shmget error");
	
	int ret;
	int semid;
	semid = semget((key_t)123, 2, IPC_CREAT | 0600);
    if (semid == -1) perror("semget error");
	
	union semun sem_args;
    unsigned short array[2]={1,1};
    sem_args.array = array;
    ret = semctl(semid, 1, SETALL, sem_args);

	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
    struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
    struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
    struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};

	while(1){
		cin >> direction;

       	semop(semid, sem_opt_wait2, 1);
      	sprintf(shm_ptr, "%d", direction);
       	semop(semid, sem_opt_wakeup1, 1);

		cout << shm_ptr << endl;
       	if(strcmp(shm_ptr,"Finish!") == 0) break;
    }
}
void main_Proccess_func(){
	char* shm_ptr;

	int shm_id = shmget((key_t)12345, SIZE, 0666 | IPC_CREAT);
	if (shm_id == -1) perror("shmget error");


	shm_ptr = (char*)shmat(shm_id, (void *)0, 0);
    if (shm_ptr == (void *)-1) perror("shmget error");
	
	int semid;
	semid = semget((key_t)123, 0, IPC_CREAT | 0600);
    if (semid == -1) perror("semget error");

	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
    struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
    struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
    struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};

	while(1){
		semop(semid, sem_opt_wait1, 1);
		strcpy(shm_ptr, playing(atoi(shm_ptr)).c_str());
		semop(semid, sem_opt_wakeup2, 1);
		if(strcmp(shm_ptr,"Finish!") == 0) break;
	}
	sleep(0.5);
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
