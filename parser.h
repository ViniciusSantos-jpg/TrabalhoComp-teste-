#include "scanner.h"

class Parser
{
	private:
		Scanner* scanner;
		Token* lToken;
		SymbolTable* globalST;
    	SymbolTable* currentST;

		// VARIÁVEIS PARA O MODO PÂNICO:
		bool hasErrors;
		int lastErrorLine;
		int linhaAnterior;

		void advance();
		void match(int);
		void error(string);
		void initSimbolTable();
		void synchronize();

		void program();
		void function();
		void varDeclaration();
		void type();
		void paramTypes();
		void statement();
		void assign();
		void expression();
		void term();

	public:
		Parser(string);
		void run();
};