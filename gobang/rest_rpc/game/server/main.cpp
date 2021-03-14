#include "../../include/rest_rpc.hpp"
#include <string>
#include <fstream>
#include "hall.hpp"

using namespace std;
using namespace rest_rpc;
using namespace rpc_service;

Hall global_hall;

uint32_t Rpc_Register(rpc_conn conn, const string& name, const string& passwd) 
{
	cout << name << ":" << passwd << endl;

    return global_hall.PlayerRegister(name, passwd);
}

uint32_t Rpc_Login(rpc_conn conn, const uint32_t id, const string& passwd) 
{
    cout << "recv Login request... id is " << id << " passwd is " << passwd << endl;
    return global_hall.PlayerLogin(id, passwd);
}

bool Rpc_Match(rpc_conn conn, const uint32_t id)
{
    return global_hall.PushPlayer2MatchPool(id);
}


int Rpc_CheckReady(rpc_conn conn, const uint32_t id)
{
    return global_hall.GetPlayerstatus(id);
}

void Rpc_PopPlayerformMatchPool(rpc_conn conn, const uint32_t id)
{
    global_hall.PopPlayer(id);
}

uint32_t Rpc_GetRoomId(rpc_conn conn, const uint32_t id) 
{
    return global_hall.GetPlayerRoomId(id);
}

char Rpc_GetPiece(rpc_conn conn, const uint32_t id, const uint32_t room_id) 
{
    return global_hall.GetPlayerPiece(room_id, id);
}

string Rpc_GetRoomBoard(rpc_conn conn, const uint32_t room_id) 
{
    return global_hall.GetRoomBoard(room_id);
}

bool Rpc_IsMyTurn(rpc_conn conn, const uint32_t room_id, const uint32_t id) 
{
    return global_hall.IsMyturn(room_id, id);
}

char Rpc_Step(rpc_conn conn, const uint32_t room_id, const uint32_t id, const int x, const int y) 
{
    return global_hall.Step(room_id, id, x, y);
}

char Rpc_GetResult(rpc_conn conn, const uint32_t room_id) 
{
    return global_hall.GetResult(room_id);
}

int main() 
{
	rpc_server server(9000, 2);
	server.register_handler("Rpc_Register", Rpc_Register);
	server.register_handler("Rpc_Login", Rpc_Login);
	server.register_handler("Rpc_Match", Rpc_Match);
	server.register_handler("Rpc_CheckReady", Rpc_CheckReady);
	server.register_handler("Rpc_PopPlayerformMatchPool", Rpc_PopPlayerformMatchPool);
	server.register_handler("Rpc_GetRoomId", Rpc_GetRoomId);
	server.register_handler("Rpc_GetPiece", Rpc_GetPiece);
	server.register_handler("Rpc_GetRoomBoard", Rpc_GetRoomBoard);
	server.register_handler("Rpc_IsMyTurn", Rpc_IsMyTurn);
	server.register_handler("Rpc_Step", Rpc_Step);
	server.register_handler("Rpc_GetResult", Rpc_GetResult);
    int ret = global_hall.HallInit();
    if(ret < 0)
    {
        LOG(ERROR, "游戏大厅启动失败！请检查代码！") << endl;
    }

    LOG(INFO, "服务启动成功...") << "port: 9000 " << endl;
    server.run();

    string str;
    cin >> str;
}























//#include "qps.h"
//
//struct dummy{
//	int add(rpc_conn conn, int a, int b) {
//		return a + b;
//	}
//};
//
//std::string translate(rpc_conn conn, const std::string& orignal) {
//	std::string temp = orignal;
//	for (auto& c : temp) { 
//		c = std::toupper(c); 
//	}
//	return temp;
//}
//
//
struct person {
	int id;
	std::string name;
	int age;

	MSGPACK_DEFINE(id, name, age);
};
//
std::string get_person_name(rpc_conn conn, const person& p) {
	return p.name;
}
//
//person get_person(rpc_conn conn) {
//	return { 1, "tom", 20 };
//}
//
//void upload(rpc_conn conn, const std::string& filename, const std::string& content) {
//	std::cout << content.size() << std::endl;
//	std::ofstream file(filename, std::ios::binary);
//	file.write(content.data(), content.size());
//}
//
//std::string download(rpc_conn conn, const std::string& filename) {
//	std::ifstream file(filename, std::ios::binary);
//	if (!file) {
//		return "";
//	}
//
//	file.seekg(0, std::ios::end);
//	size_t file_len = file.tellg();
//	file.seekg(0, std::ios::beg);
//	std::string content;
//	content.resize(file_len);
//	file.read(&content[0], file_len);
//	std::cout << file_len << std::endl;
//
//	return content;
//}
//
//qps g_qps;
//
//std::string get_name(rpc_conn conn, const person& p) {
//	g_qps.increase();
//	return p.name;
//}
//
////if you want to response later, you can use async model, you can control when to response
//void async_echo(rpc_conn conn, const std::string& src) {
//	auto req_id = conn.lock()->request_id();//note: you need keep the request id at that time, and pass it into the async thread
//	
//	std::thread thd([conn, req_id, src] {
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//		auto conn_sp = conn.lock();
//		if (conn_sp) {
//			conn_sp->pack_and_response(req_id, std::move(src));
//		}
//	});
//	thd.detach();
//}
//
//std::string echo(rpc_conn conn, const std::string& src) {
//  g_qps.increase();
//	return src;
//}
//
//int get_int(rpc_conn conn, int val) {
//	return val;
//}
//
//void test_ssl() {
//    rpc_server server(9000, std::thread::hardware_concurrency(), { "server.crt", "server.key" });
//    server.register_handler("hello", hello);
//    server.register_handler("echo", echo);
//    server.run();
//}
//
//void benchmark_test(){
//  rpc_server server(9000, std::thread::hardware_concurrency());
//  server.register_handler("echo", echo);
//  server.run();
//}
//
//int main() {
//////  benchmark_test();
//	rpc_server server(9000, 2);
////
////	dummy d;
////	server.register_handler("add", &dummy::add, &d);
////	server.register_handler("translate", translate);
//	server.register_handler("hello", hello);
//	server.register_handler("get_person_name", get_person_name);
////	server.register_handler("get_person", get_person);
////	server.register_handler("upload", upload);
////	server.register_handler("download", download);
////	server.register_handler("get_name", get_name);
////	server.register_handler<Async>("async_echo", async_echo);
////	server.register_handler("echo", echo);
////	server.register_handler("get_int", get_int);
////
////	server.register_handler("publish_by_token", [&server](rpc_conn conn, std::string key, std::string token, std::string val) {
////		server.publish_by_token(std::move(key), std::move(token), std::move(val));
////	});
////
////	server.register_handler("publish", [&server](rpc_conn conn, std::string key, std::string token, std::string val) {
////		server.publish(std::move(key), std::move(val));
////	});
////
////	std::thread thd([&server] {
////		person p{ 1, "tom", 20 };
////		while (true) {
////			server.publish("key", "hello subscriber");
////			auto list = server.get_token_list();
////			for (auto& token : list) {				
////				server.publish_by_token("key", token, p);
////				server.publish_by_token("key1", token, "hello subscriber1");
////			}
////			std::this_thread::sleep_for(std::chrono::milliseconds(50));
////		}
////	});
////
//	server.run();
////
//	std::string str;
//	std::cin >> str;
//}
