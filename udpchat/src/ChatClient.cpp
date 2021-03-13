#include <unistd.h>
#include "ChatClient.hpp"
#include "ChatWindows.hpp"

void Menu()
{
    std::cout << "*******************************" << std::endl;
    std::cout << "*****1.register *** 2.ligon****" << std::endl;
    std::cout << "*****3.logout   *** 4.exit ****" << std::endl;
    std::cout << "*******************************" << std::endl;
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        //  ./ChatClient -ip [ip]
        std::cout << "using ./ChatClent [ip]" << std::endl;
        return -1;
    }
    std::string ip;
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "-ip") == 0 && (i+1) < argc)
        {
            ip = argv[i + 1];
        }
    }

    if(ip.size() == 0)
    {
        LOG(ERROR, "Illegal IP, please retry start ChatClent") << std::endl;
        return -1;
    }

    UdpClient* uc = new UdpClient(ip);
    ChatWindow* cw = new ChatWindow();
    while(1)
    {
        Menu();
        int select = -1;
        std::cout << "please enter your select: ";
        fflush(stdout);
        std::cin >> select;
        if(select == 1)
        {
            int ret = uc->RegistertoSvr();
            if(ret < 0)
            {
                LOG(WARNING, "please retry register") << std::endl;
            }
            else if(ret == 0)
            {
                LOG(INFO, "register success! please login...") << std::endl;
            }
            uc->CloseFd();
        }
        else if(select == 2)
        {
            //登录的逻辑
            int ret = uc->LoginToSvr();
            if(ret < 0)
            {
                LOG(ERROR, "please retry login") << std::endl;
            }
            else if(ret == 0)
            {
                LOG(INFO, "login success, please chatting...") << std::endl;
                cw->start(uc);
            }
        }
        else if(select == 3)
        {
            //登出
        }
        else if(select == 4)
        {
            //退出的逻辑
            LOG(INFO, "exit chat client") << std::endl;
            exit(0);
        }
    }



    while(1)
    {
        sleep(1);
    }
    return 0;
}
