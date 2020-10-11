#include"server.h"

int main(){
    Server *s=new Server();
    if(s->ServerStartup()==-1)
        return -1;
    if(s->ListenStartup()==-1)
        return -1;
    if(s->Loop()==-1)
        return -1;
}