#pragma once
/*
 * 1.针对不同的用户注册， 创建不同的玩家对象（类对象）
 * 2.管理不同的玩家对象， key-value
 * */
#include <pthread.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "log.hpp"

using namespace std;

#define PREPAREID 1000

typedef enum PlayerStatus
{
    /*
     * 不在线， 在线， 正在匹配当中， 正在游戏当中
     * */
    OFFLINE = 0,
    ONLINE,
    MATCHING,
    PLAYING
}status_t;

class Player
{
    public:
        Player()
        {}

        Player(const string& name, const string& passwd, uint32_t id)
        {
            name_ = name;
            passwd_ = passwd;
            id_ = id;

            win_count_ = 0;
            lose_count_ = 0;
            tie_count_ = 0;

            player_status_ = OFFLINE;
            room_id_ = 0;
        }


        ~Player()
        {

        }

        string& Getpasswd()
        {
            return passwd_;
        }


        void SetUserStatus(status_t s)
        {
            player_status_ = s;
        }

        int GetRate()
        {
            int total = win_count_ + lose_count_;
            if(total == 0)
            {
                return 0;
            }

            return win_count_ * 100 / total;
        }

        int GetPlayerstatus()
        {
            return player_status_;
        }

        void SetRoomId(const uint32_t room_id)
        {
            room_id_ = room_id;
        }

        uint32_t GetRoomId()
        {
            return room_id_;
        }
    private:
        string name_;
        string passwd_;
        uint32_t id_;
        /*
         * 保存玩家的胜场数
         * 保存玩家的败场数
         * 保存玩家的平局数
         *
         * 这三个数量数为了之后能够匹配相同胜率的玩家对战使用的
         * */
        int win_count_;
        int lose_count_;
        int tie_count_;

        status_t player_status_;

        uint32_t room_id_;
};



class PlayerManager
{
    public:
        PlayerManager()
        {
            player_map_.clear();
            pthread_mutex_init(&map_lock_, NULL);

            prepare_id_ = PREPAREID;
        }

        ~PlayerManager()
        {
            pthread_mutex_destroy(&map_lock_);
        }


        //本质上就是为注册新用户调用的接口
        uint32_t InsertPlayer2map(const string& name, const string& passwd)
        {
            /*
             * 1.使用name, passwd， 创建玩家对象
             * 2.将玩家对象放到map当中
             * 3.返回一个用户id
             * */
            pthread_mutex_lock(&map_lock_);
            uint32_t id = prepare_id_++;
            Player p(name, passwd, id);

            player_map_.insert({id, p});
            pthread_mutex_unlock(&map_lock_);

            return id;
        }


        uint32_t UserLogin(const uint32_t id, const string& passwd)
        {
            /*
             * 1.通过id进行查找用户是否存在
             *   用户不存在， 直接返回
             *   用户存在， 校验登录密码
             * 2.校验用户登录密码
             * 3.更改用户状态
             *
             * */
            pthread_mutex_lock(&map_lock_);

            auto iter = player_map_.find(id);
            if(iter == player_map_.end())
            {
                cout << "id is " << id <<" ,User does not exist" << endl;
                return 1;
            }

            if(iter->second.Getpasswd() != passwd)
            {
                cout << "Password Inconsistency" << endl;
                return 2;
            }

            iter->second.SetUserStatus(ONLINE);
            pthread_mutex_unlock(&map_lock_);

            return id;
        }

        void SetUserStatus(uint32_t id, status_t s)
        {
            return player_map_[id].SetUserStatus(s);
        }


        int GetPlayerRate(uint32_t id)
        {
            return player_map_[id].GetRate();
        }

        int GetPlayerstatus(uint32_t id)
        {
            return player_map_[id].GetPlayerstatus();
        }

        void SetRoomId(const uint32_t id, const uint32_t room_id)
        {
            player_map_[id].SetRoomId(room_id);
        }

        uint32_t GetRoomId(const uint32_t id)
        {
            return player_map_[id].GetRoomId();
        }
    private:
        /*
         * 管理类采用key-value的形式来进行管理
         * */
        // key : 用户id
        // value : 用户对象
        unordered_map<uint32_t, Player> player_map_;
        pthread_mutex_t map_lock_;
        //预分配的用户id, 当有用户注册的时候， 使用该变量当中的id进行分配
        uint32_t prepare_id_;

};
