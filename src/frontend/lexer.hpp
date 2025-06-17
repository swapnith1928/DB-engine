#ifndef __LEXER_HPP
#define __LEXER_HPP
#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
#define FAIL    "\e[0;31m"
#define SUCCESS "\e[0;32m"
#define DEFAULT "\e[0;37m"
using namespace std;

/*
    SYNTAX:

        READING   : SELECT FROM <TABLE_NAME> WHERE (<CONDITON>)
        DELETE    : DELETE FROM <TABLE_NAME> WHERE (<CONDITON>)
        UPDATE    : UPDATE <TABLE_NAME> SET (COLUMN1=VAL1,..) WHERE (<CONDITION>)
        CREATION  : CREATE NEW DATABASE <DB_NAME>
                    CREATE NEW TABLE <TABLE_NAME> (INT ID,STRING DATA,...) 
        USE DATABASE
        INSERTION : INSERT INTO <DB_NAME> VALUE (<ELEMENT1>,....)
        =,<,>
*/

typedef enum{
    LEXER_FAIL,
    LEXER_SUCCESS
}LEXER_STATUS;

typedef enum{
    TOKEN_INSERT,
    TOKEN_INTO,
    TOKEN_STRING,
    TOKEN_VALUE,
    TOKEN_DELETE,
    TOKEN_FROM,
    TOKEN_SELECT,
    TOKEN_CREATE,
    TOKEN_NEW,
    TOKEN_USE,
    TOKEN_DATABASE,
    TOKEN_TABLE,
    TOKEN_UPDATE,
    TOKEN_WHERE,
    TOKEN_USET,
    TOKEN_EQUALS,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_INTEGER,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_COMMA,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_STR,
    TOKEN_EXIT,
    TOKEN_END_OF_INPUT
}TOKEN_SET;

struct TOKEN{
    TOKEN_SET TOKEN_TYPE;
    string VALUE;
};
unordered_map<string,TOKEN_SET> KEYWORD_MAP={
    {"insert",TOKEN_INSERT},
    {"into"  ,TOKEN_INTO},
    {"values" ,TOKEN_VALUE},
    {"delete",TOKEN_DELETE},
    {"from"  ,TOKEN_FROM},
    {"select",TOKEN_SELECT},
    {"use",TOKEN_USE},
    {"database",TOKEN_DATABASE},
    {"table",TOKEN_TABLE},
    {"new",TOKEN_NEW},
    {"create",TOKEN_CREATE},
    {"update",TOKEN_UPDATE},
    {"where",TOKEN_WHERE},
    {"set",TOKEN_USET},
    {"int",TOKEN_INT},
    {"string",TOKEN_STR},
    {"exit",TOKEN_EXIT},

    {"INSERT",TOKEN_INSERT},
    {"INTO"  ,TOKEN_INTO},
    {"VALUEs" ,TOKEN_VALUE},
    {"DELETE",TOKEN_DELETE},
    {"FROM"  ,TOKEN_FROM},
    {"SELECT",TOKEN_SELECT},
    {"USE",TOKEN_USE},
    {"DATABASE",TOKEN_DATABASE},
    {"TABLE",TOKEN_TABLE},
    {"NEW",TOKEN_NEW},
    {"CREATE",TOKEN_CREATE},
    {"UPDATE",TOKEN_UPDATE},
    {"WHERE",TOKEN_WHERE},
    {"SET",TOKEN_USET},
    {"INT",TOKEN_INT},
    {"STRING",TOKEN_STR},
    {"EXIT",TOKEN_EXIT}
};
string TokenTypeToString(TOKEN_SET REQUIRED_TOKEN){
    switch(REQUIRED_TOKEN){
        case TOKEN_INSERT       : return "TOKEN_INSERT";
        case TOKEN_INTO         : return "TOKEN_INTO";
        case TOKEN_STRING       : return "TOKEN_STRING";
        case TOKEN_DELETE       : return "TOKEN_DELETE";
        case TOKEN_FROM         : return "TOKEN_FROM";
        case TOKEN_SELECT       : return "TOKEN_SELECT";
        case TOKEN_CREATE       : return "TOKEN_CREATE";
        case TOKEN_DATABASE     : return "TOKEN_DATABASE";
        case TOKEN_NEW          : return "TOKEN_NEW";
        case TOKEN_USE          : return "TOKEN_USE";
        case TOKEN_TABLE        : return "TOKEN_TABLE";
        case TOKEN_UPDATE       : return "TOKEN_UPDATE";
        case TOKEN_WHERE        : return "TOKEN_WHERE";
        case TOKEN_EQUALS       : return "TOKEN_EQUALS";
        case TOKEN_GREATER_THAN : return "TOKEN_GREATER_THAN";
        case TOKEN_LESS_THAN    :  return "TOKEN_LESS_THAN";
        case TOKEN_USET         : return "TOKEN_USET";
        case TOKEN_VALUE        : return "TOKEN_VALUE";
        case TOKEN_INTEGER      : return "TOKEN_INTEGER";
        case TOKEN_LEFT_PAREN   : return "TOKEN_LEFT_PAREN";
        case TOKEN_RIGHT_PAREN  : return "TOKEN_RIGHT_PAREN";
        case TOKEN_COMMA        : return "TOKEN_COMMA";
        case TOKEN_ID           : return "TOKEN_ID";
        case TOKEN_INT          : return "TOKEN_INT";
        case TOKEN_STR          : return "TOKEN_STR";
        case TOKEN_EXIT         : return "TOKEN_EXIT";
        case TOKEN_END_OF_INPUT : return "TOKEN_END_OF_INPUT";
    }
    return "[!] ERROR : UNIDENTIFIED TOKEN : " +REQUIRED_TOKEN;
}
class lexer{
    private:
        int cursor;
        int length;
        char current;
        string localInputBuff;
        vector<TOKEN *> TOKEN_LIST;
        bool stringParsingError;
        char advance(){
            if(cursor == length-1){
                current='\0';
                return current;
            }
            else{
                current=localInputBuff[++cursor];
                return current;
            }
        };
        LEXER_STATUS throwLexerError(){
            cout<<FAIL<<"[!] LEXER ERROR : UNIDENTIFIED CHARECTER AT INDEX "<<cursor<<" : "<<current<<endl<<DEFAULT;
            return LEXER_FAIL;
        }
        LEXER_STATUS throwStringParsingError(){
            cout<<FAIL<<"[!] LEXER ERROR : CLOSING QUOTES NOT FOUND "<<cursor<<" : "<<current<<endl<<DEFAULT;
            return LEXER_FAIL;
        }
    public:
        lexer();//constructor
        vector<TOKEN*> *getTokenStream();
        LEXER_STATUS tokenize();
        void initialise(string InputBuff);
        void skipWhiteSpaces();
        void displayAllTokens();
        TOKEN* tokenizeID();
        TOKEN* tokenizeINTEGER();
        TOKEN* tokenizeSPECIAL(TOKEN_SET NEW_TOKEN_TYPE);
        TOKEN* tokenizeSTRING();
};

#endif