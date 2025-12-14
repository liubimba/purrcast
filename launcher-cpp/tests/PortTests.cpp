//
// Created by bimba on 11/15/25.
//


#include "Tests.hpp"

TEST(Port, isBound)
{
    ASSERT_FALSE(!Port::isBound(1704));
    ASSERT_FALSE(!Port::isBound(1704));
#if defined(__linux)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(sockfd, -1);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1704);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        FAIL() << "bind failed";
    }
    ASSERT_TRUE(Port::isBound(1704));
    close(sockfd);
#endif
}
