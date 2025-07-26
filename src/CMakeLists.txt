set(SRC_LIST
    mprpcapplication.cpp
    mprpcchannel.cpp
    mprpcconfig.cpp
    rpcprovider.cpp
    rpcheader.pb.cc
    mprpccontroller.cpp
    logger.cpp
    zookeeperutil.cpp
)
add_library(mprpc ${SRC_LIST})
target_compile_definitions(mprpc PUBLIC THREADED)
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
