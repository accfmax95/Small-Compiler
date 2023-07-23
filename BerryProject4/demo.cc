#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <iostream>

#include "demo.h"
#include "compiler.h"
#include "lexer.h"

using namespace std;

LexicalAnalyzer lexer;
struct InstructionNode* insNode;
map <string, int> myMap;

InstructionNode* parse_generate_intermediate_representation() {

    Parser parser;
    parser.parseProgram();
    return insNode;
}

int Parser::parsePrimary() {
    
    int val;
    Token token;
    token = lexer.GetToken();

    if (token.token_type == NUM){

        val = next_available;
        mem[next_available++] = stoi(token.lexeme);
    } else if (token.token_type == ID) {

        val = myMap[token.lexeme];
    } 

    return val;
}

void Parser::parseProgram() {
    
    Token token;
    token = peek();
    if (token.token_type == ID) {
       
        parseIDList();
        lexer.GetToken();
    } 
    
    token = peek();
    if (token.token_type == LBRACE) {
        
        insNode = parseBody();
    } 
    
    token = peek();
    if (token.token_type == NUM) {
        
        parseNumList();
    } 
}

void Parser::parseIDList(){
    
    Token token;
    token = lexer.GetToken();

    myMap[token.lexeme] = next_available;
    mem[next_available++] = 0;

    token = lexer.GetToken();
    if (token.token_type != COMMA) {
        
        lexer.UngetToken(token);
    } else {

        parseIDList();
    }
}

void Parser::parseNumList() {

    Token token;
    token = lexer.GetToken();
 
    inputs.push_back(stoi(token.lexeme));
    
    token = peek();
    if (token.token_type != NUM) {
        
        
    } else {

        parseNumList();
    }
}

InstructionNode* Parser::parseBody() {

    InstructionNode* ins1 = nullptr;
    
    lexer.GetToken();
    ins1 = parseStmtList();
    lexer.GetToken();
    
    return ins1;
}

InstructionNode* Parser::parseStmtList() {
    
    Token token = peek();
    InstructionNode* ins1 = nullptr;
    InstructionNode* ins2 = nullptr;

    ins1 = parseStmt();
    InstructionNode* getLast = ins1;
    if (token.token_type != ID && token.token_type != WHILE && token.token_type != IF && token.token_type != SWITCH
       && token.token_type != FOR && token.token_type != OUTPUT && token.token_type != INPUT) {
        
    
    } else {

        ins2 = parseStmtList();
        while (getLast -> next != nullptr) {
            
            getLast = getLast -> next;
        }

        getLast -> next = ins2;
        return ins1;
    }

    return ins1;
}

InstructionNode* Parser::parseStmt() {
    
    Token token;
    InstructionNode* getLast;
    InstructionNode* ins3 = nullptr;
    InstructionNode* ins4 = new InstructionNode;
    
    
    ins4 -> type = NOOP;
    ins4 -> next = nullptr;

    token = peek();
    if (token.token_type ==  ID) {

        return parseAssignStmt();
    } else if (token.token_type ==  WHILE) {

        return parseWhileStmt() ;
    } else if (token.token_type ==  IF) {

        return parseIfStmt();
    } else if (token.token_type ==  FOR) {
        
        return parseForStmt();
    } else if (token.token_type ==  OUTPUT) {

        InstructionNode* outputInst = new InstructionNode;
        
        lexer.GetToken();
        outputInst -> type = OUT;
        
        token = lexer.GetToken();
        outputInst -> output_inst.var_index = myMap[token.lexeme];
        lexer.GetToken();

        return outputInst;
    } else if (token.token_type ==  INPUT) {

        InstructionNode* inputInst = new InstructionNode;

        lexer.GetToken();
        inputInst -> type = IN;

        token = lexer.GetToken();
        inputInst->input_inst.var_index = myMap[token.lexeme];
        lexer.GetToken();

        return inputInst;
    } else if (token.token_type ==  SWITCH) {

        ins3 = parseSwitchStmt(ins4);
        getLast = ins3;

        while (getLast -> next != nullptr){
            
            getLast = getLast->next;
        }

        getLast -> next = ins4;
        return ins3;
    }

    return ins3;
}

InstructionNode* Parser::parseAssignStmt() {

    Token token = lexer.GetToken();
    InstructionNode* instNode = new InstructionNode;
    InstructionNode* getInfo = new InstructionNode;

    instNode -> type = ASSIGN;
    instNode -> assign_inst.left_hand_side_index = myMap[token.lexeme];

    lexer.GetToken();
    Token token1 = lexer.GetToken();
    Token token2 = peek();

    lexer.UngetToken(token1);
    if ((token1.token_type == ID || token1.token_type == NUM) && (token2.token_type == PLUS || token2.token_type == MINUS 
                    || token2.token_type == MULT || token2.token_type == DIV)) { 
        
        getInfo -> assign_inst.operand1_index = parsePrimary();

        token = lexer.GetToken();

        if (token.token_type == PLUS) {

            getInfo -> assign_inst.op = OPERATOR_PLUS;
        } else if (token.token_type == MINUS) {

            getInfo -> assign_inst.op = OPERATOR_MINUS;
        } else if (token.token_type == MULT) {

            getInfo -> assign_inst.op = OPERATOR_MULT;
        } else if (token.token_type == DIV) {

            getInfo -> assign_inst.op = OPERATOR_DIV;
        }

        getInfo -> assign_inst.operand2_index = parsePrimary();

        instNode -> assign_inst.op = getInfo -> assign_inst.op;
        instNode -> assign_inst.operand1_index = getInfo -> assign_inst.operand1_index;
        instNode -> assign_inst.operand2_index = getInfo -> assign_inst.operand2_index;
    } else if ((token1.token_type == ID || token1.token_type == NUM) && (token2.token_type == SEMICOLON)){

        instNode -> assign_inst.op = OPERATOR_NONE;
        instNode -> assign_inst.operand1_index = parsePrimary();
    }

    lexer.GetToken();

    return instNode;
}

InstructionNode* Parser::parseWhileStmt() {

    Token token;
    InstructionNode* ins1 = new InstructionNode;
    lexer.GetToken();

    ins1 -> type = CJMP;
    InstructionNode* ins2 = parseCondition();
    ins1 -> cjmp_inst.operand1_index = ins2 -> cjmp_inst.operand1_index;
    ins1 -> cjmp_inst.condition_op = ins2 -> cjmp_inst.condition_op;
    ins1 -> cjmp_inst.operand2_index = ins2 -> cjmp_inst.operand2_index;

    token = peek();
    if (token.token_type != LBRACE){
        

    } else {
        
        ins1 -> next = parseBody();
    }

    InstructionNode* ins3 = new InstructionNode;
    InstructionNode* ins4 = new InstructionNode;
    InstructionNode* getLast = ins1;
    ins3 -> type = JMP;
    ins3 -> jmp_inst.target = ins1;

    ins4 -> type = NOOP;

    while (getLast -> next != nullptr){
        
        getLast = getLast -> next;
    }

    getLast -> next = ins3;
    ins3 -> next = ins4;
    ins1 -> cjmp_inst.target = ins3 -> next;

    return ins1;
}

InstructionNode* Parser::parseIfStmt() {
    
    Token token;
    InstructionNode* ins1 = new InstructionNode;
    lexer.GetToken();

    ins1 -> type = CJMP;
    InstructionNode* ins2 = parseCondition();
    ins1 -> cjmp_inst.operand1_index = ins2->cjmp_inst.operand1_index;
    ins1 -> cjmp_inst.condition_op = ins2->cjmp_inst.condition_op;
    ins1 -> cjmp_inst.operand2_index = ins2->cjmp_inst.operand2_index;

    token = peek();
    if(token.token_type != LBRACE){
        
    } else {

        ins1->next = parseBody();
    }

    InstructionNode* ins3 = new InstructionNode;
    InstructionNode* getLast = ins1;
    ins3 -> type = NOOP;

    while(getLast->next != nullptr) {

        getLast = getLast -> next;
    }

    getLast -> next = ins3;
    ins1 -> cjmp_inst.target = getLast -> next;

    return ins1;
}

InstructionNode* Parser::parseCondition() {

    Token token;
    InstructionNode* ins1 = new InstructionNode;

    token = peek();
    if (token.token_type != ID && token.token_type != NUM){
        
        
    } else {
        
        ins1 -> cjmp_inst.operand1_index = parsePrimary();
    }
    
    token = lexer.GetToken();
    if (token.token_type == GREATER ) {

        ins1->cjmp_inst.condition_op = CONDITION_GREATER;
    } else if (token.token_type == LESS ) {

        ins1->cjmp_inst.condition_op = CONDITION_LESS;   
    } else if  (token.token_type == NOTEQUAL) {

        ins1->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }
        
    token = peek();
    if (token.token_type != ID && token.token_type != NUM) {
        

    } else {
        
        ins1 -> cjmp_inst.operand2_index = parsePrimary();
    }

    return ins1;
}

InstructionNode* Parser::parseCaseList(int num, InstructionNode* tempNode){
   
    Token token;
    InstructionNode* ins1 = new InstructionNode;
    InstructionNode* ins2 = nullptr;

    token = peek();
    if (token.token_type != CASE){
    

    } else {
        
        ins1 = parseCase(num);
        InstructionNode* ins3 = new InstructionNode;

        ins3 -> type = JMP;
        ins3 -> jmp_inst.target = tempNode;

        InstructionNode* getLast = ins1 -> cjmp_inst.target;
        while (getLast -> next -> next != nullptr){
            
            getLast = getLast->next;
        }

        getLast->next = ins3;
    }

    token = peek();
    InstructionNode* getLast = ins1;
    if (token.token_type != CASE) {
        

    } else {

        while (getLast -> next -> next != nullptr) {

            getLast = getLast -> next;
        }

        ins2 = parseCaseList(num, tempNode);
        getLast -> next = ins2;
        return ins1;
    }

    return ins1;
}

InstructionNode* Parser::parseSwitchStmt(InstructionNode* tempNode) {
    
    Token token;
    InstructionNode* ins1 = new InstructionNode;
    lexer.GetToken();

    token = lexer.GetToken();
    int switchOp1 = myMap[token.lexeme];
    lexer.GetToken();
    
    token = peek();
    if(token.token_type != CASE){


    } else {

        ins1 = parseCaseList(switchOp1, tempNode);
    }

    token = peek();
    if (token.token_type == DEFAULT){

        InstructionNode* ins2 = ins1;
        while (ins2 -> next -> next != nullptr) {

            ins2 = ins2 -> next;
        }

        InstructionNode* ins3 = new InstructionNode;
        lexer.GetToken();
        lexer.GetToken();
        
        token = peek();
        if (token.token_type == LBRACE) {

            ins3 = parseBody();
        } 

        ins2 -> next = ins3;
        lexer.GetToken();

    } else if (token.token_type == RBRACE) {

        token = lexer.GetToken();
        return ins1;
    } 

    return ins1;
}

InstructionNode* Parser::parseCase(int num){
    
    Token token;
    int index = next_available;
    InstructionNode* ins1 = new InstructionNode;

    lexer.GetToken();
    ins1 -> type = CJMP;
    ins1 -> cjmp_inst.operand1_index = num;
    ins1 -> cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    token = lexer.GetToken();

    mem[next_available++] = stoi(token.lexeme);
    ins1->cjmp_inst.operand2_index = index;

    lexer.GetToken();
    token = peek();
    if (token.token_type != LBRACE) {
        

    } else {

        ins1->cjmp_inst.target = parseBody();   
    }

    InstructionNode* ins2 = new InstructionNode;
    InstructionNode* ins3 = ins1 -> cjmp_inst.target;
    ins2 -> type = NOOP;

    while (ins3 -> next != nullptr) {

        ins3 = ins3 -> next;
    }

    ins1 -> next = ins2;
    ins3 -> next = ins2;

    return ins1;
}

InstructionNode* Parser::parseForStmt(){
    
    Token token;
    InstructionNode* ins1 = new InstructionNode;
    InstructionNode* ins2 = new InstructionNode;
    InstructionNode* ins3 = new InstructionNode;

    lexer.GetToken();
    lexer.GetToken();

    token = peek();
    if (token.token_type != ID) {
        

    } else { 
        
        ins1 = parseAssignStmt();
    }

    ins3 -> type = CJMP;
    InstructionNode* tempNode = parseCondition();
    ins3 -> cjmp_inst.operand1_index = tempNode -> cjmp_inst.operand1_index;
    ins3 -> cjmp_inst.condition_op = tempNode ->cjmp_inst.condition_op;
    ins3 -> cjmp_inst.operand2_index = tempNode -> cjmp_inst.operand2_index;

    lexer.GetToken();
    token = peek();
    if (token.token_type != ID) {
        

    } else {
        
        ins2 = parseAssignStmt();
        lexer.GetToken();
    }

    token = peek();
    if (token.token_type != LBRACE){
        

    } else {
        
        ins3 -> next = parseBody();
    }

    InstructionNode* ins4 = ins3 -> next;
    InstructionNode* ins5 = new InstructionNode;
    InstructionNode* ins6 = new InstructionNode;
    InstructionNode* ins7 = ins3;
    while (ins4 -> next != nullptr) {

        ins4 = ins4 -> next;
    }

    ins4 -> next = ins2;
    ins5 -> type = JMP;
    ins5 -> jmp_inst.target = ins3;

    ins6 -> type = NOOP;
    ins5 -> next = ins6;

    ins3 -> cjmp_inst.target = ins6;
    ins1 -> next = ins3;

    while (ins7 -> next != nullptr) {

        ins7 = ins7 -> next;
    }

    ins7 -> next = ins5;

    return ins1;
}

Token Parser::peek() {
    
    Token token;
    token = lexer.GetToken();
    lexer.UngetToken(token);

    return token;
}
