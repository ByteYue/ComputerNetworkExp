#include"server.h"

int main(){
    Server *s = new Server();
    if(s->serverStart()==-1)
        return -1;
    if(s->serverListen()==-1)
        return -1;
    
    return 0;
}