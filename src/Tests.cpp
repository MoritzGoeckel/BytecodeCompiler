#include <iostream>
#include <vector>

#include "OperationCodes.cpp"
#include "Memory.cpp"
#include "ByteCode.cpp"
#include "gtest/gtest.h"

TEST(Writer, WriteFileTest) {
    std::vector<unsigned char> outData = {15, 30};
    ByteCode outCode(outData);
    outCode.write("testfile.bytes");

    ByteCode inCode;
    inCode.read("testfile.bytes");

    EXPECT_EQ (15, inCode.getAt(0));
    EXPECT_EQ (30, inCode.getAt(1));
    EXPECT_EQ (2, inCode.size());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}