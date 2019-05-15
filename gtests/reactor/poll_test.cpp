#include <gtest/gtest.h>
#include <poll.h>
#include <vector>

TEST(test_pollfd_using_vector, vector_poll){
    // std::vector<struct pollfd*> poll_vector{};

    // struct pollfd* pfd1 = new pollfd;
    // pfd1->fd = 1;
    // pfd1->events |= POLLIN;

    // struct pollfd* pfd2 = new pollfd;
    // pfd2->fd = 2;
    // pfd2->events |= POLLOUT;

    // poll_vector.push_back(pfd1);
    // poll_vector.push_back(pfd2);

    // ASSERT_EQ(poll_vector.size(), 2);
    // auto p1 = poll_vector[0];
    // auto p2 = poll_vector[1];
    // ASSERT_EQ(p1, pfd1);
    // ASSERT_EQ(pfd2, p2);
    // ASSERT_EQ(pfd2 - pfd1, sizeof (struct pollfd));
}