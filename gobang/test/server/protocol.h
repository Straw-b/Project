#pragma once

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//暂不考虑序列化和反序列化，简单实现
struct param{
    int x;
    int y;
    char fun[64];
};

int rpc_fun(int sock, const char *fun, int x, int y)
{
    struct param pa;
    pa.x = x;
    pa.y = y;
    strcpy(pa.fun, fun);
    send(sock, &pa, sizeof(pa), 0);
    int result = -1;
    recv(sock, &result, sizeof(result), 0);
    return result;
}

extern int sock;
#define add(x,y) rpc_fun(sock,"add", x, y)
#define sub(x,y) rpc_fun(sock,"sub", x, y)
