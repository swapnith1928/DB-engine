#pragma once
#include <string>
#include <vector>
using namespace std;
struct Column {
    string name;
    string type; // "INT" or "STRING"
};

struct TableSchema {
    string name;
    vector<Column> columns;
};

class StorageEngine {
public:
    bool createTable(const TableSchema& schema);
    bool insertRecord(const string& table, const vector<string>& record,const vector<string>& type);
    vector<vector<string>> selectWhere(const string& table, int colIndex, const string& op, const string& value);
    bool deleteWhere(const string& table, int colIndex, const string& op, const string& value);
    bool updateWhere(const string& table, int colIndex, const string& op, const string& value, const vector<string>& newRecord,vector<string>& type);
    int getColumnIndex(const string& table, const string& colName);
};
