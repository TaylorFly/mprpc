#include "friend.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"
#include <cstdint>
#include <google/protobuf/service.h>

class FriendService: public fixbug::FriendServiceRpc {
public: 
    std::vector<std::string> getFriendList(uint32_t id) {
        std::cout << "do get friend list service\n";
        return {"Lora", "FGG"};
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListsReq* request,
                       ::fixbug::GetFriendListsResp* response,
                       ::google::protobuf::Closure* done) 
    {
        uint32_t id = request->userid();
        auto ret = getFriendList(id);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (auto it: ret) {
            std::string *p = response->add_friends();
            *p = it;
        }
        done->Run();
    }
};

int main(int argc, char *argv[]) {
    MprpcApplication::init(argc, argv);
    RpcProvider provider;
    provider.notifyService(new FriendService());
    provider.Run();
    return 0;
}