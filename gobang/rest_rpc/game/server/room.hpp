#pragma once
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "log.hpp"
#include "../protocol.hpp"

using namespace std;

class Room
{
    public:
        Room()
        {

        }
        Room(const uint32_t p1, const uint32_t p2, uint32_t room_id)
        {
            player_one_ = p1;
            player_two_ = p2;
            room_id_ = room_id;

            memset(board_, ' ', sizeof(board_));
            current_ = p1;

            result_ = 'C';

            piece_[0] = 'X';
            piece_[1] = 'O';
        }

        ~Room()
        {

        }

        char GetPiece(const uint32_t id)
        {
            int pos = (id == player_one_ ? 0 : 1);
            return piece_[pos];
        }

        string GetBoard()
        {
            string board;
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    board.push_back(board_[i][j]);
                }
            }
            return board;
        }

        uint32_t GetCurrect()
        {
            return current_;
        }

        char Step(const uint32_t id, const int x, const int y)
        {
            if(current_ != id)
            {
                return 'C';
            }

            char piece = ' ';
            if(id == player_one_)
            {
                piece = piece_[0];
            }
            else
            {
                piece = piece_[1];
            }

            board_[x - 1][y - 1] = piece;

            //要改变下棋的人
            ChangeCurrent();

            result_ = IsOver(x - 1, y - 1);
            return result_;
        }

        void ChangeCurrent()
        {
            current_ = (current_ == player_one_ ? player_two_ : player_one_);
        }

        char IsOver()
        {
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                if(board_[i][0] != ' ' && board_[i][0] == board_[i][1] && board_[i][1] == board_[i][2]
                        && board_[i][2] == board_[i][3] && board_[i][3] == board_[i][4])
                {
                    return board_[i][0];
                }
            }

            for(int i = 0; i < BOARD_SIZE; i++)
            {
                if(board_[0][i] != ' ' && board_[0][i] == board_[1][i] && board_[1][i] == board_[2][i]
                        && board_[2][i] == board_[3][i] && board_[3][i] == board_[4][i])
                {
                    return board_[0][i];
                }
            }

            if(board_[0][0] != ' ' && board_[0][0] == board_[1][1] && board_[1][1] == board_[2][2] && board_[2][2] == board_[3][3] && board_[3][3] == board_[4][4])
            {
                return board_[0][0];
            }

            if(board_[0][4] != ' ' && board_[0][4] == board_[1][3] && board_[1][3] == board_[2][2] &&       board_[2][2] == board_[3][1] && board_[3][1] == board_[4][0])
            {
                return board_[0][4];
            }

            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    if(board_[i][j] == ' ')
                    {
                        return 'C';
                    }
                }
            }

            return 'T';
        }

        char JudgeRowOrCol(const string& board)
        {
            char row_tmp[BOARD_SIZE] = {0};
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                memset(row_tmp, '\0', BOARD_SIZE);
                strncpy(row_tmp, board.c_str() + (i * BOARD_SIZE), BOARD_SIZE);

                char* ret = strstr(row_tmp, "XXXXX");
                if(ret != NULL)
                {
                    return 'X';
                }

                ret = strstr(row_tmp, "OOOOO");
                if(ret != NULL)
                {
                    return 'O';
                }
            }
            return 'C';
        }

        //调用传参的时候， 需要 x-1， y-1
        char IsOver(const int x, const int y)
        {
            //1.判断行当中是否有连续的 黑子（X） 或者 白子（O）
            string row_board = GetBoard();
            char ret = JudgeRowOrCol(row_board);
            if(ret == 'X' || ret == 'O')
            {
                return ret;
            }

            //2.判断列是否有连续的 黑子或者白子
            string col_board;
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    //[0][0] [1][0] ..
                    col_board.push_back(board_[j][i]); 
                }
            }

            ret = JudgeRowOrCol(col_board);
            if(ret == 'X' || ret == 'O')
            {
                return ret;
            }

            //3.判断是否有对角线 连成了5个黑子或者白子
            //   3.1 判断反斜线
            vector<char> vec;
            char bash_char = board_[x][y];
            vec.clear();
            vec.push_back(bash_char);
            int tmp_x = x;
            int tmp_y = y;

            while(tmp_x - 1 >= 0 && tmp_y - 1 >= 0)
            {
                if(board_[tmp_x - 1][tmp_y - 1] == bash_char)
                {
                    vec.push_back(board_[tmp_x - 1][tmp_y - 1]);
                    tmp_x -= 1;
                    tmp_y -= 1;
                }
                else
                {
                    break;
                }
            }
            tmp_x = x;
            tmp_y = y;

            while(tmp_x + 1 < BOARD_SIZE && tmp_y + 1 < BOARD_SIZE)
            {
                if(board_[tmp_x + 1][tmp_y + 1] == bash_char)
                {
                    vec.push_back(board_[tmp_x + 1][tmp_y + 1]);
                    tmp_x += 1;
                    tmp_y += 1;
                }
                else
                {
                    break;
                }
            }
            tmp_x = x;
            tmp_y = y;
            if(vec.size() >= 5)
            {
                return bash_char;
            }


            //3.2 判断正斜线
            vec.clear();
            vec.push_back(bash_char);

            while(tmp_x - 1 >= 0 && tmp_y + 1 < BOARD_SIZE)
            {
                if(board_[tmp_x - 1][tmp_y + 1] == bash_char)
                {
                    vec.push_back(board_[tmp_x - 1][tmp_y + 1]);
                    tmp_x -= 1;
                    tmp_y += 1;
                }
                else
                {
                    break;
                }
            }
            tmp_x = x;
            tmp_y = y;
            
            while(tmp_x + 1 < BOARD_SIZE && tmp_y - 1 >= 0)
            {
                if(board_[tmp_x + 1][tmp_y - 1] == bash_char)
                {
                    vec.push_back(board_[tmp_x + 1][tmp_y - 1]);
                    tmp_x += 1;
                    tmp_y -= 1;
                }
                else
                {
                    break;
                }
            }
            tmp_x = x;
            tmp_y = y;

            if(vec.size() >= 5)
            {
                return bash_char;
            }


            //判断是否棋盘已经下满了
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    if(board_[i][j] == ' ')
                    {
                        return 'C';
                    }
                }
            }
            return 'T';
        }

        char GetResult()
        {
            return result_;
        }

    private:
        uint32_t player_one_;
        uint32_t player_two_;
        uint32_t room_id_;

        char board_[BOARD_SIZE][BOARD_SIZE];
        //保存当前是哪一个玩家该落棋
        uint32_t current_;

        //游戏的结果
        //   1.黑子赢 X
        //   2.白子赢 O
        //   3.平局  T
        //   4.还可以继续下 C
        char result_;

        //p1 ==> piece[0] ==> 'X'
        //p2 ==> piece[1] ==> 'O'
        char piece_[2];
};

class RoomManager
{
    public:
        RoomManager()
        {
            map_room_.clear();
            prepare_room_id_ = PREPARE_ROOM_ID;
            pthread_mutex_init(&room_lock_, NULL);
        }

        ~RoomManager()
        {
            pthread_mutex_destroy(&room_lock_);
        }

        uint32_t CreateRoom(const uint32_t p1, const uint32_t p2)
        {
            pthread_mutex_lock(&room_lock_);
            uint32_t room_id = prepare_room_id_++;
            Room r(p1, p2, room_id);
            map_room_.insert({room_id, r});
            pthread_mutex_unlock(&room_lock_);

            return room_id;
        }


        char GetPlayPiece(const uint32_t id, const uint32_t room_id)
        {

            char piece = map_room_[room_id].GetPiece(id);
            cout << id << "想要piece: " << piece << endl;
            return piece;
        }

        string GetRoomBoard(const uint32_t room_id)
        {
            return map_room_[room_id].GetBoard();
        }

        bool IsMyTurn(const uint32_t room_id, const uint32_t id)
        {
            return map_room_[room_id].GetCurrect() == id ? true : false;
        }

        char Step(const uint32_t room_id, const uint32_t id, const int x, const int y)
        {
            char result = map_room_[room_id].Step(id, x, y);
            if(result != 'C')
            {
                pthread_mutex_lock(&room_lock_);
                map_room_.erase(room_id);
                pthread_mutex_unlock(&room_lock_);
            }
            return result;
        }

        char GetRoomResult(const uint32_t room_id)
        {
            return map_room_[room_id].GetResult();
        }
        
    private:
        unordered_map<uint32_t, Room> map_room_;
        uint32_t prepare_room_id_;

        pthread_mutex_t room_lock_;
};
