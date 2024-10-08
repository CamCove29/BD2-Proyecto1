#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;
// tokens = {SELECT, CREATE, INSERT, DELETE, INTO, FROM, WHERE, USING, BETWEEN, AND, FILE=11, TABLE, VALUES, LEFTPARENT, RIGHTPARENT, NUMBER=16, STRING, COMMA, EQUAL, INDEX, ID, EOF, _, EOL, NULL};

class Token {
public:
    enum Type {
    SELECT=0, CREATE, INSERT, DELETE, INTO, FROM, WHERE, USING, BETWEEN, AND, FILE, TABLE, VALUES, LEFTPARENT, RIGHTPARENT, NUMBER, STRING, COMMA, EQUAL, INDEX, ID, END, SPACE, EOL, NULLO, ERR
    };
    static const char* token_names[26];
    Type type;
    string lexema;
    Token(Type);
    Token(Type, const string source);

    //sobrecarga del cout
    friend ostream& operator<<(ostream& os, const Token& token) {
        if (token.lexema == "") {
            os << token.token_names[token.type];
        } else {
            os << token.token_names[token.type] << " " << token.lexema;
        }
        return os;
    }
};

class Scanner {
public:
    Scanner(string in_s);
    Token* nextToken();
    ~Scanner();
private:
    string input;
    int state;
    int first, current;
    unordered_map<string, Token::Type> reserved;
    char nextChar();
    void rollBack();
    void startLexema(); 
    string getLexema();
    Token::Type checkReserved(string);
};

struct MetaData {
    string table_name;
    string record_type;
    string index_type;
    string key_name;
};

class SQLParser {
private:
    std::string metadata_file = "metadata.csv";
    void save_to_metadata(string table_name,string record_type,string index_type,string key_name);
    MetaData* get_metadata(string table_name);

    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    vector<string> parseStmList();
    string parseStm();
    string parseCreateStm();
    string parseSelectStm();
    string parseInsertStm();
    string parseDeleteStm();

    //mas
    string parseEqualExpr(); //key --
    pair<string, string> parseBetweenExpr(); //key, key --
    std::vector<std::string> parseAttrList(); //id --
    std::vector<std::string> parseValueList(); //value --

public:
    SQLParser(Scanner* scanner);
    vector<string> parse();
    vector<string> machine();
};

/*
<select-stm> 	:= <select><_><attr_list><_><from><_><id><_><where><_><id>(<equal-expr> | (<_> (<equal-expr> | <between-expr>)))
<create-stm> 	:= <create><_><table><_><id><_><from><_><string><_><using><_><index><_><id><(><string><)>
<insert-stm> 	:= <insert><_><into><_><id><_><values><(><value_list><)>
<delete-stm>    := <delete><_><from><_><id><_><where><_><id><_>?<equal-expr>
<equal-expr>    := <=><_>?<value> 
<between-expr>  := <between><_><value><_><and><_><value>
*/



