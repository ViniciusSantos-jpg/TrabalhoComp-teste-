#include "scanner.h"    

//Construtor
Scanner::Scanner(string input, SymbolTable* table)
{
    pos = 0;
    line = 1;

    st = table;

    ifstream inputFile(input, ios::in);
    string ln;

    if (inputFile.is_open())
    {
        while (getline(inputFile, ln))
        {
            this->input.append(ln + '\n');
        }
        inputFile.close();
    }
    else 
        cout << "Unable to open file\n"; 
}

int
Scanner::getLine()
{
    return line;
}

//Método que retorna o próximo token da entrada
Token* 
Scanner::nextToken()
{
    Token* tok;
    string lexeme;
    int state = 1;
    
    while(true)
    {
        //Pula espaços em branco e controla a contagem de linhas
        while(pos < (int)input.size() && isspace(input[pos]))
        {
            if(input[pos] == '\n') line++;
            pos++;
        }

        //Fim de arquivo
        if(pos >= (int)input.size())
            return new Token(END_OF_FILE);

        switch (state)
        {
        case 1:

            //Identificadores e palavras reservadas
            if(isalpha(input[pos]) || input[pos] == '_')
            {
                lexeme.push_back(input[pos]);
                pos++;
                while(isalnum(input[pos]) || input[pos] == '_')
                {
                    lexeme.push_back(input[pos]);
                    pos++;
                }

                //Pesquisa na tabela de símbolos:
                //se encontrar, é palavra reservada; caso contrário, é ID
                STEntry* obj = st->get(lexeme);
                if(!obj)
                    tok = new Token(ID, lexeme);
                else
                    tok = new Token(obj->token->name, lexeme);

                return tok;
            }

            //Constante inteira
            else if(isdigit(input[pos]))
            {
                while(isdigit(input[pos]))
                {
                    lexeme.push_back(input[pos]);
                    pos++;
                }
                tok = new Token(INTEGER, lexeme);
                return tok;
            }

            //Constante character: 'c', '\n' ou '\0'
            else if(input[pos] == '\'')
            {
                pos++;//Consome a aspa simples de abertura
                lexeme = "'";

                if(input[pos] == '\\')
                {
                    //Sequências de escape válidas: \n e \0
                    pos++;
                    if(input[pos] == 'n' || input[pos] == '0')
                    {
                        lexeme += '\\';
                        lexeme += input[pos];
                        pos++;
                    }
                    else
                        lexicalError("Sequência inválida");
                }
                else if(isprint(input[pos]) && input[pos] != '\'' && input[pos] != '\\')
                {
                    lexeme += input[pos];
                    pos++;
                }
                else
                    lexicalError("Caractere inválido em constante char");

                if(input[pos] != '\'')
                    lexicalError("constante character não terminada");
                pos++;//Consome a aspa simples de fechamento
                lexeme += "'";

                tok = new Token(CHARACTER, lexeme);
                return tok;
            }

            //Constante string
            else if(input[pos] == '"')
            {
                pos++;//Consome aspas de abertura
                lexeme = "\"";
                while(pos < (int)input.size() && input[pos] != '"')
                {
                    if(!isprint(input[pos]) || input[pos] == '\\' || input[pos] == '\n')
                        lexicalError("caractere inválido dentro de string");
                    lexeme += input[pos];
                    pos++;
                }
                if(pos >= (int)input.size())
                    lexicalError("string não terminada");
                pos++;//Consome aspas de fechamento
                lexeme += "\"";
                tok = new Token(STRING, lexeme);
                return tok;
            }

            //Operadores: < ou <=
            else if(input[pos] == '<')
            {
                pos++;
                if(input[pos] == '=')
                {
                    pos++;
                    tok = new Token(OP, LESS_OR_EQUAL);
                }
                else
                    tok = new Token(OP, LESS_THAN);
                return tok;
            }

            //Operadores: > ou >=
            else if(input[pos] == '>')
            {
                pos++;
                if(input[pos] == '=')
                {
                    pos++;
                    tok = new Token(OP, GREATER_OR_EQUAL);
                }
                else
                    tok = new Token(OP, GREATER_THAN);
                return tok;
            }

            //Operadores: = ou ==
            else if(input[pos] == '=')
            {
                pos++;
                if(input[pos] == '=')
                {
                    pos++;
                    tok = new Token(OP, EQUAL);
                }
                else
                    tok = new Token(OP, ASSIGNMENT);
                return tok;
            }

            //Operador ! ou !=
            else if(input[pos] == '!')
            {
                pos++;
                if(input[pos] == '=')
                {
                    pos++;
                    tok = new Token(OP, NOT_EQUAL);
                }
                else
                    tok = new Token(OP, EXCLAMATION_MARK);
                return tok;
            }

            //Operadores: +, -, *
            else if(input[pos] == '+')
            {
                pos++;
                tok = new Token(OP, ADDITION);
                return tok;
            }
            else if(input[pos] == '-')
            {
                pos++;
                tok = new Token(OP, SUBTRACTION);
                return tok;
            }
            else if(input[pos] == '*')
            {
                pos++;
                tok = new Token(OP, ASTERISK);
                return tok;
            }

            //Operador / ou comentários
            else if(input[pos] == '/')
            {
                pos++;
                //Comentário de linha
                if(input[pos] == '/')
                {
                    while(pos < (int)input.size() && input[pos] != '\n')
                        pos++;
                    state = 1;//Ignora o comentário e busca o próximo token
                }
                //Comentário de bloco
                else if(input[pos] == '*')
                {
                    pos++;
                    state = 2;
                }
                //Caso seja apenas o operador /
                else
                {
                    tok = new Token(OP, BAR);
                    return tok;
                }
            }

            //Operador &&
            else if(input[pos] == '&')
            {
                pos++;
                if(input[pos] == '&')
                {
                    pos++;
                    tok = new Token(OP, AMPERSAND);
                    return tok;
                }
                else
                    lexicalError("Operador inválido '&', esperado '&&'");
            }

            //Operador ||
            else if(input[pos] == '|')
            {
                pos++;
                if(input[pos] == '|')
                {
                    pos++;
                    tok = new Token(OP, DOUBLE_PIPES);
                    return tok;
                }
                else
                    lexicalError("Operador inválido '|', esperado '||'");
            }

            //Separador (
            else if(input[pos] == '(')
            {
                pos++;
                tok = new Token(SEP, LEFT_PARENTHESE);
                return tok;
            }
            //Separador )
            else if(input[pos] == ')')
            {
                pos++;
                tok = new Token(SEP, RIGHT_PARENTHESE);
                return tok;
            }
            //Separador {
            else if(input[pos] == '{')
            {
                pos++;
                tok = new Token(SEP, LEFT_BRACE);
                return tok;
            }
            //Separador }
            else if(input[pos] == '}')
            {
                pos++;
                tok = new Token(SEP, RIGHT_BRACE);
                return tok;
            }
            //Separador [
            else if(input[pos] == '[')
            {
                pos++;
                tok = new Token(SEP, LEFT_BRACKET);
                return tok;
            }
            //Separador ]
            else if(input[pos] == ']')
            {
                pos++;
                tok = new Token(SEP, RIGHT_BRACKET);
                return tok;
            }
            //Separador ,
            else if(input[pos] == ',')
            {
                pos++;
                tok = new Token(SEP, COMMA_LITERAL);
                return tok;
            }
            //Separador ;
            else if(input[pos] == ';')
            {
                pos++;
                tok = new Token(SEP, SEMICOLON);
                return tok;
            }

            else
                lexicalError("Caractere inesperado '" + string(1, input[pos]) + "'");

            break;

        //Estado 2: dentro de comentário de bloco
        case 2:
            while(pos + 1 < (int)input.size())
            {
                if(input[pos] == '\n') line++;
                if(input[pos] == '*' && input[pos+1] == '/')
                {
                    pos += 2;
                    state = 1;
                    break;
                }
                pos++;
            }
            if(state == 2)
                lexicalError("Comentário de bloco não terminado");
            break;
        }
    }
}

void 
Scanner::lexicalError(string msg)
{
    cout << "Linha " << line << ": " << msg << endl;
    exit(EXIT_FAILURE);
}
