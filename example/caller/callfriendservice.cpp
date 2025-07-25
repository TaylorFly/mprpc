#include "logger.hpp"
#include "mprpcapplication.hpp"
#include "mprpcchannel.hpp"
#include "../friend.pb.h"
#include "mprpccontroller.hpp"

int main(int argc, char *argv[]) {
    LOG_INFO("first log message");

    MprpcApplication::init(argc, argv);
    
    MprpcController controller;

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendListsReq req;
    req.set_userid(1000);
    fixbug::GetFriendListsResp resp;
    stub.GetFriendList(&controller, &req, &resp, nullptr);

    if (controller.Failed()) {
        std::cout << controller.ErrorText() << "\n";
        return 0;
    }

    const int n = resp.friends_size();
    for (int i = 0; i < n; i++) {
        std::cout << resp.friends(i) << "\n";
    }
    return 0;
}