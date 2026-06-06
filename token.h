#include <iostream>
#include <ctype.h>//Funções de caracteres
#include <string>

using namespace std;

enum Names
{
    UNDEF,

    // Identificadores e constantes
    ID,
    INTEGER,
    CHARACTER,
    STRING,

    // Operadores
    OP,
    LESS_THAN,        // <
    LESS_OR_EQUAL,    // <=
    GREATER_THAN,     // >
    GREATER_OR_EQUAL, // >=
    EQUAL,            // ==
    NOT_EQUAL,        // !=
    ASSIGNMENT,       // =
    ADDITION,         // +
    SUBTRACTION,      // -
    ASTERISK,         // *
    BAR,              // /
    AMPERSAND,        // &&
    DOUBLE_PIPES,     // ||
    EXCLAMATION_MARK, // !

    // Separadores
    SEP,
    LEFT_PARENTHESE,  // (
    RIGHT_PARENTHESE, // )
    LEFT_BRACE,       // {
    RIGHT_BRACE,      // }
    LEFT_BRACKET,     // [
    RIGHT_BRACKET,    // ]
    SEMICOLON,        // ;
    COMMA_LITERAL,    // ,

    // Palavras reservadas da C--
    INT,
    CHAR,
    VOID,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,

    END_OF_FILE
};

class Token 
{
    public: 
        int name;
        int attribute;
        string lexeme;
    
        Token(int name)
        {
            this->name = name;
            attribute = UNDEF;
        }

        Token(int name, string l)
        {
            this->name = name;
            attribute = UNDEF;
            lexeme = l;
        }
        
        Token(int name, int attr)
        {
            this->name = name;
            attribute = attr;
        }
};
