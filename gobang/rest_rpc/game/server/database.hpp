#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "log.hpp"
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

#define JudgeUserInfo "select * from reg_userinfo where email='%s'"
#define juage_stu_exist "select stu_id from stu_info where stu_id=%s"
#define GETONESTUINFO "select * from stu_info where stu_id=\'%s\'"

#define JUDGEVALUE(p) ((p != NULL) ? p : "")

class DataBaseServer
{
    public:
        DataBaseServer(std::string& host, std::string user, std::string passwd, std::string db, uint16_t port = 3306)
        {
            host_ = host;
            user_ = user;
            passwd_ = passwd;
            db_ = db;
            port_ = port;

            mysql_init(&mysql_);
        }

        ~DataBaseServer()
        {
            FreeConnect();
        }

        bool Connect2Mysql()
        {
            if(!mysql_real_connect(&mysql_, host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), port_, NULL, CLIENT_FOUND_ROWS)) 
            {
                LOG(ERROR, "connect database error") << mysql_error(&mysql_) << std::endl;
                return false;

            }
            return true;

        }

        bool QueryOneStuinfo(std::string user_id, Json::Value* result)
        {
            mysql_query(&mysql_, "set names utf8");

            char sql[1024] = {0};
            snprintf(sql, sizeof(sql), GETONESTUINFO, user_id.c_str());

            int ret = mysql_query(&mysql_, sql);
            if(ret != 0)
            {
                printf("查询失败:%s\n", mysql_error(&mysql_));
                return false;

            }

            MYSQL_RES* res = mysql_store_result(&mysql_);
            if(!res)
            {
                return false;

            }

            MYSQL_ROW row = mysql_fetch_row(res);
            Json::Value tmp;
            (*result)["stu_id"] = atoi(row[0]);
            (*result)["stu_name"] = row[1];
            (*result)["stu_choice_score"] = JUDGEVALUE(row[6]);
            (*result)["stu_program_score"] = JUDGEVALUE(row[7]);
            (*result)["stu_total_score"] = JUDGEVALUE(row[8]);
            (*result)["stu_speculative_score"] = JUDGEVALUE(row[10]);
            (*result)["stu_code_score"] = JUDGEVALUE(row[11]);
            (*result)["stu_think_score"] = JUDGEVALUE(row[12]);
            (*result)["stu_expression_score"] = JUDGEVALUE(row[13]);
            (*result)["stu_interview_score"] = JUDGEVALUE(row[14]);
            (*result)["stu_interview_techer"] = ((row[15] != NULL) ? row[15] : "");
            (*result)["stu_techer_suggest"] = ((row[16] != NULL) ? row[16] : "");
            (*result)["stu_interview_time"] = ((row[9] != NULL) ? row[9] : "");
            mysql_free_result(res);    
            return true;

        }

        bool QueryUserExist(Json::Value& request, Json::Value* result)
        {
            mysql_query(&mysql_, "set names utf8");
            std::string email = request["email"].asString();
            std::string password = request["password"].asString();

            char sql[1024] = {0};
            snprintf(sql, sizeof(sql), JudgeUserInfo, email.c_str());
            int ret = mysql_query(&mysql_, sql);
            if(ret != 0)
            {
                printf("查询失败:%s\n", mysql_error(&mysql_));
                return false;

            }

            MYSQL_RES* res = mysql_store_result(&mysql_);
            if(!res)
            {
                return false;

            }

            int row_num = mysql_num_rows(res);    
            if (row_num < 1) 
            {    
                (*result)["is_exist"] = false;
                mysql_free_result(res);    
                return true;    
            }    
            else
            {
                (*result)["is_exist"] = true;

            }

            Json::Value tmp;
            MYSQL_ROW row = mysql_fetch_row(res);
            tmp["password"] = ((row[2] != NULL) ? row[2] : "");
            if(strcmp(password.c_str(), tmp["password"].asString().c_str()) == 0)
            {
                (*result)["is_password"] = true;

            }
            else
            {
                (*result)["is_password"] = false;

            }

            (*result)["user_id"] = atoi(row[0]);

            mysql_free_result(res);    
            return true;

        }

        bool InsetData(const std::string& sql)
        {
            mysql_query(&mysql_, "set names utf8");

            if(mysql_query(&mysql_, sql.c_str()))
            {
                printf("查询失败:%s\n", mysql_error(&mysql_));
                LOG(ERROR, "exec sql: ") << sql << "failed!" << std::endl;
                return false;

            }
            return true;

        }

        void FreeConnect()
        {
            mysql_close(&mysql_);

        }
    private:
        MYSQL mysql_;

        std::string host_;
        std::string user_;
        std::string passwd_;
        std::string db_;
        uint16_t port_;

};


