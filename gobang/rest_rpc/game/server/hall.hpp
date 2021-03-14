#pragma once
#include <vector>
#include "player.hpp"
#include "room.hpp"


/*
 * 由大厅来进行统筹管理，玩家管理模块（注册， 登录） + 玩家匹配 + 房间管理（对战）
 * */

#define VEC_SIZE 101

class Hall
{
    public:
        Hall()
            :match_pool_(101)
        {
            pm_ = NULL;
            match_pool_num_ = 0;

            pthread_mutex_init(&vec_lock_, NULL);
            pthread_cond_init(&vec_cond_, NULL);

            rm_mana_ = NULL;
        }

        ~Hall()
        {
            if(pm_)
            {
                delete pm_;
                pm_ = NULL;
            }

            if(rm_mana_)
            {
                delete rm_mana_;
                rm_mana_ = NULL;
            }

            pthread_mutex_destroy(&vec_lock_);
            pthread_cond_destroy(&vec_cond_);
        }

        int HallInit()
        {
            pm_ = new PlayerManager();
            if(pm_ == NULL)
            {
                return -1;
            }
            LOG(INFO, "玩家管理模块启动成功...") << endl;

            rm_mana_ = new RoomManager();
            if(rm_mana_ == NULL)
            {
                return -1;
            }
            LOG(INFO, "房间管理模块启动成功...") << endl;

            pthread_t tid;
            int ret = pthread_create(&tid, NULL, MatchServer, (void*)this);
            if(ret < 0)
            {
                LOG(ERROR, "pthread create failed") << endl;
                return -1;
            }
            LOG(INFO, "匹配线程启动成功") << endl;

            return 0;
        }

        uint32_t PlayerRegister(const string& name, const string& passwd)
        {
            return pm_->InsertPlayer2map(name, passwd);
        }

        uint32_t PlayerLogin(const uint32_t id, const string& passwd)
        {
            return pm_->UserLogin(id, passwd);
        }

        /*
         * 1.当客户端调用匹配接口的时候， 使用到的函数
         * 2.函数内部实现
         *
         *    2.1 更改用户状态为匹配中
         *    2.2 通过id查找该用户的胜率
         *    lock
         *    2.3 通过胜率获取到匹配池当中的vector
         *    2.4 将用户插入到匹配池当中
         *          在插入的时候需要判断下， 当前的匹配池当中是否有该用户
         *    unlock
         *    signal
         * */
        bool PushPlayer2MatchPool(uint32_t id)
        {
            //cout << "push " << id << " to match pool" << endl;
            pm_->SetUserStatus(id, MATCHING);

            int rate = pm_->GetPlayerRate(id);
            if(rate < 0 || rate > 100)
            {
                return false;
            }

            pthread_mutex_lock(&vec_lock_);
            auto &v = match_pool_[rate];

            for(auto it = v.begin(); it != v.end(); it++)
            {
                if(*it == id)
                {
                    pthread_mutex_unlock(&vec_lock_);
                    return true;
                }
            }
            v.push_back(id);
            match_pool_num_++;
            pthread_mutex_unlock(&vec_lock_);
            pthread_cond_signal(&vec_cond_);
            return true;
        }

        void GetMatchPlayer(vector<uint32_t>* vec)
        {
            for(int i = VEC_SIZE - 1; i >= 0; i--)
            {
                auto& v = match_pool_[i];
                if(v.empty())
                {
                    continue;
                }
                
                for(auto it = v.begin(); it != v.end(); it++)
                {
                    vec->push_back(*it);
                }
            }
        }

        void MatchPoolClear(uint32_t id)
        {
            for(int i = VEC_SIZE - 1; i >= 0; i--)
            {
                auto& v = match_pool_[i];
                if(v.empty())
                {
                    continue;
                }

                vector<uint32_t>().swap(v);
            }

            if(id >= 1000)
            {
                pm_->SetUserStatus(id, ONLINE);
            }

            match_pool_num_ = 0;
        }

        /*
         * MatchServer这个函数是线程的入口函数， 该函数完成从匹配池当中获取相近胜率的玩家列表
         * lock();
         * 1.判断匹配池当中的匹配人数
         *     1.1 如果匹配人数小于2个人， 该线程等待（条件变量）
         *         pthread_cond_wait
         *     1.2 匹配的人数是大于等2个人
         * 2.从匹配池（vector<vector<uint32_t>> 当中获取匹配序列（按照玩家胜率进行排序的）
         * 3.判断序列是偶数还是奇数
         *     3.1 如果是偶数，则直接进行两两分组
         *     3.2 如果是偶数， 肯定有一个人要轮空， 需要将这个人的状态设置成为在线状态（匹配状态->在线状态）
         * 4.两两分组， 只需要遍历序列
         *     4.1 分组成功的玩家， 更改状态（匹配状态-->游戏中）
         *     4.2 创建游戏房间
         *     4.3 对每一个用户设置房间号
         * 5.清空匹配池
         * unlock
         * */
        static void* MatchServer(void* arg)
        {
            pthread_detach(pthread_self());
            Hall* ha = (Hall*)arg;

            uint32_t last_id = 0;
            while(1)
            {
                pthread_mutex_lock(&ha->vec_lock_);
                while(ha->match_pool_num_ < 2)
                {
                    LOG(INFO, "匹配线程正在等待...") << endl;
                    pthread_cond_wait(&ha->vec_cond_, &ha->vec_lock_);
                }

                vector<uint32_t> vec;
                ha->GetMatchPlayer(&vec);

                size_t vec_size = vec.size();
                //奇数和偶数最大的区别就是 最低位的二进制不一样
                //  偶数：最低位的二进制位为0
                //  奇数：最低位的二进制位位1
                if(vec_size & 1)
                {
                    //奇数
                    last_id = vec[vec_size - 1];
                    vec_size &= (~1);
                }

                for(int i = vec_size - 1; i >= 0; i-=2)
                {
                    uint32_t player_one = vec[i];
                    uint32_t player_two = vec[i - 1];

                    LOG(INFO, "匹配上了，玩家1：") << player_one << " vs 玩家2: " << player_two << endl;
                    ha->pm_->SetUserStatus(player_one, PLAYING);
                    ha->pm_->SetUserStatus(player_two, PLAYING);
                    //  1.创建房间
                    uint32_t room_id = ha->rm_mana_->CreateRoom(player_one, player_two);
                    //  2.给玩家设置房间号
                    ha->pm_->SetRoomId(player_one, room_id);
                    ha->pm_->SetRoomId(player_two, room_id);
                    LOG(INFO, "create room for ") << player_one << " and " << player_two << " ,room id is " << room_id << endl;
                }

                //清空匹配池
                ha->MatchPoolClear(last_id);
                pthread_mutex_unlock(&ha->vec_lock_);
            }
            
            return NULL;
        }

        int GetPlayerstatus(const uint32_t id)
        {
            return pm_->GetPlayerstatus(id);
        }

        void PopPlayer(const uint32_t id)
        {
            MatchPoolClear(id);
        }

        uint32_t GetPlayerRoomId(const uint32_t id)
        {
            return pm_->GetRoomId(id);
        }

        char GetPlayerPiece(const uint32_t room_id, const uint32_t id)
        {
            return rm_mana_->GetPlayPiece(id, room_id);
        }

        string GetRoomBoard(const uint32_t room_id)
        {
            return rm_mana_->GetRoomBoard(room_id);
        }

        bool IsMyturn(const uint32_t room_id, const uint32_t id)
        {
            return rm_mana_->IsMyTurn(room_id, id);
        }

        char Step(const uint32_t room_id, const uint32_t id, const int x, const int y)
        {
            return rm_mana_->Step(room_id, id, x, y);
        }

        char GetResult(const uint32_t room_id)
        {
            return rm_mana_->GetRoomResult(room_id);
        }
    private:
        PlayerManager* pm_;
        //匹配池
        vector<vector<uint32_t> > match_pool_;
        //匹配的人数
        int match_pool_num_;
        pthread_mutex_t vec_lock_;
        //针对消费线程的条件变量
        pthread_cond_t vec_cond_;

        RoomManager* rm_mana_;

};
