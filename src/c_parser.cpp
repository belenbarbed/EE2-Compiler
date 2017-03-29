#include "ast.hpp"

#include <iostream>

using namespace std;

extern const ASTnode *parseAST();

int main() {

    const ASTnode *ast=parseAST();

    cout << endl;
    cout << "<?xml version=\"1.0\"?>" << endl;
    cout << "<Program>" << endl;

    ast->print();

    cout << "</Program>" << endl;
    cout << endl;

    return 0;
}
