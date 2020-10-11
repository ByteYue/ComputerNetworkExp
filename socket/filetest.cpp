#include"fileProcess.h"

int main(){
    string a("GET /index.html HTTP/1.1\nHost: 127.0.0.1:5050\nConnection: keep-alive\nCache-Control: max-age=0");
    fileHandler fh(a);
    fh.getFilename();
    fh.test();
    return 0;
}