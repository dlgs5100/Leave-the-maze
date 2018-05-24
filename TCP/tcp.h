#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<iostream>
#include<fstream>
#include<unistd.h>
#include<error.h>
#include<string.h>
#include<string>
#include<cstdio>
#include<cstdlib>
int MAXLINE=1024;

struct msg {
   int type;
   int cmd;
   int x;
   int y;
   int status[5];
};

