set(USER_SERVICE_LIST UserService.cpp ../user.pb.cc)
set(FRIEND_SERVICE_LIST FriendService.cpp ../friend.pb.cc)

add_executable(provider ${USER_SERVICE_LIST})
add_executable(friend_provider ${FRIEND_SERVICE_LIST})

target_link_libraries(provider mprpc protobuf)
target_link_libraries(friend_provider mprpc protobuf)
