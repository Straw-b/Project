#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>

#include <json/json.h>

#define TCP_PORT 17878
#define UDP_PORT 17878

#define UDP_MAX_DATA_LEN 10240

//注册请求的数据格式
/*
 * 1.昵称
 * 2.学校
 * 3.用户的密码
 * client to server 严格按照Register格式传输注册数据
 */

struct RegisterInfo
{
    RegisterInfo()
    {
        memset(nick_name_, '\0', sizeof(nick_name_));
        memset(school_, '\0', sizeof(school_));
        memset(passwd_, '\0', sizeof(passwd_));
    }

    char nick_name_[20];
    char school_[20];
    char passwd_[20];
};

//登陆请求的数据格式
/*
 * 1.用户id
 * 2.密码
 * */

struct LoginInfo
{
    LoginInfo()
    {
        memset(passwd_, '\0', sizeof(passwd_));
    }
    uint32_t id_;
    char passwd_[20];
};


//服务端给客户端回复应答的数据格式  异议
/*
 * 1.当前的状态（注册的状态， 登陆状态）
 * 2.返回用户ID, 类似与注册完毕之后返回的"QQ"号
 * */

struct RelpyInfo
{
    int resp_status_;
    uint32_t id_;
};

//枚举状态
enum Status 
{
    REGISTER_FAILED = 0,
    REGISTRE_SUCCESS = 1,
    LOGIN_FAILED = 2,
    LOGIN_SUCCESS = 3,
    ONLINE = 4
};

//如何标识当前的请求是注册请求还是登陆请求
/*
 * 1.对于每一种请求， 在tcp层面都会发送两个tcp包
 *  第一个tcp包， 发送一个字节， 标识请求是"注册"还是"登陆"
 *  第二个tcp包， 发送具体的"注册"或者"登陆"的请求数据
 * */

enum ResqType
{
    REGISTER_RESQ = 0,
    LOGIN_RESQ
};

//双方约定UDP数据格式

class UdpMsg
{
    public:
        UdpMsg()
        {

        }

        ~UdpMsg()
        {

        }

        /*
         * 序列化接口, 就是将对象转化成为二进制的过程
         * 为了将二进制通过网络接口传输到对端
         * */
        void serialize(std::string* msg)
        {
            Json::Value json_msg;

            json_msg["nick_name"] = nick_name_;
            json_msg["school"] = school_;
            json_msg["user_id"] = user_id_;
            json_msg["msg"] = msg_;

            Json::FastWriter writer;
            *msg = writer.write(json_msg);
        }

        /*
         * 反序列化接口, 就是将二进制转化成为对象的过程
         * 为了将从网络当中接收的数据, 反序列化成为我们认识的字符串
         * */
        void deserialize(std::string msg)
        {
            Json::Reader reader;
            Json::Value val;
            reader.parse(msg, val);

            nick_name_ = val["nick_name"].asString();
            school_ = val["school"].asString();
            user_id_ = val["user_id"].asUInt();
            msg_ = val["msg"].asString();
        }
    public:
        std::string nick_name_;
        std::string school_;
        uint32_t user_id_;
        std::string msg_;
};
