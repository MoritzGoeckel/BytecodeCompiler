#include <iostream>
#include <vector>

#include "Writer.cpp"
#include "gtest/gtest.h"

TEST(Writer, WriteFileTest) {
    std::vector<unsigned char> outData = {15, 30};
    write(outData);

    std::vector<unsigned char> data;
    read(data);
    EXPECT_EQ (15, data.front());
    EXPECT_EQ (30, data.at(1));
    EXPECT_EQ (2, data.size());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}