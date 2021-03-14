#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "room.hpp"
#include "log.hpp"

using namespace std;

#define ROBOT_BEGIN_ID 10001
#define ROBOT_END_ID   13000

class RobotPool;

struct Thread_Pram
{
    Thread_Pram(uint32_t id, RobotPool* rp)
    {
        robot_id_ = id;
        rp_ = rp;
    }
    uint32_t robot_id_;
    RobotPool* rp_;
};

typedef enum HardLevel
{
    EASY = 0,
    MID,
    HARD
}hard_level;

class Robot
{
    public:
        Robot(uint32_t id, size_t win_rate,  pthread_t tid)
        {
            id_ = id;
            win_rate_ = win_rate;

            if(win_rate >= 80)
            {
                hard_level_ = HARD;
            }
            else if(win_rate_ <= 30)
            {
                hard_level_ = EASY;
            }
            else
            {
                hard_level_ = MID;
            }

            //没有对局的时候， room_id_就是非法值
            room_id_ = 0;
            self_ = tid;
            is_free_ = false;
        }

        bool IsFree()
        {
            return is_free_;
        }
    private:
        uint32_t id_;
        uint32_t room_id_;
        pthread_t self_;

        bool is_free_;
        size_t win_rate_;
        hard_level hard_level_;
};

class RobotPool
{
    public:
        RobotPool(size_t robot_num = 3)
        {
            robot_pool_.clear();
            robot_num_ = robot_num;
            free_robot_.clear();
            pthread_mutex_init(&robot_lock_, NULL);
            prepare_robot_id_ = ROBOT_BEGIN_ID;
        }

        ~RobotPool()
        {
            pthread_mutex_destroy(&robot_lock_);
        }

        int InitRobotPool()
        {
            int win_rate = 10;
            for(size_t i = 0; i < robot_num_; i++)
            {

                Thread_Pram* tp = new Thread_Pram(prepare_robot_id_, this);
                pthread_t tid;
                int ret = pthread_create(&tid, NULL, RobotStart, (void*)tp);
                if(ret < 0)
                {
                    LOG(ERROR, "create robot failed\n");
                    continue;
                }

                Robot robot(prepare_robot_id_, win_rate, tid);
                win_rate += 20;

                free_robot_.push_back(prepare_robot_id_);
                robot_pool_.insert({prepare_robot_id_++, robot});
            }
            return 0;
        }

        static void* RobotStart(void* arg)
        {
            pthread_detach(pthread_self());
            Thread_Pram* tp = (Thread_Pram*)arg;
            RobotPool* rp = tp->rp_;
            uint32_t id = tp->robot_id_;

            auto it = rp->robot_pool_.find(id);
            if(it == rp->robot_pool_.end())
            {
                pthread_mutex_lock(&rp->robot_lock_);
                rp->robot_num_--;
                pthread_mutex_unlock(&rp->robot_lock_);
                pthread_exit(NULL);
            }

            Robot rb = it->second;
            while(1)
            {
               if(!rb.IsFree())
               {
                   continue;
               }

               //直接获取棋盘， 准备下棋， 由获取机器人的线程， 更改is_free_
               
            }
        }
    private:
        unordered_map<uint32_t, Robot> robot_pool_;
        size_t robot_num_;
        vector<uint32_t> free_robot_;
        pthread_mutex_t robot_lock_;

        uint32_t prepare_robot_id_;
        RoomManager* rm_mana_;
};
