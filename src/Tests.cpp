#include <iostream>
#include <vector>

#include "VM/OptCodeEngine.cpp"
#include "VM/Memory.cpp"
#include "VM/ByteCode.cpp"

#include "VM/Types.cpp"
#include "ErrorHandling.cpp"

#include "VM/Assembler.cpp"
#include "VM/Executer.cpp"

#include "Interpreter/Lexer.cpp"
#include "Interpreter/Parser.cpp"

#include "Interpreter/Compiler.cpp"

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
  assembleFile("testfile.ma", "testfile.bc");
  std::cout << "Done assembling" << std::endl;
}

TEST(MVM, ReadAndExecute) {
  ByteCode code;
  code.read("testfile.bc");
  std::cout << "Done reading" << std::endl;
  //execute(code); //TODO: Enable again
  std::cout << "Done executing" << std::endl;
}

TEST(Lexxer, LexerTest) {
  Lexer l("samplecode.m");
  
  while(!l.eof())
    l.getNextToken().print();

  std::cout << std::endl;
}

TEST(Paarser, ParserTest) {
  Lexer l("samplecode.m");
  
  std::vector<Token> tokens;
  while(!l.eof())
    tokens.push_back(l.getNextToken());

  Parser p(tokens);
  ASTNode n = p.parse();
  //n.print(); //TODO: Enable again?

  std::cout << "Done executing" << std::endl;
}

TEST(Compiler, CompilerTest) {
  Lexer l("samplecode.m");
  
  std::vector<Token> tokens;
  while(!l.eof())
    tokens.push_back(l.getNextToken());

  Parser p(tokens);
  ASTNode n = p.parse();
  
  Compiler c;

  std::cout << c.compile(n) << std::endl;

  std::cout << "Done compiling" << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
