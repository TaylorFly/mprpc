set(SRC_LIST calluserservice.cpp ../user.pb.cc)
set(CALL_FRIEND_LIST callfriendservice.cpp ../friend.pb.cc)

add_executable(consumer ${SRC_LIST})
add_executable(call_friend ${CALL_FRIEND_LIST})

target_link_libraries(consumer mprpc protobuf)
target_link_libraries(call_friend mprpc protobuf)