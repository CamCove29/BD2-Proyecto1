#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include "parser.h"
#include "parser.h"
#include "record.h"
#include "sequentialfile.h"
// #include "extendiblehash.h"
// #include "sparseindex.h"
using namespace std;

const char* Token::token_names = {
    "SELECT", "CREATE", "INSERT", "DELETE", "INTO", "FROM", "WHERE", "USING", "BETWEEN", "AND", "FILE", "TABLE", "VALUES", "LEFTPARENT", "RIGHTPARENT", "NUMBER", "STRING", "COMMA", "EQUAL", "INDEX", "ID", "EOF", "SPACE", "EOL", "NULLO", "ERR"
};

Token::Token(Type t) : type(t) {lexema = "";}

Token::Token(Type t, const string source) : type(t), lexema(source) {}

std::string operator<<(std::ostream& outs, const Token& tok) {
    if (tok.lexema == "") {
        outs << Token::token_names[tok.type];
    } else {
        outs << Token::token_names[tok.type] << " " << tok.lexema;
    }
}


Scanner::Scanner(string s):input(s), first(0), current(0){
    reserved["SELECT"] = Token::SELECT;
    reserved["CREATE"] = Token::CREATE;
    reserved["INSERT"] = Token::INSERT;
    reserved["DELETE"] = Token::DELETE;
    reserved["INTO"] = Token::INTO;
    reserved["FROM"] = Token::FROM;
    reserved["WHERE"] = Token::WHERE;
    reserved["USING"] = Token::USING;
    reserved["BETWEEN"] = Token::BETWEEN;
    reserved["AND"] = Token::AND;
    reserved["FILE"] = Token::FILE;
    reserved["TABLE"] = Token::TABLE;
    reserved["VALUES"] = Token::VALUES;
    reserved["INDEX"] = Token::INDEX;
    reserved["SPACE"] = Token::SPACE;
    reserved["NULLO"] = Token::NULLO;
}

// metodos privados
char Scanner::nextChar(){
    int c = input[current]
    current++;
    return c;
    }

void Scanner::rollBack(){
    current--;
}

voi Scanner::startLexema(){
    first = current-1;
}

string Scanner::getLexema(){
    return input.substr(first, current-first);
}

Token::Type Scanner::checkReserved(string lexema){
    if (reserved.find(lexema) != reserved.end()){
        return reserved[lexema];
    }
    return Token::ERR;
}

Token* scanner::nextToken(){
    Token* token;
    char c;
    state = 0;
    startLexema();
    while(True){
        switch(state){
            case 0: c = nextChar();
                if (c = '\0') return new Token(Token::EOF);
                else if (c == ' ' || c== '\t') state = 1;
                else if (c == '\n') return new Token(Token::EOL);
                else if (c == '(') return new Token(Token::LEFTPARENT);
                else if (c == ')') return new Token(Token::RIGHTPARENT);
                else if (c == ',') return new Token(Token::COMMA);
                else if (c == '=') return new Token(Token::EQUAL);
                else if (c == '"') {startLexema(); state = 2;}
                else if (isalpha(c)) state = 3;
                else if (isdigit(c)) state = 4;
                else return new Token(Token::ERR);
                break;
            case 1: c = nextChar();
                if (c == ' ' || c == '\t') state = 1;
                else{
                    rollBack();
                    return new Token(Token::SPACE);
                }
                break;
            case 2: c = nextChar();
                if (c == '"') {
                    return new Token(Token::STRING, input.substr(first, current-first-1));
                }
                else{
                    state = 2;
                }
                break;
            case 3: c = nextChar();
                if (isalpha(c) || isdigit(c)) state = 3;
                else if (c == '_')
                    state = 5;
                }else {
                    state = 9;
                }
                break;
            case 4: c = nextChar();
                if (isdigit(c)) state = 4;
                else if (c == '.'){
                    state = 7;
                else{
                    state = 10;
                }
                break;
            case 5: c = nextChar();
                if (isalpha(c) || isdigit(c)) state = 6;
                else{
                    return new Token(Token::ERR);
                }
                break;  
            case 6: c = nextChar();
                if (isalpha(c) || isdigit(c)) state = 6;
                else{
                    state = 9;
                }
                break;
            case 7: c = nextChar();
                if (isdigit(c)) state = 8;
                else{
                    return new Token(Token::ERR);
                }
                break;
            case 8: c = nextChar();
                if (isdigit(c)) state = 8;
                else{
                    state = 10;
                }
                break;
            case 9:
                rollBack();
                string lexema = getLexema();
                Token::Type type = checkReserved(lexema);
                if (type != Token::ERR){
                    return new Token(type);
                }else{
                    return new Token(Token::ID, lexema);
                }
                break;
            case 10:
                rollBack();
                return new Token(Token::NUMBER, getLexema());
                break;
            default: return new Token(Token::ERR);
        }
    }
}

/*

    reserved["LEFTPARENT"] = Token::LEFTPARENT;
    reserved["RIGHTPARENT"] = Token::RIGHTPARENT;
    reserved["NUMBER"] = Token::NUMBER;
    reserved["COMMA"] = Token::COMMA;
    reserved["STRING"] = Token::STRING;
    reserved["EQUAL"] = Token::EQUAL;
    reserved["ID"] = Token::ID;
    reserved["EOF"] = Token::EOF;
    reserved["EOL"] = Token::EOL;
    reserved["NULL"] = Token::NULLO;

*/

//----------------------------- parser ------------------------------

bool SQLParser::match(Token::Type ttype){
    if (check(ttype)){
        advance();
        return true;
    }
    return false;
}

bool SQLParser::check(Token::Type ttype){
    return current->type == ttype;
}

bool SQLParser::isAtEnd(){
    return current->type == Token::EOF;
}

bool SQLParser::advance(){
    if(!isAtEnd()){
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if(check(Token::ERR)){
            cout << "Error de sintaxis, not recognized: " << current->lexema << endl;
            exit(0);
        }
        return true;
    }
    return false;
}

vector<string> SQLParser::parse(){
    current = scanner->nextToken();
    if(check(Token::ERR)){
        cout << "Error de sintaxis" << endl;
        exit(0);
    }
    vector<string> results = parseStmList();
    if (current->type != Token::EOF) {
    cout << "Esperaba fin-de-input, se encontro " << current << endl;
    }
}

vector<string> SQLParser::parseStmList(){
    vector<string> results;
    while(!isAtEnd()){
        results.push_back(parseStm());
    }
    return results;
}

string SQLParser::parseStm(){
    if (check(Token::CREATE)){
        return parseCreateStm();
    }else if (check(Token::SELECT)){
        return parseSelectStm();
    }else if (check(Token::INSERT)){
        return parseInsertStm();
    }else if (check(Token::DELETE)){
        return parseDeleteStm();
    }else{
        cout << "Error de sintaxis: se esperaba CREATE, SELECT, INSERT o DELETE" << endl;
        exit(0);
    }
}

string SQLParser::parseCreateStm(){
    string result;
    string table_name, file_name, index_type, key;
    if(!match(Token::CREATE)){
        cout << "Error de sintaxis: se esperaba CREATE"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::TABLE)){
        cout << "Error de sintaxis: se esperaba TABLE"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::ID)){
        cout << "Error de sintaxis: se esperaba el nombre de la tabla"<< endl;
        exit(0);
    }
    table_name = previous->lexema;
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::FROM)){
        cout << "Error de sintaxis: se esperaba FROM"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::STRING)){
        cout << "Error de sintaxis: se esperaba el nombre del archivo"<< endl;
        exit(0);
    }
    file_name = previous->lexema;
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::USING)){
        cout << "Error de sintaxis: se esperaba USING"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::INDEX)){
        cout << "Error de sintaxis: se esperaba INDEX"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::ID)){
        cout << "Error de sintaxis: se esperaba el nombre del indice"<< endl;
        exit(0);
    }
    index_type = previous->lexema;
    if(!match(Token::LEFTPARENT)){
        cout << "Error de sintaxis: se esperaba LEFTPARENT"<< endl;
        exit(0);
    }
    if(!match(Token::STRING)){
        cout << "Error de sintaxis: se esperaba el nombre del archivo"<< endl;
        exit(0);
    }
    key = previous->lexema;
    if(!match(Token::RIGHTPARENT)){
        cout << "Error de sintaxis: se esperaba RIGHTPARENT"<< endl;
        exit(0);
    }

    //logica
    if (index_type == "sequential"){
        if (file_name == "meets.csv"){
            SequentileFile<RecordMeet, int> meetFile(file_name + "table", [](RecordMeet* record) { return record->getkey(key); });
            meetFile.init(file_name);
        }else if (file_name == "openpowerlifting.csv") {
            SequentileFile<RecordCompetition, int> competitionFile(file_name + "table", [](RecordCompetition* record) { return record->getkey(key); });
            competitionFile.init(file_name);
        } else {
            cout << "trki"
        }

    }else if (index_type == "extendible"){

    }else if (index_type == "sparse"){
    
    }
    
}

string SQLParser::parseSelectStm(){
    string result;
    if(!match(Token::SELECT)){
        cout << "Error de sintaxis: se esperaba SELECT"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::ID)){
        cout << "Error de sintaxis: se esperaba el nombre de la tabla"<< endl;
        exit(0);
    }
    result = previous->lexema;
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::FROM)){
        cout << "Error de sintaxis: se esperaba FROM"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::ID)){
        cout << "Error de sintaxis: se esperaba el nombre de la tabla"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::WHERE)){
        cout << "Error de sintaxis: se esperaba WHERE"<< endl;
        exit(0);
    }
    if(!match(Token::SPACE)){
        cout << "Error de sintaxis: se esperaba SPACE"<< endl;
        exit(0);
    }
    if(!match(Token::ID)){
        cout << "Error de sintaxis: se esperaba el nombre de la columna"<< endl;
        exit(0);
    }
    if(!match(Token::EQUAL)){
        cout << "Error de sintaxis: se esperaba EQUAL"<< endl;
        exit(0);
    }
    if(!match(Token::NUMBER)){
        cout << "Error de sintaxis: se esperaba el valor"<< endl;
        exit(0);
    }
    return result;
}




















int main(){
    
    SQLParser* parser = new Parser(new Scanner("SELECT * FROM table WHERE id = 1"));
    vector<ResultType*> = parser->parse();

    for (ResultType* result : results) {
        cout << result->value << endl;
    }
}