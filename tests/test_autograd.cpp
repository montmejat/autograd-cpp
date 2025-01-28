#include <iostream>
#include "../include/scalar.h"
#include <gtest/gtest.h>

TEST(AutoGradTest, Constructor)
{
    autograd::Scalar s = autograd::Scalar(2);
    EXPECT_EQ(s.data(), 2);
}

TEST(AutoGradTest, Add)
{
    autograd::Scalar s1 = autograd::Scalar(2);
    autograd::Scalar s2 = autograd::Scalar(3);
    autograd::Scalar s3 = s1 + s2;
    EXPECT_EQ(s3.data(), 5);

    autograd::Scalar s4 = s1 + 3;
    EXPECT_EQ(s4.data(), 5);

    autograd::Scalar s5 = 3 + s1;
    EXPECT_EQ(s5.data(), 5);
}

TEST(AutoGradTest, Childs)
{
    autograd::Scalar s1 = autograd::Scalar(2);
    autograd::Scalar s2 = autograd::Scalar(3);
    autograd::Scalar s3 = s1 + s2;

    EXPECT_EQ(s3.left().has_value(), true);
    EXPECT_EQ(s3.right().has_value(), true);

    EXPECT_EQ(s3.left().value()->data(), 2);
    EXPECT_EQ(s3.right().value()->data(), 3);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
