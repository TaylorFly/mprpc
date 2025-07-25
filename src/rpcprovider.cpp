#include "rpcprovider.hpp"
#include "mprpcapplication.hpp"
#include "rpcheader.pb.h"
#include <cstdint>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <memory>
#include <muduo/net/Callbacks.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <string>
#include <functional>


void RpcProvider::notifyService(google::protobuf::Service *service) {
    ServiceInfo info;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = pserviceDesc->name();
    // 获取方法数量
    int methodCnt = pserviceDesc->method_count();

    std::cout << "service name: " << service_name << "\n";

    for (int i = 0; i < methodCnt; i++) {
        // 获取服务对象对象的服务方法的描述
        const google::protobuf::MethodDescriptor* methodDesc = pserviceDesc->method(i);
        const std::string methodName = methodDesc->name();
        info.m_methodMap.insert({methodName, methodDesc});
        std::cout << "service method: " << methodName << "\n";
    }
    info.m_service = service;
    m_serviceMap.insert({service_name, info});
}

void RpcProvider::Run() {
    std::string ip = MprpcApplication::getInstance().getConfig().load("rpcserverip");
    uint16_t port = std::stoi(MprpcApplication::getInstance().getConfig().load("rpcserverport"));
    muduo::net::InetAddress addr(ip, port);

    std::unique_ptr<muduo::net::TcpServer> server = std::make_unique<muduo::net::TcpServer>(&m_loop, addr, "mprpcserver");
    server->setThreadNum(4);
    server->setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server->setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    std::cout << "RPC start service at ip: " << ip << ", port: " << port << "\n";

    server->start();
    m_loop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        conn->shutdown();
    } else {

    }
}

/**
发送的数据需要包含: service name, method name, args

header_size + headerstr + args_str 
eg: 16UserServiceLoginlora
*/

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    // 网络上接收的远程rpc调用请求的字符数据
    std::string recv_buf = buffer->retrieveAllAsString();

    //从字符流中读取前四个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 根据header size读取数据头中的原始字符流 serviceName + methodName + args size
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name; 
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        //数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.arg_size();
    } else {
        //数据头反序列化失败
        std::cout << "Rpc head str: " << rpc_header_str << " parse error!\n";
        return ;
    }

    //获取rpc参数
    std::string rpc_args_str = recv_buf.substr(header_size + 4, args_size);

    //打印调试信息
    std::cout << "===============================================\n";
    std::cout << "header size: " << header_size << "\n";
    std::cout << "service name: " << service_name << "\n";
    std::cout << "method name: " << method_name << "\n";
    std::cout << "args size: " << args_size << "\n";
    std::cout << "===============================================\n";

    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end()) {
        std::cout << service_name << " is not exist\n";
        return ;
    }
    google::protobuf::Service *service = it->second.m_service;
    // method iterator
    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) {
        std::cout << method_name << " is not exist\n";
        return ;
    }
    const google::protobuf::MethodDescriptor *method = mit->second;
    
    //生成rpc方法调用的request和response
    google::protobuf::Message *req = service->GetRequestPrototype(method).New();
    if (!req->ParseFromString(rpc_args_str)) {
        std::cout << "request parse error! content: " << rpc_args_str << "\n";
        return ;
    }
    google::protobuf::Message *resp = service->GetResponsePrototype(method).New();

    //给下面的method方法的调用,绑定一个Closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback
                <RpcProvider,
                const muduo::net::TcpConnectionPtr&, 
                google::protobuf::Message*>
            (this, &RpcProvider::SendRpcResponse, conn, resp);
    
    //在框架上根据远端rpc请求,调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, req, resp, done);  
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* resp) {
    std::string str;
    if (resp->SerializeToString(&str)) {
        // 序列化成功之后,通过网络将rpc方法执行的结果发送给rpc的调用方
        conn->send(str);
        conn->shutdown(); // 模拟http的短连接服务
    } else {
        std::cout << "response serialize error!\n";
    }
}