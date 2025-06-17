#include "parser.hpp"
#include <iostream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<string>
using namespace std;

parser::parser(){

}

void parser::initialise(vector<TOKEN *> *TOKEN_LIST_ADDRESS){
    LOCAL_COPY_TOKEN_STREAM.clear();
    LOCAL_COPY_TOKEN_STREAM = *(TOKEN_LIST_ADDRESS);
    token_number=0;
    CURRENT_TOKEN = LOCAL_COPY_TOKEN_STREAM[token_number];
    syntaxError=false;
}

AST_NODE* parser::parseCONDITION(){
    //( ID REL_OP INT/STRING )
    proceed(TOKEN_LEFT_PAREN);
    AST_NODE* CONDITION_NODE=new AST_NODE;

    CONDITION_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;

    switch(CURRENT_TOKEN->TOKEN_TYPE){
        case TOKEN_LESS_THAN    :
        {
            CONDITION_NODE->NODE_TYPE=NODE_CONDITION_LESS_THAN; 
            proceed(TOKEN_LESS_THAN);
            break;
        }
        case TOKEN_GREATER_THAN :
        {
            CONDITION_NODE->NODE_TYPE=NODE_CONDITION_GREATER_THAN;
            proceed(TOKEN_GREATER_THAN);
            break;
        }
        case TOKEN_EQUALS       :
        {
            CONDITION_NODE->NODE_TYPE=NODE_CONDITION_EQUALS;
            proceed(TOKEN_EQUALS);
            break;
        }
        default                 : throwSyntaxError();
    } 

    if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_INTEGER){
        CONDITION_NODE->SUB_PAY_LOAD=&checkAndProceed(TOKEN_INTEGER)->VALUE;
        CONDITION_NODE->SUB_PAY_LOAD_TYPE="INT";
    }
    else if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_STRING){
        CONDITION_NODE->SUB_PAY_LOAD=&checkAndProceed(TOKEN_STRING)->VALUE;
        CONDITION_NODE->SUB_PAY_LOAD_TYPE="STRING";
    }
    else{
        throwSyntaxError();
    }
    proceed(TOKEN_RIGHT_PAREN);
    return CONDITION_NODE;
}

PARSER_STATUS parser::parseCreate(){
    //CREATE NEW DATABASE DB1
    //CREATE NEW TABLE T1(INT ID,STRING NAME,...)
    
    EVALUATED_NODE=new AST_NODE;
    proceed(TOKEN_CREATE);
    proceed(TOKEN_NEW);
    if(syntaxError==true){
        return throwSyntaxError();
    }
    if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_DATABASE){
        EVALUATED_NODE->NODE_TYPE=NODE_CREATE_DATABASE;
        proceed(TOKEN_DATABASE);
        if(syntaxError==true){
            return throwSyntaxError();
        }
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
    }
    else if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_TABLE){
        EVALUATED_NODE->NODE_TYPE=NODE_CREATE_TABLE;
        proceed(TOKEN_TABLE);
        EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
        if(syntaxError){
            return throwSyntaxError();
        }
        proceed(TOKEN_LEFT_PAREN);
        if(syntaxError){
            return throwSyntaxError();
        }
        while(true){
            if(CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_INT&&CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_STR){
                return throwSyntaxError();
            }
            AST_NODE* columndef = new AST_NODE;
            columndef->NODE_TYPE=(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_INT)? NODE_INTEGER : NODE_STRING;
            proceed(CURRENT_TOKEN->TOKEN_TYPE);
            if(CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_ID){
                return throwSyntaxError();
            }
            columndef->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;

            EVALUATED_NODE->CHILDREN.push_back(columndef);

            if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_RIGHT_PAREN){
                proceed(TOKEN_RIGHT_PAREN);
                break;
            }
            proceed(TOKEN_COMMA);
            if(syntaxError){
                return throwSyntaxError();
            }
        }
    }
    else{
        return throwSyntaxError();
    }
    check(TOKEN_END_OF_INPUT);

    return PARSER_SUCCESS;
}

PARSER_STATUS parser::parseUse(){

    //USE DB1

    EVALUATED_NODE = new AST_NODE;
    proceed(TOKEN_USE);
    EVALUATED_NODE->NODE_TYPE=NODE_USE;
    EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
    check(TOKEN_END_OF_INPUT);

    return PARSER_SUCCESS;
}

AST_NODE* parser:: parseCHILDREN(){
    AST_NODE* NEW_CHILD_NODE=new AST_NODE;
    NEW_CHILD_NODE->NODE_TYPE=(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_INTEGER)? NODE_INTEGER : NODE_STRING;
    NEW_CHILD_NODE->PAYLOAD=&CURRENT_TOKEN->VALUE;
    if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_INTEGER){
        proceed(TOKEN_INTEGER);
    }
    else{
        proceed(TOKEN_STRING);
    }
    return NEW_CHILD_NODE;
}

PARSER_STATUS parser::parseInsert(){
    //insert into T1 VALUE (230101101,"SWAPNITH",...)
    EVALUATED_NODE = new AST_NODE;
    proceed(TOKEN_INSERT);
    proceed(TOKEN_INTO);
    if(syntaxError){
        return throwSyntaxError();
    }
    EVALUATED_NODE->NODE_TYPE=NODE_INSERT;
    EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
    proceed(TOKEN_VALUE);
    if(syntaxError){
        return throwSyntaxError();
    }
    proceed(TOKEN_LEFT_PAREN);
    if(syntaxError){
        return throwSyntaxError();
    }
    while(true){
        if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_END_OF_INPUT){
            return throwSyntaxError();
        }
        if(CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_INTEGER&&CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_STRING){
            return throwSyntaxError();
        }
        EVALUATED_NODE->CHILDREN.push_back(parseCHILDREN());
        if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_RIGHT_PAREN){
            proceed(TOKEN_RIGHT_PAREN);
            break;
        }
        proceed(TOKEN_COMMA);
        if(syntaxError){
            return throwSyntaxError();
        }
    }
    check(TOKEN_END_OF_INPUT);
    return PARSER_SUCCESS;
}

PARSER_STATUS parser::parseSelect(){
    //SELECT FROM T1 WHERE (<CONDITION>)
    EVALUATED_NODE=new AST_NODE;
    EVALUATED_NODE->NODE_TYPE=NODE_SELECT;
    proceed(TOKEN_SELECT);
    proceed(TOKEN_FROM);
    if(syntaxError){
        return throwSyntaxError();
    }
    EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
    if(syntaxError){
        return throwSyntaxError();
    }
    proceed(TOKEN_WHERE);
    if(syntaxError){
        return throwSyntaxError();
    }
    EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());
    check(TOKEN_END_OF_INPUT);
    return PARSER_SUCCESS;
}

PARSER_STATUS parser::parseDelete(){
    //DELETE FROM T1 WHERE (<CONDITON>)
    EVALUATED_NODE=new AST_NODE;
    EVALUATED_NODE->NODE_TYPE=NODE_DELETE;
    checkAndProceed(TOKEN_DELETE);
    checkAndProceed(TOKEN_FROM);
    if(syntaxError){
        return throwSyntaxError();
    }
    EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
    checkAndProceed(TOKEN_WHERE);
    if(syntaxError){
        return throwSyntaxError();
    }
    EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());
    check(TOKEN_END_OF_INPUT);
    return PARSER_SUCCESS;
}

PARSER_STATUS parser::parseUpdate(){
    //UPDATE T1 SET (<column_name>=v1,...) WHERE (<CONDITION>)
    EVALUATED_NODE =new AST_NODE;
    proceed(TOKEN_UPDATE);
    EVALUATED_NODE->NODE_TYPE=NODE_UPDATE;
    EVALUATED_NODE->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
    if(syntaxError){
        return throwSyntaxError();
    }
    proceed(TOKEN_USET);
    if(syntaxError){
        return throwSyntaxError();
    }
    proceed(TOKEN_LEFT_PAREN);
    if(syntaxError){
        return throwSyntaxError();
    }

    while(true){
        if(CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_ID) return throwSyntaxError();

        AST_NODE* updatePair = new AST_NODE;
        updatePair->NODE_TYPE=NODE_ASSIGN;
        updatePair->PAYLOAD=&checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_EQUALS);
        if(syntaxError){
            return throwSyntaxError();
        }
        if(CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_INTEGER&&CURRENT_TOKEN->TOKEN_TYPE!=TOKEN_STRING){
            return throwSyntaxError();
        }
        AST_NODE* valueNode=parseCHILDREN();
        updatePair->CHILDREN.push_back(valueNode);
        EVALUATED_NODE->UPDATE_CHILDREN.push_back(updatePair);
        if(CURRENT_TOKEN->TOKEN_TYPE==TOKEN_RIGHT_PAREN){
            proceed(TOKEN_RIGHT_PAREN);
            break;
        }
        proceed(TOKEN_COMMA);
        if(syntaxError) return throwSyntaxError();
    }
    proceed(TOKEN_WHERE);
    if (syntaxError) return throwSyntaxError();

    EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());

    check(TOKEN_END_OF_INPUT);
    return PARSER_SUCCESS;
}

PARSER_STATUS parser::parseExit(){
    //exit
    EVALUATED_NODE=new AST_NODE;
    proceed(TOKEN_EXIT);
    EVALUATED_NODE->NODE_TYPE=NODE_EXIT;
    check(TOKEN_END_OF_INPUT);
    return PARSER_SUCCESS;
}

PARSER_STATUS parser:: parse(){
    switch(CURRENT_TOKEN->TOKEN_TYPE){
        case TOKEN_CREATE: return parseCreate();
        case TOKEN_USE   : return parseUse();
        case TOKEN_INSERT: return parseInsert();
        case TOKEN_SELECT: return parseSelect();
        case TOKEN_DELETE: return parseDelete();
        case TOKEN_UPDATE: return parseUpdate();
        case TOKEN_EXIT  : return parseExit();
        default          : return throwSyntaxError();
    }
    return PARSER_SUCCESS;
}
