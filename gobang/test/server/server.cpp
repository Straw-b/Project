#include "protocol.h"

int _add(int x, int y)
{
    std::cout << "run rpc add ..." << std::endl;
    return x + y;

}

int _sub(int x, int y)
{
    std::cout << "run rpc sub ..." << std::endl;
    return x - y;

}

int main()
{
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0){
        std::cerr << "socket error" << std::endl;
        exit(1);

    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(8888);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
        std::cerr << "bind error" << std::endl;
        exit(2);

    }
    if(listen(listen_sock, 5) < 0){
        std::cerr << "listen error" << std::endl;
        exit(3);

    }

    struct sockaddr_in peer;
    while(1){
        socklen_t len = sizeof(peer);
        int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
        if(sock < 0){
            std::cerr << "accept error" << std::endl;
            continue;

        }

        struct param pa;
        ssize_t s = recv(sock, &pa, sizeof(pa), 0);
        if(s > 0){
            std::string fun_name = pa.fun;
            int x = pa.x;
            int y = pa.y;
            int result = -1;
            if(fun_name == "add"){
                result = _add(x, y);

            }
            else if(fun_name == "sub"){
                result = _sub(x, y);

            }
            else{
                //TODO
                //            
            }
            send(sock, &result, sizeof(result), 0);
        }
        close(sock);
    }
}
