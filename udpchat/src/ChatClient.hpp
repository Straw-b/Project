#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ConnectInfo.hpp"
#include "tools.hpp"
#include "UserManager.hpp"

struct MySelf
{
    std::string nick_name_;
    std::string school_;
    std::string passwd_;
    uint32_t user_id_;
};

class UdpClient
{
    public:
        UdpClient(const std::string& ip)
        {
            tcp_sock_ = -1;
            udp_sock_ = socket(AF_INET, SOCK_DGRAM, 0);
            if(udp_sock_ < 0)
            {
                LOG(ERROR, "create udp socket failed") << std::endl;
                exit(0);
            }

            ip_ = ip;

            vec_.clear();
        }

        ~UdpClient()
        {

        }

        int CreateSock()
        {
            tcp_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
            if(tcp_sock_ < 0)
            {
                LOG(ERROR, "create socket failed") << std::endl;
                return -1;
            }
            /*
             * 为了客户端可以在同一台机器上面多开, 我们不主动绑定端口
             * 让操作系统进行绑定
             * */
            return 0;
        }

        //ip 是服务端的ip地址
        //服务端的ip地址已经约定好了, 在ConnectInfo这个文件当中
        int ConnectoSvr()
        {
            struct sockaddr_in dest_addr;
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(TCP_PORT);
            dest_addr.sin_addr.s_addr = inet_addr(ip_.c_str());

            int ret = connect(tcp_sock_, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
            if(ret < 0)
            {
                LOG(ERROR, "Connect server failed, addr is") << ip_ << ":" << TCP_PORT << std::endl;
                return -1;
            }
            return 0;
        }

        int RegistertoSvr()
        {
            //1.创建套接字
            int ret = CreateSock();
            if(ret < 0)
            {
                return -1;
            }
            //2.连接服务端
            ret = ConnectoSvr();
            if(ret < 0)
            {
                return -1;
            }
            //3.准备注册包
            char type = REGISTER_RESQ;
            ssize_t send_size = send(tcp_sock_, &type, 1, 0);
            if(send_size < 0)
            {
                return -1;
            }

            struct RegisterInfo ri;
            std::cout << "please enter nick-name: ";
            fflush(stdout);
            std::cin >> ri.nick_name_;
            me_.nick_name_ = ri.nick_name_;
            std::cout << "please enter school: ";
            fflush(stdout);
            std::cin >> ri.school_;
            me_.school_ = ri.school_;
            /*
             * 对于密码字段而言, 我们需要进行一个双重校验, 防止用户在输入密码的时候, "手心"不一致
             * */
            while(1)
            {
                std::string first_passwd;
                std::string second_passwd;
                std::cout << "please enter your passwd: ";
                fflush(stdout);
                std::cin >> first_passwd;
                std::cout << "please retry enter your passwd: ";
                fflush(stdout);
                std::cin >> second_passwd;
                if(first_passwd == second_passwd)
                {
                    strncpy(ri.passwd_, first_passwd.c_str(), sizeof(ri.passwd_));
                    me_.passwd_ = first_passwd;
                    break;
                }
            }
            //4.发送注册包
            send_size = send(tcp_sock_, &ri, sizeof(ri), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send register failed") << std::endl;
                return -1;
            }
            //5.接收应答
            struct RelpyInfo reply_info;
            ssize_t recv_size = recv(tcp_sock_, &reply_info, sizeof(reply_info), 0);
            if(recv_size < 0)
            {
                LOG(ERROR, "recv register response failed") << std::endl;
                return -1;
            }
            else if(recv_size == 0)
            {
                LOG(ERROR,"udpchat server shutdown connect") << std::endl;
                CloseFd();
                return -1;
            }

            //6.判断应答结果
            if(reply_info.resp_status_ == REGISTER_FAILED)
            {
                LOG(ERROR, "server respnese register failed") << std::endl;
                return -1;
            }
            //7.返回给上层调用者注册的结果
            LOG(INFO, "register success") << std::endl;
            me_.user_id_ = reply_info.id_;
            return 0;
        }

        int LoginToSvr()
        {
            //1.创建套接字
            int ret = CreateSock();
            if(ret < 0)
            {
                return -1;
            }
            //2.连接服务端
            ret = ConnectoSvr();
            if(ret < 0)
            {
                return -1;
            }

            //3.发送类型
            char type = LOGIN_RESQ;
            ssize_t send_size = send(tcp_sock_, &type, 1, 0);
            if(send_size < 0)
            {
                LOG(ERROR, "send login packet failed") << std::endl;
                return -1;
            }
            //4.发送登录包
            
            struct LoginInfo ri;
            ri.id_ = me_.user_id_;
            strncpy(ri.passwd_, me_.passwd_.c_str(), sizeof(ri.passwd_));
            send_size = send(tcp_sock_, &ri, sizeof(ri), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "send login packet failed") << std::endl;
                return -1;
            }
            //5.接收应答
            
            struct RelpyInfo reply_info;
            ssize_t recv_size = recv(tcp_sock_, &reply_info, sizeof(reply_info), 0);
            if(recv_size < 0)
            {
                LOG(ERROR, "recv failed") << std::endl;
            }
            else if(recv_size == 0)
            {
                CloseFd();
                LOG(ERROR, "server shutdown connect") << std::endl;
                return -1;
            }
            //6.分析应答数据
            if(reply_info.resp_status_ != LOGIN_SUCCESS)
            {
                LOG(ERROR, "recv status not LOGIN_SUCCESS")<< std::endl;
                return -1;
            }

            LOG(INFO, "login success") << std::endl;
            return 0;
        }

        void CloseFd()
        {
            if(tcp_sock_ > 0)
            {
                close(tcp_sock_);
                tcp_sock_ = -1;
            }
        }

        int SendUdpMsg(const std::string& msg)
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(UDP_PORT);
            addr.sin_addr.s_addr = inet_addr(ip_.c_str());

            ssize_t send_size = sendto(udp_sock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&addr, sizeof(addr));
            if(send_size < 0)
            {
                LOG(ERROR, "send udp msg failed") << std::endl;
            }
            return 0;
        }

        int RecvUdpMsg(std::string* msg)
        {
            char buf[UDP_MAX_DATA_LEN] = { 0 };
            ssize_t recv_size = recvfrom(udp_sock_, buf, sizeof(buf) - 1, 0, NULL, NULL);
            if(recv_size < 0)
            {
                return -1;
            }
            msg->assign(buf, strlen(buf));
            return 0;
        }
        
        MySelf& GetMe()
        {
            return me_;
        }

        std::vector<UdpMsg>& GetVec()
        {
            return vec_;
        }

        void SetOnlineUserInfo(UdpMsg um)
        {

        }
    private:
        int tcp_sock_;
        MySelf me_;
        int udp_sock_;
        std::string ip_;
        std::vector<UdpMsg> vec_;
        //std::unordered_map<uint32_t, UserInfo> map_;
};
