struct msg
{
   int type;
   int cmd;
   int x;
   int y;
   int status[5];
};
program RPC_PROG
{
   version RPC_VERS
      {
      msg PROC(msg)=1;
      }=1;
}= 0x9000;
