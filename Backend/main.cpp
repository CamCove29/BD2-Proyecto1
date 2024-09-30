#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        cout << "Incorrect number of arguments" << endl;
        exit(1);
    }
    
    string filename = argv[1];
    
    // HANDLE FILE
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    string line;
    string text;
    while (getline(file, line)) {
        text += line + "\n";
    }

    file.close();

    /* SCANNER Y PARSER*/
    Scanner* scanner = new Scanner(text);

    SQLParser* parser = new SQLParser(scanner);

    parser->parse();

    
    return 0;
}

