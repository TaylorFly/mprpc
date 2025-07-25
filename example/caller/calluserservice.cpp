#include "mprpcapplication.hpp"
#include "mprpcchannel.hpp"
#include "user.pb.h"

int main(int argc, char *argv[]) {
    MprpcApplication::init(argc, argv);

    // 调用远程发布的rpc方法login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginReq request;
    request.set_name("zhangsan");
    request.set_pwd("123456");

    fixbug::LoginResp resp;
    stub.login(nullptr, &request, &resp, nullptr);

    if (resp.result().errcode() == 0) {
        std::cout << "rpc login response: " << resp.succes() << "\n";
    } else {
        std::cout << "rcp login response error: " << resp.result().errmsg() << "\n";
    }

    fixbug::RegisterReq rreq;
    fixbug::RegisterResp rresp;
    rreq.set_id(1000);
    rreq.set_name("lora");
    rreq.set_pwd("Ligaofei2003..");

    stub.reg(nullptr, &rreq, &rresp, nullptr);
    if (rresp.result().errcode() == 0) {
        std::cout << "rpc register response: " << rresp.success() << "\n";
    } else {
        std::cout << "rpc register error: " << resp.result().errmsg() << "\n";
    }
    return 0;
} 