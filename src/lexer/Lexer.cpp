#include "Lexer.h"
#include <cstdlib>

int lexer::current_token;
std::string lexer::identifier;
double lexer::number;

// The actual implementation of the lexer is a single function
// get_current_token(). It's called to return the next token from standard input
// get_current_token() works by invoking the getchar() function to 
// read characters one at a time. Then, it recognizes these and stores 
// the last character in last_character.
int lexer::get_current_token() {
    static int last_character = ' ';

    // The first thing that we need to 
    // do is to ignore whitespaces between tokens
    while (isspace(last_character)) {
        last_character = getchar();
    }

    // Alphanumerics (DEF, EXTERN, IDENTIFIER)
    if (isalpha(last_character)) {
        identifier = last_character;
        while (isalnum(last_character = getchar())) {
            identifier += last_character;
        }
        // Def
        if (identifier == "def") {
            return DEFINITION;
        }
        // Extern
        if (identifier == "extern") {
            return EXTERN;
        }
        // Identifier
        return IDENTIFIER;
    } 

    // Numbers
    if (isdigit(last_character) || last_character == '.') {
        std::string number_string;
        number_string += last_character;
        
        while (isdigit(last_character = getchar()) || last_character == '.') {
            number_string += last_character;
        }
        number = strtod(number_string.c_str(), 0);
        // Number
        return NUMBER;
    }

    // Comments/EOF
    if (last_character == '#') {
        while ((last_character = getchar()) != EOF 
                && last_character != '\n' 
                && last_character != '\r');
        if (last_character == EOF) {
            return END_OF_FILE;
        }
    }

    // EOF
    if (last_character == EOF) {
        return END_OF_FILE;
    }

    // Others [0-255]
    int this_character = last_character;
    last_character = getchar();
    return this_character;
}

int lexer::get_next_token() {
    return current_token = get_current_token();
}