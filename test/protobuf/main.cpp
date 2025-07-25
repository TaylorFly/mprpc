#include "test.pb.h"
#include <iostream>

using namespace fixbug;

int main() {
    GetFriendListResp resp;
    RCode *code = resp.mutable_result();
    code->set_errcode(1);
    code->set_errmsg("hello");
    User *user1 = resp.add_users();
    user1->set_name("zhangsan");
    user1->set_gender(User::MAN);
    std::cout << resp.users_size() << "\n";
    User *user2 = resp.add_users();
    user2->set_name("lora");
    user2->set_gender(User::WOMAN);
    std::cout << resp.users_size() << "\n";
    const User &user3 = resp.users(1);
    std::cout << user3.age() << " " << user3.gender() << "\n";
    return 0;
}