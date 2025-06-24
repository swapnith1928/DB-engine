#include "storage.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

#define FAIL    "\e[0;31m"
#define SUCCESS "\e[0;32m"
#define DEFAULT "\e[0;37m"

using namespace std;

int StorageEngine::getColumnIndex(const string& table, const string& colName) {
    ifstream schema(table + ".schema");
    if(!schema.is_open()){
        return -2;
    }
    string line;
    int index = 0;

    while (getline(schema, line)) {
        stringstream ss(line);
        string type, name;
        ss >> type >> name;
        if (name == colName)
            return index;
        index++;
    }
    return -1; // not found
}

bool checkValidCond(const string& table,int colIndex,const string& valueType){
    ifstream schema(table+".schema");
    string TYPE,NAME;
    for(int i=0;i<=colIndex;i++){
        schema>>TYPE>>NAME;
    }
    //schema.close();
    if(TYPE!=valueType){
        cout<<FAIL<<"[!] ERROR : Mismatch in datatypes of the above condition.\n"<<DEFAULT;
        return false;
    }
    return true;
}

bool StorageEngine::createTable(const TableSchema& schema) {
    string schemaPath=schema.name+".schema";
    string tablePath=schema.name+".txt";
    ifstream check(schemaPath);
    if(check.is_open()){
        cerr<<FAIL<<"[!] ERROR : TABLE '"<<schema.name<<"' already exists.\n"<<DEFAULT;
        return false;
    }
    
    ofstream out(schemaPath);
    for (const auto& col : schema.columns) {
        out << col.type << " " << col.name << "\n";
    }
    out.close();

    ofstream tableFile(schema.name + ".txt");
    tableFile.close();
    return true;
}

bool StorageEngine::insertRecord(const string& table, const vector<string>& record,const vector<string>& ltype) {
    ifstream schemaFile(table+".schema");
    if(!schemaFile.is_open()){
        cerr<<FAIL<<"[!] ERROR: could not open schema for table '"<<table<<"'.\n"<<DEFAULT;
        return false;
    }

    vector<Column> columns;
    string type,name;
    while(schemaFile>>type>>name){
        columns.push_back({name,type});
    }
    schemaFile.close();

    if(record.size()!=columns.size()){
        cerr<<FAIL<<"[!] ERROR: Expected "<<columns.size()<<" values, but got "<<record.size()<<" value.\n"<<DEFAULT;
        return false;
    }
    
    for(size_t i=0;i<record.size();i++){
        const string &val=record[i];
        const string&ctype=columns[i].type;
        const string &rtype=ltype[i];
        if(ctype!=rtype){
                cerr <<FAIL<< "[!] ERROR: Incorrect Datatypes\n"<<DEFAULT;
                return false;
        }
    }
    ifstream in(table + ".txt");
    vector<vector<string>> records;
    string line;

    while (getline(in, line)) {
        stringstream ss(line);
        string val;
        vector<string> row;
        while (getline(ss, val, ',')) row.push_back(val);
        records.push_back(row);
    }
    in.close();

    for(const auto& row: records){
        if(!row.empty()&&row[0]==record[0]){
            cerr <<FAIL<< "[!] ERROR: Duplicate primary-key value '" << record[0]<< "' in column '" << columns[0].name << "'.\n"<<DEFAULT;
            return false;
        }
    }

    records.push_back(record);

    ofstream out(table + ".txt", ios::trunc);
    for (const auto& r : records) {
        for (size_t i = 0; i < r.size(); ++i) {
            out << r[i];
            if (i != r.size() - 1) out << ",";
        }
        out << "\n";
    }
    out.close();
    return true;
}

vector<vector<string>> StorageEngine::selectWhere(const string& table, int colIndex, const string& op, const string& value) {
    ifstream in(table + ".txt");

    string line;
    vector<vector<string>> result;

    while (getline(in, line)) {
        stringstream ss(line);
        string val;
        vector<string> row;
        while (getline(ss, val, ',')) row.push_back(val);

        if (colIndex < row.size()) {
            if ((op == ">" && row[colIndex] > value) ||
                (op == "<" && row[colIndex] < value) ||
                (op == "=" && row[colIndex] == value)) {
                result.push_back(row);
            }
        }
    }
    return result;
}

bool StorageEngine::deleteWhere(const string& table, int colIndex, const string& op, const string& value) {
    ifstream in(table + ".txt");
    if(!in.is_open()){
        cerr<<FAIL<<"[!] ERROR: Table '"<<table<<"'does not exist.\n"<<DEFAULT;
        return false;
    }
    vector<vector<string>> records;
    string line;

    while (getline(in, line)) {
        stringstream ss(line);
        string val;
        vector<string> row;
        while (getline(ss, val, ',')) row.push_back(val);
        records.push_back(row);
    }
    in.close();

    ofstream out(table + ".txt", ios::trunc);
    for (const auto& row : records) {
        if (!((op == ">" && row[colIndex] > value) ||
              (op == "<" && row[colIndex] < value) ||
              (op == "=" && row[colIndex] == value))) {
            for (size_t i = 0; i < row.size(); ++i) {
                out << row[i];
                if (i != row.size() - 1) out << ",";
            }
            out << "\n";
        }
    }
    out.close();
    return true;
}

bool StorageEngine::updateWhere(const string& table,int colIndex,const string& op,const string& value,const vector<string>& newRecord,vector<string>& type){

    ifstream in(table + ".txt");
    if(!in.is_open()){
        cerr<<FAIL<<"[!] ERROR: Table '"<<table<<"'does not exist.\n"<<DEFAULT;
        return false;
    }
    vector<vector<string>> records;
    string line;

    // Read all records
    while (getline(in, line)) {
        stringstream ss(line);
        string val;
        vector<string> row;
        while (getline(ss, val, ',')) row.push_back(val);
        records.push_back(row);
    }
    in.close();


    ifstream schema(table+".schema");
    vector<string> COL_TYPE;
    string TYPE,NAME;
    while(schema>>TYPE>>NAME){
        COL_TYPE.push_back(TYPE);
    }
    schema.close();

    // Apply updates
    vector<vector<string>> updatedRecords;
    for (const auto& row : records) {
        if (colIndex < row.size()) {
            bool match = false;
            if (op == ">" && row[colIndex] > value) match = true;
            else if (op == "<" && row[colIndex] < value) match = true;
            else if (op == "=" && row[colIndex] == value) match = true;

            if (match) {
                vector<string> updatedRow = row;
                for (size_t i = 0; i < newRecord.size(); ++i) {
                    if (!newRecord[i].empty()) {
                        if(type[i]!=COL_TYPE[i]){
                            cerr<<FAIL<<"[!] ERROR : INVALID TYPE,EXPECTED '"<<COL_TYPE[i]<<"' FOUND '"<<type[i]<<"'."<<endl<<DEFAULT;
                            return false;
                        }
                        updatedRow[i] = newRecord[i];
                    }
                }
                updatedRecords.push_back(updatedRow);
                continue;
            }
        }
        // If not matched, keep row as is
        updatedRecords.push_back(row);
    }

    // Write back to file
    ofstream out(table + ".txt", ios::trunc);
    for (const auto& row : updatedRecords) {
        for (size_t i = 0; i < row.size(); ++i) {
            out << row[i];
            if (i != row.size() - 1) out << ",";
        }
        out << "\n";
    }
    out.close();
    return true;
}
