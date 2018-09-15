// SC-T-603-THYD Fall 2018. Project part I.
#include <iostream>
#include <fstream>
#include "hlexer.h"
#include "flexer.h"

using namespace std;

int main(int argc, char* argv[]) {

    // Process command-line arguments, if any.
    // Usage:  program [ option [ filename ] ]  (option -h or -f)
    bool use_flex = false;
    if ( argc >= 2 && string(argv[1]) == "-f" ) {
        use_flex = true;
    }
    string filename( "test.decaf" );
    if ( argc >= 3 ) {
        filename = argv[2];
    }

    // Open file with Decaf program, exit if error opening file.
    ifstream fis( filename );
    if ( !fis.good() ) {
        cerr << "Could not open input file '" << filename << "'." << endl;
        return -1;
    }

    // Instantiate the right lexer.
    Lexer* lexer;
    /*if (use_flex) {
        lexer = new FLexer(fis);
    }
    else {
        lexer = new HLexer(fis);
    }*/
    lexer = new FLexer(fis);

    // Output tokens.
    cout << "Using lexical analyzer: " << lexer->get_name() << endl;
    Token token;
    lexer->get_next( token );
    while ( token.type != Tokentype::EOI ) {
        cout << '(' << token.type << ','
                    << token.lexeme << ','
                    << token.line
             << ')' << endl;
        lexer->get_next( token );
    }

    // Clean up and return.
    delete lexer;
    return 0;
}