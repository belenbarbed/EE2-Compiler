#include "ast.hpp"
#include "context.hpp"

#include <iostream>

using namespace std;

extern const ASTnode *parseAST();

int main() {

    const ASTnode *ast=parseAST();
    
    Context ctxt = new Context();

	cout << endl;
	/* CHECK THE AST FORMATION
	cout << "now printing the AST" << endl;
    ast->print();
    cout << endl;
	*/
    //cout << "now printing the MIPS I code" << endl;
	cout << "    .file       1 \"test.c\"" << endl;				// TODO
	cout << "    .section    .mdebug.abi32" << endl;
	cout << "    .previous   " << endl;
	cout << "    .nan        legacy" << endl;
	cout << "    .module     fp=xx" << endl;
	cout << "    .module     nooddspreg" << endl;
	cout << "    .abicalls   " << endl;
	cout << endl;
    ast->compile(ctxt, 99);
    cout << endl;

    return 0;
}

/* HOW TO TEST 

couting my shit:
make clean && clear && make bin/c_compiler && ./bin/c_compiler<test.in
make clean && clear && make bin/c_compiler && ./bin/c_compiler<test.in > main.s

alternatively:
bash ./run.sh

generating the mips with gcc:
mips-linux-gnu-gcc -c -S test.c

executing .s :
mips-linux-gnu-gcc -static main.s -o a.out
qemu-mips a.out

*/