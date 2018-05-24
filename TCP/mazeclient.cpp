#include"tcp.h"
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

	int	sockfd;
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		 perror("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port   = htons(13000);	

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		 perror("connect error");
	
	int direction;
	while(1){
		struct msg message, result;
		cin >> direction;

       	message.type = 1;
       	message.cmd = direction;
       	message.x = Entrance->x;
       	message.y = Entrance->y;
		for(int i=0; i<5; i++)
			message.status[i] = room[Entrance->x][Entrance->y].status[i];
       	write(sockfd, &message, sizeof(message));
		read(sockfd, &result, sizeof(result));
		
		if(result.x == Entrance->x && result.y == Entrance->y)
			cout << "Wall!" << endl;
		else if(result.x == Exit->x && result.y == Exit->y){
			message.type = 0;
			write(sockfd, &message, sizeof(message));
			cout << "Finish!" << endl;
			break;
		}
		else{
			Entrance->x = result.x;
			Entrance->y = result.y;
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
