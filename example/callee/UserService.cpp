#include <cstdint>
#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"

using namespace fixbug;

class UserService: public UserServiceRpc {
public:
    //本地业务
    bool login(std::string name, std::string pwd) {
        std::cout << "Doing local service: Login\n";
        std::cout << "name: " << name << " pwd: " << pwd << "\n";
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "Doing Local Service: Register\n";
        std::cout << "id: " << id << " name: " << name << "pwd: " << pwd << "\n";
        return true;
    }

    void login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginReq* request,
                       ::fixbug::LoginResp* response,
                       ::google::protobuf::Closure* done) override
    {
        std::string name = request->name();
        std::string pwd  = request->pwd();
        bool res = login(name, pwd);
        ResultCode *code = response->mutable_result();
        code->set_errcode(0);// fake data
        code->set_errmsg("ArgT0 &&arg0");
        response->set_succes(res);
        //执行回调操作, 序列化并发送rpc
        done->Run();
    }

    void reg(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterReq* request,
                       ::fixbug::RegisterResp* response,
                       ::google::protobuf::Closure* done) override 
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool ret = Register(id, name, pwd);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);
        done->Run();
    }
};

/**
 * 想要实现的框架的样子
*/
int main(int argc, char *argv[]) {

    MprpcApplication::init(argc, argv);
    
    RpcProvider provider;
    provider.notifyService(new UserService());
    provider.Run(); 
   
    return 0;
}