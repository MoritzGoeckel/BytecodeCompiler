#include <iostream>
#include <vector>

#include "OptCodeEngine.cpp"
#include "Memory.cpp"
#include "ByteCode.cpp"

#include "ErrorHandling.cpp"

#include "Compiler.cpp"
#include "VirtualMachine.cpp"

#include "Types.cpp"

#include "gtest/gtest.h"

TEST(Writer, WriteFileTest) {
  std::vector<int8> outData = {15, 30};
  ByteCode outCode(outData);
  outCode.write("testfile_vec.bc");

  ByteCode inCode;
  inCode.read("testfile_vec.bc");

  EXPECT_EQ (15, inCode.getAt(0));
  EXPECT_EQ (30, inCode.getAt(1));
  EXPECT_EQ (2, inCode.size());
}

TEST(Compiler, ReadAndCompile) {
  compileFile("testfile.ma", "testfile.bc");
}

TEST(MVM, ReadAndExecute) {
  ByteCode code;
  code.read("testfile.bc");
  VirtualMachine::execute(code);
  std::cout << "Done executing" << std::endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}