#include "parser.h"

Parser::Parser(string input)
{
	currentST = globalST = new SymbolTable();
	initSimbolTable();

	scanner = new Scanner(input, globalST);

	// INICIALIZANDO VARIÁVEIS DE ERRO:
	hasErrors = false;
	lastErrorLine = 0;
	linhaAnterior = 1;
}

void
Parser::advance()
{
	linhaAnterior = scanner->getLine(); // Salva a linha antes de avançar
	lToken = scanner->nextToken();
}

void
Parser::match(int t)
{
	if (lToken->name == t || lToken->attribute == t) {
		advance();
	} else {
		// O match agora sabe o que faltou
		if (t == SEMICOLON) error("esperado ';'");
		else if (t == RIGHT_PARENTHESE) error("esperado ')'");
		else if (t == LEFT_BRACE) error("esperado '{'");
		else if (t == RIGHT_BRACE) error("esperado '}'");
		else if (t == ASSIGNMENT) error("esperado '='");
		else if (t == ID) error("esperado identificador");
		else error("sintaxe invalida");
		
		synchronize(); // Aciona o modo pânico
	}
}

void 
Parser::synchronize()
{
    while (lToken->name != END_OF_FILE) {
        // Para se encontrar um fechamento ou ponto e vírgula
        if (lToken->attribute == SEMICOLON || lToken->attribute == RIGHT_BRACE || lToken->attribute == LEFT_BRACE) {
            return;
        }
        
        // Para se encontrar palavra que inicia novo bloco
        if (lToken->name == INT || lToken->name == CHAR || lToken->name == VOID || 
            lToken->name == IF || lToken->name == WHILE || lToken->name == FOR || lToken->name == RETURN) {
            return;
        }
        advance(); // Descarta token problemático
    }
}

void
Parser::run()
{
	advance();	
	program();

	if (!hasErrors) {
		cout << "Compilação encerrada com sucesso!\n";
	} else {
		cout << "\nCompilação encerrada com erros estruturais. Verifique as mensagens acima.\n";
	}
}

void
Parser::program()
{
	while (lToken->name != END_OF_FILE)
		function();
}

void
Parser::function()
{
	// Verifica tipo de retorno (void ou tipos básicos)
	if (lToken->name == VOID)
		advance();
	else
		type();

	match(ID);
	match(LEFT_PARENTHESE);
	paramTypes();
	match(RIGHT_PARENTHESE);

	// Início do corpo da função
	match(LEFT_BRACE);

	while (lToken->name == CHAR || lToken->name == INT)
	{
		type();
		varDeclaration();
		while (lToken->attribute == COMMA_LITERAL)
		{
			advance();
			varDeclaration();
		}
		match(SEMICOLON);
	}

	while (lToken->attribute != RIGHT_BRACE && lToken->name != END_OF_FILE)
		statement();

	match(RIGHT_BRACE);
}

void
Parser::varDeclaration()
{
	match(ID);
	if (lToken->attribute == LEFT_BRACKET)
	{
		advance();
		match(INTEGER);
		match(RIGHT_BRACKET);
	}
}

void
Parser::type()
{
	if (lToken->name == CHAR)
		advance();
	else if (lToken->name == INT)
		advance();
	else
		error("Erro de Tipo (int ou char)");
}

void 
Parser::paramTypes()
{
	if (lToken->name == VOID)
		advance();
	else
	{
		type();
		match(ID);
		if (lToken->attribute == LEFT_BRACKET)
		{
			advance();
			match(RIGHT_BRACKET);
		}
		while (lToken->attribute == COMMA_LITERAL)
		{
			advance();
			type();
			match(ID);
			if (lToken->attribute == LEFT_BRACKET)
			{
				advance();
				match(RIGHT_BRACKET);
			}
		}
	}
}

void
Parser::statement()
{
	if (lToken->name == IF)
	{
		advance();
		match(LEFT_PARENTHESE);
		expression();
		match(RIGHT_PARENTHESE);
		statement();
		if (lToken->name == ELSE)
		{
			advance();
			statement();
		}
	}
	else if (lToken->name ==  WHILE)
	{
		advance();
		match(LEFT_PARENTHESE);
		expression();
		match(RIGHT_PARENTHESE);
		statement();
	}
	else if (lToken->name == FOR)
	{
		advance();
		match(LEFT_PARENTHESE);
		if (lToken->name == ID)
			assign();
		match(SEMICOLON);
		if (lToken->attribute != SEMICOLON)
			expression();
		match(SEMICOLON);
		if (lToken->name == ID)
			assign();
		match(RIGHT_PARENTHESE);
		statement();
	}
	else if (lToken->name == RETURN)
	{
		advance();
		if (lToken->attribute != SEMICOLON)
			expression();
		match(SEMICOLON);
	}
	else if (lToken->name == ID)
	{
		advance();
		if (lToken->attribute == LEFT_PARENTHESE)
		{
			advance();
			if (lToken->attribute != RIGHT_PARENTHESE)
			{
				expression();
				while (lToken->attribute == COMMA_LITERAL)
				{
					advance();
					expression();
				}
			}
			match(RIGHT_PARENTHESE);
		}
		else
		{
			if (lToken->attribute == LEFT_BRACKET)
			{
				advance();
				expression();
				match(RIGHT_BRACKET);
			}
			match(ASSIGNMENT);
			expression();
		}
		match(SEMICOLON);
	}
	else if (lToken->attribute == LEFT_BRACE)
	{
		advance();
		while (lToken->attribute != RIGHT_BRACE && lToken->name != END_OF_FILE)
			statement();
		match(RIGHT_BRACE);
	}
	else if (lToken->attribute == SEMICOLON)
	{
		advance();
	}
	else
	{
		error("statement inválido");
		advance(); // EVITA LOOP INFINITO
	}
}

void
Parser::assign()
{
	match(ID);
	if (lToken->attribute == LEFT_BRACKET)
	{
		advance();
		expression();
		match(RIGHT_BRACKET);
	}
	match(ASSIGNMENT);
	expression();
}

void
Parser::expression()
{
	term();
	while (lToken->name == OP && lToken->attribute != EXCLAMATION_MARK && lToken->attribute != ASSIGNMENT)
	{
		advance();
		term();
	}
}

void
Parser::term()
{
	if (lToken->name == OP && lToken->attribute == SUBTRACTION)
	{
		advance();
		expression();
	}
	else if (lToken->name == OP && lToken->attribute == EXCLAMATION_MARK)
	{
		advance();
		expression();
	}
	else if (lToken->attribute == LEFT_PARENTHESE)
	{
		advance();
		expression();
		match(RIGHT_PARENTHESE);
	}
	else if (lToken->name == INTEGER)
		advance();
	else if (lToken->name == CHARACTER)
		advance();
	else if (lToken->name == STRING)
		advance();
	else if (lToken->name == ID)
	{
		advance();
		if (lToken->attribute == LEFT_PARENTHESE)
		{
			advance();
			if (lToken->attribute != RIGHT_PARENTHESE)
			{
				expression();
				while (lToken->attribute == COMMA_LITERAL)
				{
					advance();
					expression();
				}
			}
			match(RIGHT_PARENTHESE);
		}
		else if (lToken->attribute == LEFT_BRACKET)
		{
			advance();
			expression();
			match(RIGHT_BRACKET);
		}
	}
	else
	{
		error("expressão inválida");
		advance();     // EVITA LOOP INFINITO
		synchronize(); // RECUPERA DO ERRO
	}
}

void
Parser::initSimbolTable()
{
	Token* t;
        
	t = new Token(INT, "int");
	globalST->add(new STEntry(t, true));
	t = new Token(CHAR, "char");
    globalST->add(new STEntry(t, true));
    t = new Token(VOID, "void");
    globalST->add(new STEntry(t, true));
	t = new Token(IF, "if");
    globalST->add(new STEntry(t, true));
	t = new Token(ELSE, "else");
    globalST->add(new STEntry(t, true));
	t = new Token(WHILE, "while");
    globalST->add(new STEntry(t, true));
	t = new Token(FOR, "for");
    globalST->add(new STEntry(t, true));
	t = new Token(RETURN, "return");
    globalST->add(new STEntry(t, true));
}

void 
Parser::error(string str)
{
    int linhaDoErro = scanner->getLine();
    
    // Se o erro for a falta de um ponto e vírgula, o erro real foi na linha de cima!
    if (str == "esperado ';'") {
        linhaDoErro = linhaAnterior;
    }
    
    // Só imprime se o erro for numa linha diferente
    if (linhaDoErro != lastErrorLine) {
        cout << "Linha " << linhaDoErro << ": erro: " << str << endl;
        lastErrorLine = linhaDoErro; // Atualiza a última linha que deu erro
    }
    
    hasErrors = true; // Marca que o programa tem erros
}
