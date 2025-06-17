#ifndef __PARSER_HPP
#define __PARSER_HPP
#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
#include "lexer.hpp"
using namespace std;


typedef enum{
    PARSER_SUCCESS,
    PARSER_FAIL
}PARSER_STATUS;

typedef enum{
    NODE_CREATE_DATABASE,
    NODE_CREATE_TABLE,
    NODE_USE,
    NODE_INSERT,
    NODE_SELECT,
    NODE_DELETE,
    NODE_UPDATE,
    NODE_ASSIGN,
    NODE_SUB_VALUES,
    NODE_STRING,
    NODE_INTEGER,
    NODE_CONDITION_EQUALS,
    NODE_CONDITION_LESS_THAN,
    NODE_CONDITION_GREATER_THAN,
    NODE_EXIT
}NODE_SET;

struct AST_NODE{
    NODE_SET NODE_TYPE;
    string * PAYLOAD;
    string * SUB_PAY_LOAD;
    string SUB_PAY_LOAD_TYPE;
    vector<AST_NODE *> CHILDREN;
    vector<AST_NODE *> UPDATE_CHILDREN;
};

string nodeTypeToString(NODE_SET REQUIRED_NODE){
    switch(REQUIRED_NODE){
        case NODE_CREATE_DATABASE        : return "NODE_CREATE_DATABASE"; 
        case NODE_CREATE_TABLE           : return "NODE_CREATE_TABLE"; 
        case NODE_USE                    : return "NODE_USE";
        case NODE_SELECT                 : return "NODE_SELECT";
        case NODE_SUB_VALUES             : return "NODE_SUB_VALUES";
        case NODE_UPDATE                 : return "NODE_UPDATE";
        case NODE_DELETE                 : return "NODE_DELETE";
        case NODE_STRING                 : return "NODE_STRING";
        case NODE_INTEGER                : return "NODE_INTEGER";
        case NODE_CONDITION_EQUALS       : return "NODE_CONDITION_EQUALS";
        case NODE_CONDITION_LESS_THAN    : return "NODE_CONDITION_LESS_THAN";
        case NODE_CONDITION_GREATER_THAN : return "NODE_CONDITION_GREATER_THAN";
        case NODE_EXIT                   : return "NODE_EXIT"; 
    }
    return "[!] UNIDENTIFIED NODE : " + REQUIRED_NODE;
}

class parser{
    private:
        TOKEN* CURRENT_TOKEN;
        vector<TOKEN*> LOCAL_COPY_TOKEN_STREAM;
        int token_number;
        bool syntaxError;
        PARSER_STATUS throwSyntaxError(){
            cout<<FAIL<<"[!] SYNTAX ERROR : UNEXPECTED TOKEN : "<< TokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE)<<DEFAULT<<endl<<endl;
            return PARSER_FAIL;
        }

        void check(TOKEN_SET REQUIRED_CHECK_TOKEN){
            if(CURRENT_TOKEN->TOKEN_TYPE != REQUIRED_CHECK_TOKEN){
                throwSyntaxError();
            }
        }
        
        TOKEN* proceed(TOKEN_SET REQUIRED_TOKEN){
            if(CURRENT_TOKEN->TOKEN_TYPE!=REQUIRED_TOKEN){
                syntaxError=true;
                return CURRENT_TOKEN;
            }
            token_number++;
            CURRENT_TOKEN=LOCAL_COPY_TOKEN_STREAM[token_number];
            return CURRENT_TOKEN;
        }

        TOKEN* checkAndProceed(TOKEN_SET REQUIRED_TOKEN){
            TOKEN* buffPointer=CURRENT_TOKEN;
            proceed(REQUIRED_TOKEN);
            return buffPointer;
        }

    public:
    AST_NODE* EVALUATED_NODE;
    parser();
    PARSER_STATUS parse();
    void initialise(vector<TOKEN*> *TOKEN_LIST_ADDRESS);
    PARSER_STATUS parseCreate();
    PARSER_STATUS parseUse();
    PARSER_STATUS parseInsert();
    PARSER_STATUS parseSelect();
    PARSER_STATUS parseDelete();
    PARSER_STATUS parseUpdate();
    PARSER_STATUS parseExit();
    AST_NODE* parseCONDITION();
    AST_NODE* parseCHILDREN();
};
#endif