#include <iostream>
#include <vector>

#include "../Source/VM/OptCodeEngine.cpp"
#include "../Source/VM/Memory.cpp"
#include "../Source/VM/ByteCode.cpp"

#include "../Source/VM/Types.cpp"
#include "../Source/ErrorHandling.cpp"

#include "../Source/VM/Assembler.cpp"
#include "../Source/VM/Executer.cpp"

#include "../Source/Interpreter/Lexer.cpp"
#include "../Source/Interpreter/Parser.cpp"

#include "../Source/Interpreter/Compiler.cpp"

#include "../Source/Interpreter/Transformers/FunctionFlatter.cpp"
#include "../Source/Interpreter/Transformers/ImplicitReturn.cpp"

#include "gtest/gtest.h"

/*

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
  ByteCode code = assembleFile("testfile.ma");
  code.write("testfile.bc");
  std::cout << "Done assembling" << std::endl;
}

TEST(MVM, ReadAndExecute) {
  ByteCode code;
  code.read("testfile.bc");
  std::cout << "Done reading" << std::endl;
  execute(code); //TODO: Enable again
  std::cout << "Done executing" << std::endl;
}

TEST(Lexxer, LexerTest) {
  Lexer l("samplecode.m");
  
  while(!l.eof())
    l.getNextToken().print();

  std::cout << std::endl;
}

 TEST(Parser, ParserTest) {
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

 TEST(Integration, IntegrationTest) {
  Lexer l("samplecode.m");
  
  std::vector<Token> tokens;
  while(!l.eof())
    tokens.push_back(l.getNextToken());

  for(Token& t : tokens)
    t.print();
  std::cout << std::endl;
  std::cout << ">> Done tokenizing" << std::endl;

  Parser p(tokens);
  ASTNode n = p.parse();
  n.print();
  std::cout << ">> Done parsing" << std::endl;

  Compiler c;
  std::string compiled = c.compile(n);
  std::cout << compiled << std::endl;
  std::cout << ">> Done compiling" << std::endl;
  
  ByteCode code = assembleString(compiled);
  std::cout << ">> Done assembling" << std::endl;

  execute(code);
  std::cout << ">> Done executing" << std::endl;
}
*/

TEST(Benchmarker, DeepCodeParsing) {
  Lexer l("MFiles/deepCode.m");
  
  std::vector<Token> tokens;
  while(!l.eof())
    tokens.push_back(l.getNextToken());
  std::cout << ">> Done tokenizing" << std::endl;

  Parser p(tokens);
  ASTNode n = p.parse();
  std::cout << ">> Done parsing" << std::endl;
}

TEST(Transformer, FlattenFunctionsTest) {
  Lexer l("MFiles/samplecode.m");
  
  std::vector<Token> tokens;
  while(!l.eof())
    tokens.push_back(l.getNextToken());

  for(Token& t : tokens)
    t.print();
  std::cout << std::endl;
  std::cout << ">> Done tokenizing" << std::endl;

  Parser p(tokens);
  ASTNode n = p.parse();
  n.print();
  std::cout << ">> Done parsing" << std::endl;

  std::vector<ASTNode> fns;
  createMain(n);
  flattenFunctions(n, fns);

  ASTNode main = fns.back();
  fns.pop_back();
  std::cout << ">> Done flattening" << std::endl;

  addVoidReturn(fns);
  std::cout << ">> Done adding void return" << std::endl;

  std::cout << ">> Main" << std::endl;
  main.print();

  std::cout << ">> Functions" << std::endl;
  for(auto f : fns){
    f.print();
  }

  Compiler c;
  std::string compiled = c.compile(fns, main);

  //std::cout << compiled << std::endl;
  std::cout << ">> Done compiling" << std::endl;
  
  ByteCode code = assembleString(compiled);
  //code.write("MAFiles/samplecode.bc");
  std::cout << ">> Done assembling" << std::endl;

  execute(code);
  std::cout << ">> Done executing" << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
