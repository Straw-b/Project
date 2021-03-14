#include "protocol.h"

int sock = -1;

int main()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        std::cerr << "socket error" << std::endl;
        exit(1);

    }
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(8888);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1"); //可以替换成远程IP

    if(connect(sock, (struct sockaddr*)&peer, sizeof(peer)) != 0){
        std::cerr << "connect error" << std::endl;
        exit(2);

    }
    int result = add(100, 200);
    std::cout << "result is : " << result << std::endl;
    //result = sub(10, 20);
    //    //std::cout << "result is : " << result << std::endl;
    close(sock);
    return 0;
}
