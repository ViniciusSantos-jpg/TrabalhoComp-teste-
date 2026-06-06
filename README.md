# Compilador C--

Este projeto consiste na implementação de um compilador para a linguagem **C--**, 
desenvolvido como trabalho prático da disciplina de **Compiladores I (2026/1)**, 
sob orientação da Profa. Bianca de Almeida Dantas.

## Autores
* **Ana Laura Rizzo Cardoso**
* **Gislayne Garabini Damasceno**
* **Vinícius Santos Silva**

## Funcionalidades Implementadas

- **Analisador Léxico (Scanner):** Conversão do código-fonte `.cmm` em *Tokens*, com suporte a comentários com // e tratamento de erros.
- **Analisador Sintático (Parser):** Implementação de um analisador preditivo descendente recursivo (Top-Down) para validação da estrutura gramatical.
- **Tabela de Símbolos:** Estrutura de dados hierárquica (com suporte a escopos) para diferenciação entre identificadores e palavras reservadas.
- **Recuperação de Erros (Modo Pânico):** Estratégia de sincronização de tokens que permite reportar múltiplos erros sintáticos em uma única execução sem encerrar prematuramente.

## Requisitos

- Sistema Linux, Unix ou WSL (Windows)
- Compilador `g++`

## Como Compilar e Executar

### 1. Compilar o projeto

Na pasta raiz do projeto, execute:

```bash
g++ *.cpp -o cmm
```

### 2. Executar o compilador

Para analisar um arquivo fonte `.cmm`:

```bash
./cmm teste1.cmm
```

## Saída

O compilador analisa o arquivo informado e:

- Exibe mensagens de erro léxico ou sintático com a linha da ocorrência;
- Continua a análise após erros sintáticos via recuperação em modo pânico;
- Informa quando a compilação é concluída com sucesso.