#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{

    while (true) {  
        
        string text;  

        cout << "SQL_parser> ";
        getline(cin, text);

        if (text == "exit") {
            break;
        } else if (text == "clear") {
            //si está en linux o si está en windows
            #ifdef __linux__
                system("clear");
            #else
                system("cls");
            #endif
            continue;
        } else {
            Scanner* scanner = new Scanner(text);
            SQLParser* parser = new SQLParser(scanner);

            parser->machine();
        }
    }
    
    return 0;
}

