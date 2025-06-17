#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "../storage/storage.hpp"
#include "../../frontend/parser.hpp"
#include <string>
using namespace std;

typedef enum{
    EXEC_FAIL,
    EXEC_SUCCESS
}EXEC_STATUS;

extern string currentDB;

EXEC_STATUS execute(AST_NODE* node);

#endif
