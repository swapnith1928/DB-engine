#include "lexer.hpp"
#include <iostream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<string>
using namespace std;

lexer::lexer(){

}

vector<TOKEN *> *lexer::getTokenStream(){
    return &TOKEN_LIST;
}

void lexer::skipWhiteSpaces(){
    while(current==' '&&current!='\0'){
        advance();
    }
}

TOKEN* lexer::tokenizeSTRING(){
    advance();
    TOKEN* newToken =new TOKEN;
    string tempBuff="";
    while(current!='"'){
        if(current=='\0'){
            stringParsingError=true;
            break;
        }
        tempBuff.push_back(current);
        advance();
    }
    advance();
    newToken->TOKEN_TYPE=TOKEN_STRING;
    newToken->VALUE=tempBuff;
    return newToken;
}

TOKEN* lexer::tokenizeID(){
    TOKEN* newTOKEN = new TOKEN;
    string tempBuff="";
    while(isalnum(current)||current=='_'){
        tempBuff.push_back(current);
        advance();
    }
    newTOKEN->TOKEN_TYPE = TOKEN_ID;
    newTOKEN->VALUE=tempBuff;

    if(KEYWORD_MAP.find(newTOKEN->VALUE)!=KEYWORD_MAP.end()){
        newTOKEN->TOKEN_TYPE=KEYWORD_MAP[newTOKEN->VALUE];
    }
    return newTOKEN;
}

TOKEN* lexer:: tokenizeINTEGER(){
    TOKEN* newTOKEN = new TOKEN;
    string tempBuff="";
    while(isdigit(current)){
        tempBuff.push_back(current);
        advance();
    }
    newTOKEN->TOKEN_TYPE=TOKEN_INTEGER;
    newTOKEN->VALUE=tempBuff;
    return newTOKEN;
}

TOKEN* lexer:: tokenizeSPECIAL(TOKEN_SET NEW_TOKEN_TYPE){
    TOKEN* newToken = new TOKEN;
    newToken->TOKEN_TYPE = NEW_TOKEN_TYPE;
    newToken->VALUE = current;
    advance();
    return newToken;
}

void lexer:: displayAllTokens(){
    int counter=0;
    for(TOKEN* CURRENT_TOKEN : TOKEN_LIST){
        cout<<++counter<<")"<<CURRENT_TOKEN->VALUE<<" ";
        cout<<TokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE)<<endl;
    }
}

LEXER_STATUS lexer::tokenize(){
    while(current){
        skipWhiteSpaces();
        if(isalpha(current)||current=='_'){
            TOKEN_LIST.push_back(tokenizeID());
        }
        else if(isdigit(current)){
            TOKEN_LIST.push_back(tokenizeINTEGER());
        }
        else{
            switch(current){
                case '(':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LEFT_PAREN));
                    break;
                }
                case ')':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_RIGHT_PAREN));
                    break;
                }
                case ',':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_COMMA));
                    break;
                }
                case '<':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN));
                    break;
                }
                case '>':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN));
                    break;
                }
                case '"':{
                    TOKEN_LIST.push_back(tokenizeSTRING());
                    if(stringParsingError){
                        return throwStringParsingError();
                    }
                    break;
                }
                case '=':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_EQUALS));
                    break;
                }
                case '\0' : break;
                default: return throwLexerError();
            }
        }
    }
    //displayAllTokens();
    TOKEN* END_TOKEN=new TOKEN;
    END_TOKEN->TOKEN_TYPE=TOKEN_END_OF_INPUT;
    TOKEN_LIST.push_back(END_TOKEN);
    return LEXER_SUCCESS;
}

void lexer::initialise(string InputBuff){
    cursor=0;
    length=InputBuff.size();
    localInputBuff=InputBuff;
    current=localInputBuff[cursor];
    TOKEN_LIST.clear();
    stringParsingError=false;
}