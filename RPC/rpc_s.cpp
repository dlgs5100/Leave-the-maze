#include"rpc.h"  // server

msg *proc_1_svc(msg *rcv, struct svc_req *rqstp){
   	static struct msg snd;
	   
	switch (rcv->cmd)
	{
	case 1:
		if (rcv->status[0] == 2)
			rcv->x -= 1;
		break;
	case 2:
		if (rcv->status[1] == 2)
			rcv->y -= 1;
		break;
	case 3:
		if (rcv->status[2] == 2)
			rcv->y += 1;
		break;
	case 4:
		if (rcv->status[3] == 2)
			rcv->x += 1;
		break;
	default:
		break;
	}

	snd.type = 2;
	snd.cmd = 0;
	snd.x = rcv->x;
	snd.y = rcv->y;
	
   	return(&snd);
}
