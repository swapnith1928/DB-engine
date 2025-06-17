#include "src/frontend/lexer.cpp"
#include "src/frontend/parser.cpp"
#include "src/backend/storage/storage.cpp"
#include "src/backend/execution/exec.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>       //for time 

#define FAIL    "\e[0;31m"
#define SUCCESS "\e[0;32m"
#define DEFAULT "\e[0;37m"

using namespace std;

class EvaluationWrapper {
private:
    lexer* MAIN_LEXER;
    parser* MAIN_PARSER;
    int commandCount;

public:
    EvaluationWrapper() {
        MAIN_LEXER = new lexer();
        MAIN_PARSER = new parser();
        commandCount = 0;
    }

    void handle(string InputBuff) {
        auto startTimer = chrono::high_resolution_clock::now();

        MAIN_LEXER->initialise(InputBuff);
        LEXER_STATUS CURR_LEXER_STAT = MAIN_LEXER->tokenize();
        PARSER_STATUS CURR_PARSER_STAT = PARSER_FAIL;
        EXEC_STATUS CURR_EXEC_STAT=EXEC_SUCCESS;

        if (CURR_LEXER_STAT == LEXER_SUCCESS) {
            MAIN_PARSER->initialise(MAIN_LEXER->getTokenStream());
            CURR_PARSER_STAT = MAIN_PARSER->parse();
        }

        if(CURR_PARSER_STAT==PARSER_SUCCESS){
            CURR_EXEC_STAT=execute(MAIN_PARSER->EVALUATED_NODE);
        }
        auto stopTimer = chrono::high_resolution_clock::now();
        auto time = chrono::duration_cast<chrono::milliseconds>(stopTimer - startTimer);
        commandCount++;

        if (CURR_LEXER_STAT == LEXER_FAIL || CURR_PARSER_STAT == PARSER_FAIL || CURR_EXEC_STAT==EXEC_FAIL) {
            cout << FAIL << "$ command ID -> " << commandCount << " failed in " << time.count() << "ms\n\n" << DEFAULT;
            return;
        } else {
            cout << SUCCESS << "$ command ID -> " << commandCount << " executed in " << time.count() << "ms\n\n" << DEFAULT;
        }
    }
};

int main() {
    system("cls");
    string InputBuff;
    EvaluationWrapper* main_io = new EvaluationWrapper();

    while (true) {
        cout << DEFAULT << "db_engine : ";
        getline(cin, InputBuff);
        main_io->handle(InputBuff);
    }
    return 0;
}
