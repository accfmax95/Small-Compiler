#ifndef DEMO_H
#define DEMO_H

#include "inputbuf.h"
#include "lexer.h"
#include "compiler.h"

class Parser {
  public:

    Token peek();

    int parsePrimary();
    void parseProgram();
    void parseIDList();
    void parseNumList();

    InstructionNode* parseBody();
    InstructionNode* parseStmtList();
    InstructionNode* parseStmt();
    InstructionNode* parseAssignStmt();
    InstructionNode* parseWhileStmt();
    InstructionNode* parseIfStmt();
    InstructionNode* parseCondition();
    InstructionNode* parseCaseList(int num, InstructionNode* tempNode);
    InstructionNode* parseSwitchStmt(InstructionNode* tempNode);
    InstructionNode* parseCase(int num);
    InstructionNode* parseForStmt();
};

#endif 