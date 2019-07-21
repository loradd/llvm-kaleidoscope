#ifndef __LEXER_H__
#define __LEXER_H__

#include <string>

namespace lexer {

    // Provides a simple token buffer
    // CurrentToken is the current token the parser is looking at.
    // GetNextToken reads another token  from the lexer and updates 
    // the CurrentToken with its results.
    extern int current_token;
    // retrieve current token
    int get_current_token();
    // retrieve next token
    int get_next_token();

    // If the current token is an IDENTIFIER
    // Identifier will hold its value 
    extern std::string identifier;

    // If the current token is a NUMBER
    // Number will hold its value
    extern double number;

    // The lexer returns tokens [0-255] if it is an
    // unknown character, otherwise one of these
    // for known things
    enum Token {
        END_OF_FILE = -1, // end-of-file
        DEFINITION  = -2, // definition
        EXTERN      = -3, // extern
        IDENTIFIER  = -4, // identifier
        NUMBER      = -5, //number 
    };

}

#endif

