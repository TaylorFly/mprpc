#include "mprpcchannel.hpp"
#include "rpcheader.pb.h"
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "mprpcapplication.hpp"

void MprpcChannel::CallMethod
                         (const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done) 
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 arg_size
    std::string args_str;
    
    if (!request->SerializeToString(&args_str)) {
        controller->SetFailed("serialize request error!");
        return ;
    }

    int args_size = args_str.size();
    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_arg_size(args_size);

    std::string rpcHeaderStr;
    uint32_t header_size;
    if (!rpcHeader.SerializeToString(&rpcHeaderStr)) {
        std::cout << "serialize rpc header error!\n";
        controller->SetFailed("serialize rpc header error!");
        return ;
    } else {
        header_size = rpcHeaderStr.size();
    }
    std::string sendRpcStr;
    sendRpcStr.insert(0, std::string((char*)&header_size, 4));
    sendRpcStr += rpcHeaderStr;
    sendRpcStr += args_str;

    std::cout << "===============================================\n";
    std::cout << "header size: " << header_size << "\n";
    std::cout << "service name: " << service_name << "\n";
    std::cout << "method name: " << method_name << "\n";
    std::cout << "args size: " << args_size << "\n";
    std::cout << "===============================================\n";

    int clientfd = socket(PF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        controller->SetFailed("create socket error!");
        exit(EXIT_FAILURE);
    }

    std::string ip = MprpcApplication::getInstance().getConfig().load("rpcserverip");
    uint16_t port = std::stoi(MprpcApplication::getInstance().getConfig().load("rpcserverport"));
    
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    if (connect(clientfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        controller->SetFailed("connect server error!");
        close(clientfd);
        return ;
    }

    // 发送rpc
    if (send(clientfd, sendRpcStr.c_str(), sendRpcStr.size(), 0) == -1) {
        controller->SetFailed("send error");
        close(clientfd);
        return ;
    }

    char buf[1024];
    int recv_len = 0;
    if ((recv_len = recv(clientfd, buf, 1024, 0)) == -1) {
        controller->SetFailed("recv error!");
        close(clientfd);
        return ;
    }
  
    if (!response->ParseFromArray(buf, recv_len)) {
        controller->SetFailed("parse error");
        close(clientfd);
        return ;
    }
    close(clientfd); 
}
