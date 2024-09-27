#include <iostream>
#include <vector>
using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    vector<string> v = {"hola", "mundo"};
    for (string s : v) {
        cout << s << endl;
    }
    return 0;
}