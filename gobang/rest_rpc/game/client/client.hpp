#pragma once
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include "../../include/rest_rpc.hpp"
#include "../protocol.hpp"

using namespace std;
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;


class Client
{
    public:
        Client() = default;
        ~Client(){}

        void Init(string& ip,uint32_t port)
        {
            ip_ = ip;
            port_ = port;
            id_ = 0;
            room_id_ = 0;
            piece_ = 'W';
        }


        /*
         * rpc调用最关键的地方在于，需要告诉服务端， 客户端想要
         *     1.调用的函数名称， 
         *     2.以及给函数的传参， 
         *     3.以及获取返回值
         * */
        uint32_t Register() 
        {
            string name;
            cout << "please enter your nick_name: ";
            cin >> name;

            string passwd;
            cout << "please enter your passwd: ";
            cin >> passwd;

            string passwd1;
            cout << "please enter your passwd again: ";
            cin >> passwd1;

            if(passwd != passwd1)
            {
                cout << "The passwords do not match!! please enter again!" << endl;
                //约定：用户id是从1000开始进行分配
                return 1;
            }

            uint32_t id = 0;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 2;
                }

                id = client.call<uint32_t>("Rpc_Register", name, passwd);
                std::cout << id << std::endl;
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return id;
        }

        uint32_t Login()
        {
            /*
             * 1.id   2.passwd
             * */

            uint32_t id;
            cout << "please enter your ID: ";
            cin >> id;

            id_ = id;
            string passwd;
            cout << "please enter your passwd: ";
            cin >> passwd;

            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 2;
                }

                /*
                 * 期望的返回值也是返回用户id
                 *     如果返回的用户id是大于1000， 说明我们登录成功了
                 *     如果返回的用户id是小于1000， 说明我们登录失败了
                 * */
                id = client.call<uint32_t>("Rpc_Login", id, passwd);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return id;
        }



        void Game()
        {
            while(1)
            {
                PlayMenu();
                int select = 0;
                scanf("%d", &select);

                switch(select)
                {
                    case 1:
                        {
                            if(Match())
                            {
                                PlayGame();
                            }
                            else
                            {
                                cout << "匹配失败了， 请重新匹配~" << endl;
                            }
                        }
                        break;
                    case 2:
                        {
                            cout << "希望下次再来玩呀~~" << endl;
                            return;
                        }
                        break;
                    default:
                        {
                            cout << "输入的选择有误， 请重新选择！" << endl;
                        }
                        break;
                }
            }
        }
    private:


        void PlayMenu()
        {
            cout << "****************************" << endl;
            cout << "*** 1.匹配        2.退出 ***" << endl;
            cout << "****************************" << endl;
            cout << "please select: " ;
        }

        void ViewBoard(const string& borad)
        {
            cout << "    ";
            for(int i = 1; i <= BOARD_SIZE; i++)
            {
                cout << i << "   ";
            }
            cout << endl;

            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int z = 0; z < BOARD_SIZE + 1; z++)
                {
                    cout << "----";
                }
                cout << endl;

                cout << i + 1 << " | ";
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    cout << borad[i * BOARD_SIZE + j] << " | ";
                }
                cout << endl;
            }

            for(int i = 0; i < BOARD_SIZE; i++)
            {
                cout << "----";
            }
            cout << endl;
        }


        bool PushPlayer2MatchPool()
        {
            bool result = false;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 2;
                }

                result = client.call<bool>("Rpc_Match", id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return result;
        }

        int CheckReady()
        {
            int result = 0;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 2;
                }

                result = client.call<int>("Rpc_CheckReady", id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return result;
        }

        void PopPlayerformMatchPool()
        {
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return;
                }

                client.call("Rpc_PopPlayerFromMatchPool", id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }

        bool Match()
        {
            PushPlayer2MatchPool();

            int time_sec = 20;
            while(1)
            {
                //检测玩家的状态
                //   1.落单了， ONLINE ==> 1
                //   2.匹配上了， PLAYING ==> 3
                int result = CheckReady();
                if(result == 3)
                {
                    cout << "匹配上了..., 可以开始对战了" << endl;
                    return true;
                }
                else if(result == 1)
                {
                    cout << "没有匹配上..., 请重新匹配" << endl;
                }
                else
                {
                    //匹配游戏的时候只有一个人， 该玩家就在匹配池当中， 但是匹配线程一直等待
                    if(time_sec <= 0)
                    {
                        cout << "匹配超时了， 请重新匹配" << endl;
                        //将玩家从匹配池当中移除出来
                        PopPlayerformMatchPool();
                        break;
                    }
                    printf("正在匹配中.................%02d\r", time_sec--);
                    fflush(stdout);
                    sleep(1);
                }
            }
            return false;
        }




        uint32_t GetRoomId()
        {
            uint32_t room_id = 0;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 0;
                }

                room_id = client.call<uint32_t>("Rpc_GetRoomId", id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            room_id_ = room_id;
            return room_id;
        }


        char GetPiece()
        {
            char piece = ' ';
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return ' ';
                }

                piece = client.call<char>("Rpc_GetPiece", id_, room_id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            piece_ = piece;
            return piece;
        }

        string GetBoard()
        {
            string board;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return " ";
                }

                board = client.call<string>("Rpc_GetRoomBoard", room_id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return board;
        }

        char GetResult()
        {
            char result = 'C';
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 'C';
                }

                result = client.call<char>("Rpc_GetResult", room_id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return result;
        }

        bool IsMyTurn()
        {
            bool my_turn = false;
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return false;
                }

                my_turn = client.call<bool>("Rpc_IsMyTurn", room_id_, id_);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return my_turn;
        }

        bool PosIsRight(const string& board, const int x, const int y)
        {
            return board[(x - 1)* BOARD_SIZE + y - 1] == ' ' ? true : false;
        }

        char Step(const int x, const int y)
        {
            char result = 'C';
            try 
            {
                rpc_client client(ip_, port_);
                bool r = client.connect();
                if (!r) {
                    std::cout << "connect timeout" << std::endl;
                    return 'C';
                }

                result = client.call<char>("Rpc_Step", room_id_, id_, x, y);
            }
            catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return result;
        }

        /*
         * 1.需要通过id获取一下房间号， 为了后续通过房间号来获取棋盘
         * 2.可以通过用户id + 房间号， 获取拿的是什么样的棋子
         *
         * 对战阶段：循环落子的过程， 直到游戏结束
         * 3.循环落子
         *    3.1 获取棋盘
         *    3.2 展示棋盘
         *    3.3 判断是否是该自己下， 判断是否是自己的回合
         *        3.3.1 不是自己回合， 从3开始继续运行
         *        3.3.2 是自己的回合， 获取玩家下棋的位置
         *    3.4 判断用户输入的位置是否正确
         *        第一个内容：输入的位置是否有效  0 < x <= 5 || 0 < y <= 5
         *                    输入的位置是否已经有棋子了， board[x][y] 是否是 'X' 或者 'O'
         *                        是 'X' 或者 'O', 重新输入
         *                        不是，则可以告知服务端， 该玩家落子的位置
         *    3.5 判断输赢情况
         *        'X' 或者 'O' 或者 'T'， 游戏结束
         *    3.6 展示游戏结果
         * */
        void PlayGame()
        {
            uint32_t room_id = GetRoomId();

            if(room_id < 1024)
            {
                //TODO  如果获取的房间号是错误的， 需要知道我们用户目前处于什么情况
                return;
            }

            char piece = GetPiece();
            cout << "请开始下棋， 你拿的是：" << piece << endl;


            char result = 'C';
            while(1)
            {
                string board = GetBoard();
                //展示
                ViewBoard(board);

                result = GetResult();
                if(result != 'C')
                {
                    break;
                }


                if(!IsMyTurn())
                {
                    cout << "不要着急， 对方正在思考~~" << endl;
                    sleep(1);
                    continue;
                }

                int x, y;
                cout << "请输出落子位置， 格式为: x,y" << endl;
                scanf("%d,%d", &x, &y);

                if(x < 1 || x > BOARD_SIZE || y < 1 || y > BOARD_SIZE)
                {
                    cout << "输入错误，请重新输入， 输入的x, y的范围为[1, " << BOARD_SIZE << "]" << endl;
                    continue;
                }

                if(!PosIsRight(board, x, y))
                {
                    cout << "输入的位置已经有棋子了， 请重新输入" << endl;
                    continue;
                }

                result = Step(x, y);
                if(result != 'C')
                {
                    break;
                }
            }

            cout << "result: " << result << "piece: " << piece << endl; 
            if(result == piece)
            {
                cout << "恭喜你赢了， 再来一局~~" << endl; 
            }
            else if(result == 'T')
            {
                cout << "平局了， 不相上下， 再来一局~~" << endl;
            }
            else
            {
                cout << "很遗憾， 对方棋高一筹~~，再来一局" << endl;
            }

        }
    private:
        string ip_;
        uint32_t port_;
        uint32_t id_;
        uint32_t room_id_;
        char piece_;//棋子是X还是O
};
