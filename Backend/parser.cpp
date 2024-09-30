#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include "record.h"
#include "extendible_hashing.h"
#include "sequentialfile.h"
#include "sparseIndex.h"
#include "parser.h"
#include <cctype>

#define PROCESS_INDEX_EQUAL_SELECT(recordType)                                                                              \
    IndexType<recordType> *index = nullptr;                                                                                 \
    if (md->index_type == "sequential")                                                                                     \
    {                                                                                                                       \
        index = new SequentileFile<recordType>(table_name + "data_file", key_name);                                         \
    }                                                                                                                       \
    else if (md->index_type == "sparse")                                                                                    \
    {                                                                                                                       \
        index = new SparseIndex<recordType>(table_name + "index_file", table_name + "data_file", key_name);                 \
    }                                                                                                                       \
    else if (md->index_type == "extendible")                                                                                \
    {                                                                                                                       \
        index = new ExtendibleHash<recordType>(table_name + "data_file", table_name + "index_file", key_name);              \
    }                                                                                                                       \
    if (index == nullptr)                                                                                                   \
    {                                                                                                                       \
        std::cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << std::endl; \
        return "";                                                                                                          \
    }                                                                                                                       \
    recordType *record = index->search(x);                                                                                  \
    if (record != nullptr)                                                                                                  \
    {                                                                                                                       \
        for (const std::string &element : lista_atributos)                                                                  \
        {                                                                                                                   \
            result += record->getStringAttribute(element).second + ",";                                                     \
        }                                                                                                                   \
        result.pop_back();                                                                                                  \
    }                                                                                                                       \
    result += "\n";

#define PROCESS_INDEX_RANGE_SEARCH(recordType)                                                                              \
    IndexType<recordType> *index = nullptr;                                                                                 \
    if (md->index_type == "sequential")                                                                                     \
    {                                                                                                                       \
        index = new SequentileFile<recordType>(table_name + "data_file", key_name);                                         \
    }                                                                                                                       \
    else if (md->index_type == "sparse")                                                                                    \
    {                                                                                                                       \
        index = new SparseIndex<recordType>(table_name + "index_file", table_name + "data_file", key_name);                 \
    }                                                                                                                       \
    else if (md->index_type == "extendible")                                                                                \
    {                                                                                                                       \
        index = new ExtendibleHash<recordType>(table_name + "data_file", table_name + "index_file", key_name);              \
    }                                                                                                                       \
    if (index == nullptr)                                                                                                   \
    {                                                                                                                       \
        std::cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << std::endl; \
        return "";                                                                                                          \
    }                                                                                                                       \
    std::vector<recordType *> records = index->range_search(x, y);                                                          \
    for (recordType * record : records)                                                                                     \
    {                                                                                                                       \
        for (const std::string &element : lista_atributos)                                                                  \
        {                                                                                                                   \
            result += record->getStringAttribute(element).second + ",";                                                     \
        }                                                                                                                   \
        result.pop_back();                                                                                                  \
        result += "\n";                                                                                                     \
    }

// #include "extendiblehash.h"

// #include "sparseindex.h"
using namespace std;

const char *Token::token_names[26] = {
    "SELECT", "CREATE", "INSERT", "DELETE", "INTO", "FROM", "WHERE", "USING", "BETWEEN", "AND", "FILE", "TABLE", "VALUES", "LEFTPARENT", "RIGHTPARENT", "NUMBER", "STRING", "COMMA", "EQUAL", "INDEX", "ID", "END", "SPACE", "EOL", "NULLO", "ERR"};

Token::Token(Type t) : type(t) { lexema = ""; }

Token::Token(Type t, const string source) : type(t), lexema(source) {}

Scanner::Scanner(string s) : input(s), first(0), current(0)
{
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

Scanner::~Scanner()
{
    // delete input;
}

// metodos privados
char Scanner::nextChar()
{
    int c = input[current];
    current++;
    return c;
}

void Scanner::rollBack()
{
    current--;
}

void Scanner::startLexema()
{
    first = current;
}

string Scanner::getLexema()
{
    return input.substr(first, current - first);
}

Token::Type Scanner::checkReserved(string lexema)
{
    if (reserved.find(lexema) != reserved.end())
    {
        return reserved[lexema];
    }
    return Token::ERR;
}

Token *Scanner::nextToken()
{
    Token *token;
    char c;
    state = 0;
    startLexema();

    while (true)
    {
        switch (state)
        {
        case 0:
        {
            c = nextChar();
            if (c == '\0')
                return new Token(Token::END);
            else if (c == ' ' || c == '\t')
                state = 1;
            else if (c == '\n')
                return new Token(Token::EOL);
            else if (c == '(')
                return new Token(Token::LEFTPARENT);
            else if (c == ')')
                return new Token(Token::RIGHTPARENT);
            else if (c == ',')
                return new Token(Token::COMMA);
            else if (c == '=')
                return new Token(Token::EQUAL);
            else if (c == '"')
            {
                startLexema();
                state = 2;
            }
            else if (isalpha(c))
                state = 3;
            else if (isdigit(c))
                state = 4;
            else
                return new Token(Token::ERR);
            break;
        }
        case 1:
        {
            c = nextChar();
            if (c == ' ' || c == '\t')
                state = 1;
            else
            {
                rollBack();
                return new Token(Token::SPACE);
            }
            break;
        }
        case 2:
        {
            c = nextChar();
            if (c == '"')
            {
                return new Token(Token::STRING, input.substr(first, current - first - 1));
            }
            else
            {
                state = 2;
            }
            break;
        }
        case 3:
        {
            c = nextChar();
            if (isalpha(c) || isdigit(c))
                state = 3;
            else if (c == '_')
                state = 5;
            else
            {
                state = 9;
            }
            break;
        }
        case 4:
        {
            c = nextChar();
            if (isdigit(c))
                state = 4;
            else if (c == '.')
                state = 7;
            else
            {
                state = 10;
            }
            break;
        }
        case 5:
        {
            c = nextChar();
            if (isalpha(c) || isdigit(c))
                state = 6;
            else
            {
                return new Token(Token::ERR);
            }
            break;
        }
        case 6:
        {
            c = nextChar();
            if (isalpha(c) || isdigit(c))
                state = 6;
            else
            {
                state = 9;
            }
            break;
        }
        case 7:
        {
            c = nextChar();
            if (isdigit(c))
                state = 8;
            else
            {
                return new Token(Token::ERR);
            }
            break;
        }
        case 8:
        {
            c = nextChar();
            if (isdigit(c))
                state = 8;
            else
            {
                state = 10;
            }
            break;
        }
        case 9:
        {
            rollBack();
            string lexema = getLexema();
            Token::Type type = checkReserved(lexema);
            if (type != Token::ERR)
            {
                return new Token(type);
            }
            else
            {
                return new Token(Token::ID, lexema);
            }
            break;
        }
        case 10:
        {
            rollBack();
            return new Token(Token::NUMBER, getLexema());
            break;
        }
        default:
        {
            return new Token(Token::ERR);
        }
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
    reserved["END"] = Token::END;
    reserved["EOL"] = Token::EOL;
    reserved["NULL"] = Token::NULLO;

*/

//----------------------------- parser ------------------------------

// constructor

SQLParser::SQLParser(Scanner *s) : scanner(s), current(nullptr), previous(nullptr)
{
    metadata_file = "metadata.csv";
}

void SQLParser::save_to_metadata(string table_name, string record_type, string index_type, string key_name)
{
    // table_name,record_type,index_type,key_name
    ofstream metadata;
    metadata.open(metadata_file, ios::app);
    std::cout << "Guardando en metadata" << std::endl;
    metadata.write((table_name + "," + record_type + "," + index_type + "," + key_name + "\n").c_str(), (table_name + "," + record_type + "," + index_type + "," + key_name + "\n").size());
    metadata.close();
}

MetaData *SQLParser::get_metadata(string table_name)
{
    ifstream metadata;
    metadata.open(metadata_file);
    string line;
    while (getline(metadata, line))
    {
        vector<string> tokens;
        stringstream check1(line);
        string intermediate;
        while (getline(check1, intermediate, ','))
        {
            tokens.push_back(intermediate);
        }
        if (tokens[0] == table_name)
        {
            MetaData *md = new MetaData();
            md->table_name = tokens[0];
            md->record_type = tokens[1];
            md->index_type = tokens[2];
            md->key_name = tokens[3];
            return md;
        }
    }
    metadata.close();

    return nullptr;
}

bool SQLParser::match(Token::Type ttype)
{
    if (check(ttype))
    {
        advance();
        return true;
    }
    return false;
}

bool SQLParser::check(Token::Type ttype)
{
    return current->type == ttype;
}

bool SQLParser::isAtEnd()
{
    return current->type == Token::END;
}

bool SQLParser::advance()
{
    if (!isAtEnd())
    {
        Token *temp = current;
        if (previous)
            delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR))
        {
            cout << "Error de sintaxis, not recognized: " << current->lexema << endl;
            return "";
        }
        return true;
    }
    return false;
}

vector<string> SQLParser::parse()
{
    current = scanner->nextToken();
    if (check(Token::ERR))
    {
        cout << "Error de sintaxis" << endl;
        return {};
    }
    vector<string> results = parseStmList();
    if (current->type != Token::END)
    {
        cout << "Esperaba fin-de-input, se encontro " << current << endl;
        return results;
    }

    return results;
}

vector<string> SQLParser::machine()
{
    current = scanner->nextToken();
    if (check(Token::ERR))
    {
        cout << "Error de sintaxis" << endl;
        return {};
    }
    vector<string> results = parseStmList();
    if (current->type != Token::END)
    {
        return results;
    }

    return results;
}

vector<string> SQLParser::parseStmList()
{
    vector<string> results;
    while (!isAtEnd())
    {
        results.push_back(parseStm());
        std::cout << results.back() << std::endl;
        if (!match(Token::EOL))
        {
            return results;
        }
    }
    return results;
}

string SQLParser::parseStm()
{
    if (check(Token::CREATE))
    {
        return parseCreateStm();
    }
    else if (check(Token::SELECT))
    {
        return parseSelectStm();
    }
    else if (check(Token::INSERT))
    {
        return parseInsertStm();
    }
    else if (check(Token::DELETE))
    {
        return parseDeleteStm();
    }
    else
    {
        cout << "Error de sintaxis: se esperaba CREATE, SELECT, INSERT o DELETE" << endl;
        return "";
    }
}

string SQLParser::parseCreateStm()
{
    string result;
    string table_name, file_name, index_type, key;
    if (!match(Token::CREATE))
    {
        cout << "Error de sintaxis: se esperaba CREATE" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::TABLE))
    {
        cout << "Error de sintaxis: se esperaba TABLE" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba el nombre de la tabla" << endl;
        return "";
    }
    table_name = previous->lexema;
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::FROM))
    {
        cout << "Error de sintaxis: se esperaba FROM" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::FILE))
    {
        cout << "Error de sintaxis: se esperaba FILE" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::STRING))
    {
        cout << "Error de sintaxis: se esperaba el nombre del archivo" << endl;
        return "";
    }
    file_name = previous->lexema;
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::USING))
    {
        cout << "Error de sintaxis: se esperaba USING" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::INDEX))
    {
        cout << "Error de sintaxis: se esperaba INDEX" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba el nombre del indice" << endl;
        return "";
    }
    index_type = previous->lexema;
    if (!match(Token::LEFTPARENT))
    {
        cout << "Error de sintaxis: se esperaba LEFTPARENT" << endl;
        return "";
    }
    if (!match(Token::STRING))
    {
        cout << "Error de sintaxis: se esperaba el nombre del archivo" << endl;
        return "";
    }
    key = previous->lexema;
    if (!match(Token::RIGHTPARENT))
    {
        cout << "Error de sintaxis: se esperaba RIGHTPARENT" << endl;
        return "";
    }

    IndexType<RecordMeet> *indexA;
    IndexType<RecordCompetition> *indexB;

    std::string metadata_line = ""; // record_type,index_type,table_name,key_name

    // logica

    // verificar que el nombre no exista
    MetaData *md = get_metadata(table_name);
    if (md != nullptr)
    {
        cout << "Error: tabla ya existe" << endl;
        return "";
    }

    if (index_type == "sequential")
    {
        if (file_name == "meets.csv" || file_name == "test_data.csv")
        {
            indexA = new SequentileFile<RecordMeet>(table_name + "data_file", key);
            indexA->init(file_name);

            save_to_metadata(table_name, "RecordMeet", index_type, key);
        }
        else if (file_name == "competitions.csv")
        {
            indexB = new SequentileFile<RecordCompetition>(table_name + "data_file", key);
            indexB->init(file_name);

            save_to_metadata(table_name, "RecordCompetition", index_type, key);
        }
    }
    else if (index_type == "sparse")
    {
        if (file_name == "meets.csv" || file_name == "test_data.csv")
        {
            indexA = new SparseIndex<RecordMeet>(table_name + "index_file", table_name + "data_file", key);
            indexA->init(file_name);

            save_to_metadata(table_name, "RecordMeet", index_type, key);
        }
        else if (file_name == "competitions.csv")
        {
            indexB = new SparseIndex<RecordCompetition>(table_name + "index_file", table_name + "data_file", key);
            indexB->init(file_name);

            save_to_metadata(table_name, "RecordCompetition", index_type, key);
        }
    }
    else if (index_type == "extendible")
    {
        if (file_name == "meets.csv" || file_name == "test_data.csv")
        {
            indexA = new ExtendibleHash<RecordMeet>(table_name + "data_file", table_name + "index_file", key);
            indexA->init(file_name);

            save_to_metadata(table_name, "RecordMeet", index_type, key);
        }
        else if (file_name == "competitions.csv")
        {
            indexB = new ExtendibleHash<RecordCompetition>(table_name + "data_file", table_name + "index_file", key);
            indexB->init(file_name);

            save_to_metadata(table_name, "RecordCompetition", index_type, key);
        }
    }
    else
    {
        cout << "Error de sintaxis: se esperaba sequential, extendible o sparse" << endl;
        return "";
    }

    return result = "Tabla creada con exito";
}

string SQLParser::parseSelectStm()
{
    std::vector<std::string> lista_atributos;
    std::string table_name, key_name, x, y;

    //<select-stm> 	:= <select><_><attr_list><_><from><_><id><_><where><_><id>(<equal-expr> | (<_> (<equal-expr> | <between-expr>)))

    if (!match(Token::SELECT))
    {
        cout << "Error de sintaxis: se esperaba SELECT" << endl;
        return "";
    }

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE luego de SELECT" << endl;
        return "";
    }

    lista_atributos = parseAttrList();

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE luego de la lista de atributos" << endl;
        return "";
    }

    if (!match(Token::FROM))
    {
        cout << "Error de sintaxis: se esperaba FROM" << endl;
        return "";
    }

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE luego de FROM" << endl;
        return "";
    }

    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return "";
    }

    table_name = previous->lexema;

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE luego del nombre de la tabla" << endl;
        return "";
    }

    if (!match(Token::WHERE))
    {
        cout << "Error de sintaxis: se esperaba WHERE" << endl;
        return "";
    }

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE luego de WHERE" << endl;
        return "";
    }

    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return "";
    }

    key_name = previous->lexema;

    std::string type = "none";

    if (match(Token::SPACE))
    {
        //<_> (<equal-expr> | <between-expr>)
        if (check(Token::EQUAL))
        {
            //<equal-expr>
            type = "EQUAL";
            x = parseEqualExpr();
        }
        else if (check(Token::BETWEEN))
        {
            //<between-expr>
            type = "BETWEEN";
            std::pair<std::string, std::string> p = parseBetweenExpr();
            x = p.first;
            y = p.second;
        }
        else
        {
            cout << "Error de sintaxis: se esperaba EQUAL o BETWEEN" << endl;
            return "";
        }
    }
    else
    {
        //<equal-expr>
        if (check(Token::EQUAL))
        {
            //<equal-expr>
            type = "EQUAL";
            x = parseEqualExpr();
        }
        else
        {
            cout << "Error de sintaxis: se esperaba EQUAL" << endl;
            return "";
        }
    }

    // logica
    std::string result = "";

    // headers
    for (std::string element : lista_atributos)
    {
        result += element + ",";
    }
    result.pop_back();
    result += "\n";

    MetaData *md = get_metadata(table_name);

    if (md == nullptr)
    {
        cout << "Error de sintaxis: tabla no encontrada" << endl;
        return "";
    }

    if (type == "EQUAL")
    {
        if (md->record_type == "RecordMeet")
        {
            PROCESS_INDEX_EQUAL_SELECT(RecordMeet)
        }
        else if (md->record_type == "RecordCompetition")
        {
            PROCESS_INDEX_EQUAL_SELECT(RecordCompetition)
        }
        else
        {
            cout << "Error de sintaxis: tipo de record desconocido" << endl;
            return "";
        }
    }
    else if (type == "BETWEEN")
    {
        if (md->record_type == "RecordMeet")
        {
            PROCESS_INDEX_RANGE_SEARCH(RecordMeet)
        }
        else if (md->record_type == "RecordCompetition")
        {
            PROCESS_INDEX_RANGE_SEARCH(RecordCompetition)
        }
        else
        {
            cout << "Error de sintaxis: tipo de record desconocido" << endl;
            return "";
        }
    }
    else
    {
        cout << "Error de sintaxis: operación SELECT desconocida" << endl;
        return "";
    }

    return result;
}

string SQLParser::parseDeleteStm()
{
    //<delete-stm>    := <delete><_><from><_><id><_><where><_><id><_>?<equal-expr>
    string table_name, key_name, x;
    if (!match(Token::DELETE))
    {
        cout << "Error de sintaxis: se esperaba DELETE" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::FROM))
    {
        cout << "Error de sintaxis: se esperaba FROM" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return "";
    }
    table_name = previous->lexema;

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::WHERE))
    {
        cout << "Error de sintaxis: se esperaba WHERE" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return "";
    }
    key_name = previous->lexema;

    match(Token::SPACE);

    x = parseEqualExpr();

    // logica

    MetaData *md = get_metadata(table_name);
    if (md == nullptr)
    {
        cout << "Error de sintaxis: tabla no encontrada" << endl;
        return "";
    }

    if (md->record_type == "RecordMeet")
    {
        IndexType<RecordMeet> *index = nullptr;
        if (md->index_type == "sequential")
        {
            index = new SequentileFile<RecordMeet>(table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "sparse")
        {
            index = new SparseIndex<RecordMeet>(table_name + "index_file", table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "extendible")
        {
            index = new ExtendibleHash<RecordMeet>(table_name + "data_file", table_name + "index_file", md->key_name);
        }
        else
        {
            cout << "Error de sintaxis: tipo de indice desconocido" << endl;
            return "";
        }

        if (index == nullptr)
        {
            cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << endl;
            return "";
        }

        index->remove(x);
    }
    else if (md->record_type == "RecordCompetition")
    {
        IndexType<RecordCompetition> *index = nullptr;
        if (md->index_type == "sequential")
        {
            index = new SequentileFile<RecordCompetition>(table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "sparse")
        {
            index = new SparseIndex<RecordCompetition>(table_name + "index_file", table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "extendible")
        {
            index = new ExtendibleHash<RecordCompetition>(table_name + "data_file", table_name + "index_file", md->key_name);
        }
        else
        {
            cout << "Error de sintaxis: tipo de indice desconocido" << endl;
            return "";
        }

        if (index == nullptr)
        {
            cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << endl;
            return "";
        }

        index->remove(x);
    }
    else
    {
        cout << "Error de sintaxis: tipo de record desconocido" << endl;
        return "";
    }

    return "DELETED 1 RECORD WITH KEY " + x;
}

string SQLParser::parseInsertStm()
{
    //<insert-stm> 	:= <insert><_><into><_><id><_><values><(><value_list><)>
    string table_name;
    if (!match(Token::INSERT))
    {
        cout << "Error de sintaxis: se esperaba INSERT" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::INTO))
    {
        cout << "Error de sintaxis: se esperaba INTO" << endl;
        return "";
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return "";
    }
    table_name = previous->lexema;

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return "";
    }

    if (!match(Token::VALUES))
    {
        cout << "Error de sintaxis: se esperaba VALUES" << endl;
        return "";
    }

    if (!match(Token::LEFTPARENT))
    {
        cout << "Error de sintaxis: se esperaba LEFTPARENT" << endl;
        return "";
    }

    std::vector<std::string> lista_valores = parseValueList();

    if (!match(Token::RIGHTPARENT))
    {
        cout << "Error de sintaxis: se esperaba RIGHTPARENT" << endl;
        return "";
    }

    // logica

    // leer metadata
    MetaData *md = get_metadata(table_name);
    if (md == nullptr)
    {
        cout << "Error de sintaxis: tabla no encontrada" << endl;
        return "";
    }

    if (md->record_type == "RecordMeet")
    {
        IndexType<RecordMeet> *index = nullptr;
        if (md->index_type == "sequential")
        {
            index = new SequentileFile<RecordMeet>(table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "sparse")
        {
            index = new SparseIndex<RecordMeet>(table_name + "index_file", table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "extendible")
        {
            index = new ExtendibleHash<RecordMeet>(table_name + "data_file", table_name + "index_file", md->key_name);
        }
        else
        {
            cout << "Error de sintaxis: tipo de indice desconocido" << endl;
            return "";
        }

        if (index == nullptr)
        {
            cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << endl;
        }

        // el vector es correcto?
        if (RecordMeet::checkVector(lista_valores) == false)
        {
            cout << "Error de sintaxis: valores incorrectos, revisar la definición de la tabla" << endl;
            return "";
        }

        RecordMeet *record = new RecordMeet(lista_valores);
        index->add(record);
    }
    else if (md->record_type == "RecordCompetition")
    {
        IndexType<RecordCompetition> *index = nullptr;
        if (md->index_type == "sequential")
        {
            index = new SequentileFile<RecordCompetition>(table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "sparse")
        {
            index = new SparseIndex<RecordCompetition>(table_name + "index_file", table_name + "data_file", md->key_name);
        }
        else if (md->index_type == "extendible")
        {
            index = new ExtendibleHash<RecordCompetition>(table_name + "data_file", table_name + "index_file", md->key_name);
        }
        else
        {
            cout << "Error de sintaxis: tipo de indice desconocido" << endl;
            return "";
        }

        if (index == nullptr)
        {
            cout << "Error de sintaxis: indice no valido, pruebe con 'sequential' o 'sparse' o 'extendible'" << endl;
            return "";
        }

        RecordCompetition *record = new RecordCompetition(lista_valores);
        index->add(record);
    }
    else
    {
        cout << "Error de sintaxis: tipo de record desconocido" << endl;
        return "";
    }

    return "";
}

string SQLParser::parseEqualExpr()
{

    if (!match(Token::EQUAL))
    {
        cout << "Error de sintaxis: se esperaba EQUAL" << endl;
        return "";
    }

    match(Token::SPACE);

    if (!match(Token::STRING) && !match(Token::NUMBER))
    {
        cout << "Error de sintaxis: se esperaba un valor" << endl;
        return "";
    }
    return previous->lexema;
}

pair<string, string> SQLParser::parseBetweenExpr()
{
    string key1, key2;
    if (!match(Token::BETWEEN))
    {
        cout << "Error de sintaxis: se esperaba BETWEEN" << endl;
        return {};
    }
    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return {};
    }
    if (!match(Token::STRING) && !match(Token::NUMBER))
    {
        cout << "Error de sintaxis: se esperaba un valor" << endl;
        return {};
    }
    key1 = previous->lexema;

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return {};
    }

    if (!match(Token::AND))
    {
        cout << "Error de sintaxis: se esperaba AND" << endl;
        return {};
    }

    if (!match(Token::SPACE))
    {
        cout << "Error de sintaxis: se esperaba SPACE" << endl;
        return {};
    }

    if (!match(Token::STRING) && !match(Token::NUMBER))
    {
        cout << "Error de sintaxis: se esperaba un valor" << endl;
        return {};
    }

    key2 = previous->lexema;

    return make_pair(key1, key2);
}

vector<string> SQLParser::parseAttrList()
{
    std::vector<std::string> lista_atributos;
    if (!match(Token::ID))
    {
        cout << "Error de sintaxis: se esperaba ID" << endl;
        return {};
    }
    lista_atributos.push_back(previous->lexema);

    while (match(Token::COMMA))
    {
        if (!match(Token::ID))
        {
            cout << "Error de sintaxis: se esperaba ID" << endl;
            return {};
        }
        lista_atributos.push_back(previous->lexema);
    }
    return lista_atributos;
}

vector<string> SQLParser::parseValueList()
{
    std::vector<std::string> lista_valores;
    if (!match(Token::STRING) && !match(Token::NUMBER))
    {
        cout << "Error de sintaxis: se esperaba un valor" << endl;
        return {};
    }
    lista_valores.push_back(previous->lexema);

    while (match(Token::COMMA))
    {
        match(Token::SPACE);
        if (!match(Token::STRING) && !match(Token::NUMBER))
        {
            cout << "Error de sintaxis: se esperaba un valor" << endl;
            return {};
        }
        lista_valores.push_back(previous->lexema);
    }
    return lista_valores;
}
