#  Custom DB Engine

This is a lightweight, file-based database engine written in C++. It parses SQL-like commands using a custom-built lexer and parser, and supports basic storage and query operations using a flat file-based backend.

##  Project Structure

```
DB engine/
├── main.cpp                    # Entry point
├── syntax.txt                  # Sample commands / syntax guide
├── src/
│   ├── frontend/               # Lexer and Parser
│   │   ├── lexer.cpp/hpp
│   │   └── parser.cpp/hpp
│   └── backend/
│       ├── execution/          # Query execution logic
│       │   ├── exec.cpp/hpp
│       │   └── director_utils.hpp
│       └── storage/            # File-based storage layer
│           ├── storage.cpp/hpp
└── .vscode/                    # Editor config
```

##  Features

-  Custom lexer and parser for SQL-like commands
-  File-based storage engine with support for:
  - Table creation
  - Record insertion
  - Record updation
  - Record deletion
  - Data retrieval (SELECT)
-  Modular architecture separating frontend and backend logic

##  Getting Started

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)

### Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/swapnith1928/DB-engine.git
```

2. Navigate into the project folder:
```bash
cd "DB engine"
g++ -std=c++17 main.cpp -o db_engine
./db_engine
```


##  Sample Syntax

You can refer to `syntax.txt` for supported commands and examples.

##  Future Improvements

-  **B-Tree Indexing**: Replace or complement the current linear search with B-Tree-based indexing for efficient query execution and record lookup.
-  **Query Optimization**: Implement a query planner and optimizer for better performance on complex queries.
-  **Support for More SQL Features**: Add AND/OR for conditons in WHERE clauses, JOINs, aggregate functions, etc.


## 📄 License

MIT License
