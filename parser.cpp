#include "parser.h"

Parser::Parser(string input)
{
	currentST = globalST = new SymbolTable();
	initSimbolTable();

	scanner = new Scanner(input, globalST);
}

void
Parser::advance()
{
	lToken = scanner->nextToken();
}

void
Parser::match(int t)
{
	if (lToken->name == t || lToken->attribute == t)
		advance();
	else
		error("Erro inesperado");
}

void
Parser::run()
{
	advance();	

	program();
	//TESTE DA TABELA DE SÍMBOLOS
	/*
    currentST = new SymbolTable(currentST);
	currentST = new SymbolTable(currentST);
    if (currentST->add(new STEntry(new Token(ID), "bianca")))
		cout << "Adição de bianca deu certo" << endl;
	else
		cout << "Adição de bianca não deu certo" << endl;

	STEntry* obj = currentST->get("bianca");

	if (obj)
		cout << "Encontrei o símbolo " << obj->lexeme << endl;
	else
		cout << "Não encontrei o símbolo buscado" << endl;

	//Fim do escopo
	currentST = currentST->getParent();

	obj = currentST->get("bianca");

	if (obj)
		cout << "Encontrei o símbolo " << obj->lexeme << endl;
	else
		cout << "Não encontrei o símbolo buscado" << endl;*/

	/////////////////////////////

	cout << "Compilação encerrada com sucesso!\n";
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
	if (lToken->name == VOID)
		advance();
	else
		type();

	match(ID);
	match(LEFT_PARENTHESE);
	paramTypes();
	match(RIGHT_PARENTHESE);
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
		error("statement inválido");
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
		error("expressão inválida");
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
	cout << "Linha " << scanner->getLine() << ": " << str << endl;

	exit(EXIT_FAILURE);
}
