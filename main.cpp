//compile with
//  g++ TcpListener_threads.cpp OcsServer.cpp main.cpp -lsqlite3 -licuuc -licudata -licui18n -lcurl -o ocsserv
//run with
//  ./webserv <portnumber> [silent]

#include "OcsServer.h"

int main(int argc, char *argv[]) {
    
    if(argc < 2) {
        std::cout << "Need to specify portnumber" << std::endl;
        return 0;
    }

    bool show_output = true;
    const char* db_path = "chu.db";
    if(argc > 2) {
        const char* lang_code = argv[2];
        if(!strcmp(lang_code, "orv")) db_path = "orv.db";
        else if(!strcmp(lang_code, "silent")) show_output = false;
    }
    if(argc > 3) {
        const char* output_flag = argv[3];
        if(!strcmp(output_flag, "silent")) show_output = false;
        else if(!strcmp(output_flag, "orv")) db_path = "orv.db";
    }
    
    int portno = atoi(argv[1]);
    OcsServer ocsserv("0.0.0.0", portno, show_output, db_path);
    if(ocsserv.init() != 0) {
        std::cout << "Init failed" << std::endl;
        return 0;
    } 
    ocsserv.run();
    return 0;
}
