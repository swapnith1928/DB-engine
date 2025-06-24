#include "exec.hpp"
#include "../storage/storage.hpp"
#include <iostream>
#include <fstream>
#include "director_utils.hpp"
using namespace std;

#define FAIL    "\e[0;31m"
#define SUCCESS "\e[0;32m"
#define DEFAULT "\e[0;37m"

StorageEngine engine;
string currentDB = "";

string getTableFilePath(const string& table) {
    if (currentDB == "") return "";
    return "databases/" + currentDB + "/" + table;
}

EXEC_STATUS execute(AST_NODE* node) {
    switch (node->NODE_TYPE) {
        case NODE_CREATE_TABLE: {
            TableSchema schema;
            schema.name = getTableFilePath(*node->PAYLOAD);
            if (schema.name == "") {
                cout <<FAIL<<"[!] ERROR : SELECT A DATABASE OR CREATE A NEW ONE\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            for (auto* col : node->CHILDREN) {
                schema.columns.push_back({*col->PAYLOAD, (col->NODE_TYPE == NODE_INTEGER ? "INT" : "STRING")});
            }
            bool status=engine.createTable(schema);
            if(!status){
                return EXEC_FAIL;
            }
            cout << "Table created.\n";
            return EXEC_SUCCESS;
            break;
        }
        case NODE_INSERT: {
            if(currentDB==""){
                cout <<FAIL<<"[!] ERROR : SELECT A DATABASE OR CREATE A NEW ONE\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }

            vector<string> row;
            vector<string> type;
            for (auto* val : node->CHILDREN) {
                row.push_back(*val->PAYLOAD);
                if(val->NODE_TYPE==NODE_INTEGER){
                    type.push_back("INT");
                }
                else if(val->NODE_TYPE==NODE_STRING){
                    type.push_back("STRING");
                }
            }
            bool status = engine.insertRecord(getTableFilePath(*node->PAYLOAD), row,type);
            if(!status){
                return EXEC_FAIL;
            }
            cout << "Record inserted.\n";
            return EXEC_SUCCESS;
            break;
        }
        case NODE_SELECT: {

            if(currentDB==""){
                cout <<FAIL<<"[!] ERROR : SELECT A DATABASE OR CREATE A NEW ONE\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }

            AST_NODE* cond = node->CHILDREN[0];
            string op = (cond->NODE_TYPE == NODE_CONDITION_GREATER_THAN ? ">" :
                         cond->NODE_TYPE == NODE_CONDITION_LESS_THAN ? "<" : "=");

            string tableName = getTableFilePath(*node->PAYLOAD);
            string columnName = *cond->PAYLOAD;
            int colIndex = engine.getColumnIndex(tableName, columnName);

            if (colIndex == -1) {
                cout << FAIL<<"[!] ERROR : Column '" << columnName << "' not found.\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            else if(colIndex==-2){
                cout<<FAIL<<"[!] ERROR : Table '"<<tableName<<"'does not exist.\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            if(!checkValidCond(tableName,colIndex,cond->SUB_PAY_LOAD_TYPE)){
                return EXEC_FAIL;
            }
            auto results = engine.selectWhere(tableName, colIndex, op, *cond->SUB_PAY_LOAD);
            for (auto& row : results) {
                for (auto& col : row) cout << col << " ";
                cout << "\n";
            }
            return EXEC_SUCCESS;
            break;
        }
        case NODE_DELETE: {

            if(currentDB==""){
                cout <<FAIL<<"[!] ERROR : SELECT A DATABASE OR CREATE A NEW ONE\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }

            AST_NODE* cond = node->CHILDREN[0];
            string op = (cond->NODE_TYPE == NODE_CONDITION_GREATER_THAN ? ">" :
                         cond->NODE_TYPE == NODE_CONDITION_LESS_THAN ? "<" : "=");

            string tableName = getTableFilePath(*node->PAYLOAD);
            string columnName = *cond->PAYLOAD;
            int colIndex = engine.getColumnIndex(tableName, columnName);
            if (colIndex == -1) {
                cout <<FAIL<< "[!] ERROR : Column '" << columnName << "' not found.\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            else if(colIndex==-2){
                cout<<FAIL<<"[!] ERROR : Table '"<<tableName<<"'does not exist.\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            if(!checkValidCond(tableName,colIndex,cond->SUB_PAY_LOAD_TYPE)){
                return EXEC_FAIL;
            }
            engine.deleteWhere(tableName, colIndex, op, *cond->SUB_PAY_LOAD);
            cout << "Matching rows deleted.\n";
            return EXEC_SUCCESS;
            break;
        }
        case NODE_UPDATE: {

            if(currentDB==""){
                cout <<FAIL<<"[!] ERROR : SELECT A DATABASE OR CREATE A NEW ONE\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }

            AST_NODE* cond = node->CHILDREN[0];
            string op = (cond->NODE_TYPE == NODE_CONDITION_GREATER_THAN ? ">" :
                         cond->NODE_TYPE == NODE_CONDITION_LESS_THAN ? "<" : "=");

            string tableName = getTableFilePath(*node->PAYLOAD);
            string columnName = *cond->PAYLOAD;
            int colIndex = engine.getColumnIndex(tableName, columnName);
            if (colIndex == -1) {
                cout << FAIL<<"[!] ERROR : Column '" << columnName << "' not found.\n"<<DEFAULT;
                return EXEC_FAIL;
                break;
            }
            
            ifstream schemaFile(tableName+".schema");
            if(!schemaFile.is_open()){
                return EXEC_FAIL;
            }
            vector<string> colOrder;
            string Type,name;
            while(schemaFile>>Type>>name){
                colOrder.push_back(name);
            }
            schemaFile.close();

            vector<string> newRow(colOrder.size(),"");
            vector<string> type(colOrder.size(),"");
            for(AST_NODE* assign:node->UPDATE_CHILDREN){
                string targetCol=*assign->PAYLOAD;
                int updateIndex=-1;
                for(size_t i=0;i<colOrder.size();i++){
                    if(colOrder[i]==targetCol){
                        updateIndex=static_cast<int>(i);
                        break;
                    }
                }
                if(updateIndex==-1){
                    cerr<<FAIL<<"[!] ERROR : Column '"<<targetCol<<"' does not exist.\n"<<endl<<DEFAULT;
                    return EXEC_FAIL;
                }
                newRow[updateIndex]=*assign->CHILDREN[0]->PAYLOAD;
                type[updateIndex]=assign->CHILDREN[0]->NODE_TYPE==NODE_INTEGER? "INT" : "STRING";
            }
            if(!checkValidCond(tableName,colIndex,cond->SUB_PAY_LOAD_TYPE)){
                return EXEC_FAIL;
            }
            if(!engine.updateWhere(tableName,colIndex,op,*cond->SUB_PAY_LOAD,newRow,type)){
                return EXEC_FAIL;
            }
            cout << "Matching rows updated.\n";
            return EXEC_SUCCESS;
        }
        case NODE_CREATE_DATABASE: {
            string dbName = *node->PAYLOAD;
            string dbRoot = "databases";
            string dbPath = dbRoot+"/"+dbName;

            if(!createDirectoryIfNotExists(dbRoot)){
                cout<<FAIL<<"[!] ERROR : COULD NOT CREATE 'databases' FOLDER.\n "<<DEFAULT;
                return EXEC_FAIL;
            }

            if(directoryExists(dbPath)){
                cout<<FAIL<<"[!] ERROR : Database '"<<dbName <<"' already exists.\n"<<DEFAULT;
                return EXEC_FAIL;
            }

            if(!createDirectoryIfNotExists(dbPath)){
                cout<<FAIL<<"[!] ERROR : COULD NOT CREATE Database folder\n"<<DEFAULT;
                return EXEC_FAIL;
            }
            cout<<"Database '"<<dbName<<"' created.\n";
            return EXEC_SUCCESS;
        }
        case NODE_USE: {
            string dbName = *node->PAYLOAD;
            string dbRoot = "databases";
            string dbPath = dbRoot+"/"+dbName;

            if(!directoryExists(dbPath)){
                cout<<FAIL<<"[!] ERROR: Database '"<<dbName<<"' does not exist."<<endl<<DEFAULT;
                return EXEC_FAIL;
            }

            currentDB = dbName;
            cout << "Switched to database '" << currentDB << "'.\n";
            return EXEC_SUCCESS;
            break;
        }
        case NODE_EXIT: {
            cout << "Exiting...\n";
            exit(0);
            return EXEC_SUCCESS;
        }
        default: {
            cout << "Unrecognized operation.\n";
            return EXEC_FAIL;
        }
    }
}
