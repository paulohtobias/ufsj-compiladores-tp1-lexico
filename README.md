# Paulo C Compiler: Analisador Léxico

**Primeiro trabalho prático da disciplina de compiladores. Analisador Léxico.**

---
## Sobre

**Autor:**  Paulo Henrique Tobias

**Apresentado em:** 12/09/2019

---

## Dependências

libpcre2-8 - PCRE2 - Perl compatible regular expressions C library (2nd API) with 8 bit character support.

---
## Compilação

Comando: `make`

---
## Execução

Comando: `./pcc [OPCOES] ARQUIVO`

Onde `ARQUIVO` é o caminho de um código fonte da linguagem **C**.

Onde `OPCOES` pode ser zero ou mais das seguintes opções:
```
  -C	Controla se os comentários serão salvos na lista de tokens ou se serão descartados.

  -O	FILE	Diretório de saída para os arquivos. "stdout" para exibir na tela. Se a opção não for usada, serão exibidos somente warnings e erros. Padrão: ''

  -h	Exibe a mensagem de ajuda.
```

---
## Limitações

- Pouco suporte às diretivas de pré-processamento.

---
## Saída

Durante a análise do código, erros e warnings encontrados serão exibidos na saída de erros `stderr` no seguinte formato:

**`arquivo`:`linha`:`coluna`:** `mensagem`

`lexema`

---
A exibição da lista de tokens e da tabela de símbolos depende das seguintes regras:

- Se a opção `-O` não foi passada, esta etapa é pulada.
- Se `stdout` foi passado para `-O`, então apenas a lista de tokens é exibida na tela.
- Se um nome válido de diretório foi passado para `-O`, então serão criados vários arquivos dentro deste diretório. Um chamado `Token.txt` contendo a lista de tokens e arquivos no formato `tipo-subtipo.txt` para cada token encontrado.

Um `token` será exibido no seguinte formato:
```
TOKEN: {tipo}
	Subtipo: {subtipo}
	Arquivo: {arquivo}
	Linha: {linha} | Coluna: {coluna}
	Lexema: {lexema}
	[Valor: {valor}]
```

### `tipo`
Representação em string do tipo do token. Possíveis valores podem ser conferidos na tabela abaixo:

| Código  | Valor | String          |
|---------|-------|-----------------|
| TK_ID   | 0     | Identificador   |
| TK_KW   | 1     | Palavra-chave   |
| TK_CNST | 2     | Constante       |
| TK_OP   | 3     | Operador        |
| TK_EXT  | 4     | Extra           |
| TK_CMT  | 5     | Comentário      |
| TK_PP   | 6     | Pré-Processador |

### `subtipo`
Representação em string do subtipo do token. Possíveis valores podem ser conferidos nas tabelas abaixo:

|        	| Identificador 	|        	|
|--------	|---------------	|--------	|
| Código 	| Valor         	| String 	|
| --     	| 0             	| N/A    	|

|                	| Palavra-chave 	|          	|
|----------------	|---------------	|----------	|
| Código         	| Valor         	| String   	|
| TK_KW_AUTO     	| 0             	| auto     	|
| TK_KW_BREAK    	| 1             	| break    	|
| TK_KW_CASE     	| 2             	| case     	|
| TK_KW_CHAR     	| 3             	| char     	|
| TK_KW_CONST    	| 4             	| const    	|
| TK_KW_CONTINUE 	| 5             	| continue 	|
| TK_KW_DEFAULT  	| 6             	| default  	|
| TK_KW_DO       	| 7             	| do       	|
| TK_KW_DOUBLE   	| 8             	| double   	|
| TK_KW_ELSE     	| 9             	| else     	|
| TK_KW_ENUM     	| 10            	| enum     	|
| TK_KW_EXTERN   	| 11            	| extern   	|
| TK_KW_FLOAT    	| 12            	| float    	|
| TK_KW_FOR      	| 13            	| for      	|
| TK_KW_GOTO     	| 14            	| goto     	|
| TK_KW_IF       	| 15            	| if       	|
| TK_KW_INT      	| 16            	| int      	|
| TK_KW_LONG     	| 17            	| long     	|
| TK_KW_REGISTER 	| 18            	| register 	|
| TK_KW_RETURN   	| 19            	| return   	|
| TK_KW_SHORT    	| 20            	| short    	|
| TK_KW_SIGNED   	| 21            	| signed   	|
| TK_KW_SIZEOF   	| 22            	| sizeof   	|
| TK_KW_STATIC   	| 23            	| static   	|
| TK_KW_STRUCT   	| 24            	| struct   	|
| TK_KW_SWITCH   	| 25            	| switch   	|
| TK_KW_TYPEDEF  	| 26            	| typedef  	|
| TK_KW_UNION    	| 27            	| union    	|
| TK_KW_UNSIGNED 	| 28            	| unsigned 	|
| TK_KW_VOID     	| 29            	| void     	|
| TK_KW_VOLATILE 	| 30            	| volatile 	|
| TK_KW_WHILE    	| 31            	| while    	|

|               	| Constante 	|                       	|
|---------------	|-----------	|-----------------------	|
| Código        	| Valor     	| String                	|
| TK_CNST_STR   	| 0         	| string-literal        	|
| TK_CNST_CHAR  	| 1         	| char                  	|
| TK_CNST_INT   	| 2         	| inteiro               	|
| TK_CNST_UINT  	| 3         	| inteiro-unsigned      	|
| TK_CNST_LINT  	| 4         	| inteiro-long          	|
| TK_CNST_ULINT 	| 5         	| inteiro-unsigned_long 	|
| TK_CNST_FLT   	| 6         	| float                 	|
| TK_CNST_DBL   	| 7         	| double                	|
| TK_CNST_LDBL  	| 8         	| double-long           	|

|                   	| Operador 	|             	|
|-------------------	|----------	|-------------	|
| Código            	| Valor    	| String      	|
| TK_OP_SETA        	| 0        	| seta        	|
| TK_OP_PONTO       	| 1        	| ponto       	|
| TK_OP_NOTL        	| 2        	| notl        	|
| TK_OP_NOTB        	| 3        	| notb        	|
| TK_OP_INC         	| 4        	| inc         	|
| TK_OP_DEC         	| 5        	| dec         	|
| TK_OP_AST         	| 6        	| ast         	|
| TK_OP_EC          	| 7        	| ec          	|
| TK_OP_BARRA       	| 8        	| barra       	|
| TK_OP_MOD         	| 9        	| mod         	|
| TK_OP_MAIS        	| 10       	| mais        	|
| TK_OP_MENOS       	| 11       	| menos       	|
| TK_OP_SHL         	| 12       	| shl         	|
| TK_OP_SHR         	| 13       	| shr         	|
| TK_OP_MENOR       	| 14       	| menor       	|
| TK_OP_MENOR_IGUAL 	| 15       	| menor_igual 	|
| TK_OP_MAIOR       	| 16       	| maior       	|
| TK_OP_MAIOR_IGUAL 	| 17       	| maior_igual 	|
| TK_OP_IGUAL       	| 18       	| igual       	|
| TK_OP_DIF         	| 19       	| dif         	|
| TK_OP_XOR         	| 20       	| xor         	|
| TK_OP_ORB         	| 21       	| orb         	|
| TK_OP_EL          	| 22       	| el          	|
| TK_OP_ORL         	| 23       	| orl         	|
| TK_OP_TER_C       	| 24       	| ter_c       	|
| TK_OP_ATRIB       	| 25       	| atrib       	|
| TK_OP_AUTO_INC    	| 26       	| auto_inc    	|
| TK_OP_AUTO_DEC    	| 27       	| auto_dec    	|
| TK_OP_AUTO_MUL    	| 28       	| auto_mul    	|
| TK_OP_AUTO_DIV    	| 29       	| auto_div    	|
| TK_OP_AUTO_MOD    	| 30       	| auto_mod    	|
| TK_OP_AUTO_EB     	| 31       	| auto_eb     	|
| TK_OP_AUTO_XORB   	| 32       	| auto_xorb   	|
| TK_OP_AUTO_ORB    	| 33       	| auto_orb    	|
| TK_OP_AUTO_SHL    	| 34       	| auto_shl    	|
| TK_OP_AUTO_SHR    	| 35       	| auto_shr    	|

|                    	| Extra 	|                   	|
|--------------------	|-------	|-------------------	|
| Código             	| Valor 	| String            	|
| TK_EXT_PAR_A       	| 0     	| parenteses-abrir  	|
| TK_EXT_PAR_F       	| 1     	| parenteses-fechar 	|
| TK_EXT_COL_A       	| 2     	| colchete-abrir    	|
| TK_EXT_COL_F       	| 3     	| colchete-fechar   	|
| TK_EXT_CHA_A       	| 4     	| chave-abrir       	|
| TK_EXT_CHA_F       	| 5     	| chave-fechar      	|
| TK_EXT_VIRGULA     	| 6     	| virgula           	|
| TK_EXT_DOIS_PONTOS 	| 7     	| dois-pontos       	|
| TK_EXT_PT_VIRGULA  	| 8     	| ponto-virgula     	|

|        	| Comentário 	|        	|
|--------	|------------	|--------	|
| Código 	| Valor      	| String 	|
| --     	| 0          	| N/A    	|

|               	| Pré-Processador 	|         	|
|---------------	|-----------------	|---------	|
| Código        	| Valor           	| String  	|
| TK_PP_IF      	| 0               	| if      	|
| TK_PP_IFDEF   	| 1               	| ifdef   	|
| TK_PP_IFNDEF  	| 2               	| ifndef  	|
| TK_PP_ELSE    	| 3               	| else    	|
| TK_PP_ELIF    	| 4               	| elif    	|
| TK_PP_ENDIF   	| 5               	| endif   	|
| TK_PP_DEFINE  	| 6               	| define  	|
| TK_PP_INCLUDE 	| 7               	| include 	|

### `arquivo`
Nome do arquivo onde o token foi encontrado. Até o momento, é usado apenas nas mensagens de erro e warning.

### `linha` e `coluna`
Posição em `arquivo` onde o `lexema` do token começa.

### `lexema`
Trecho no código que gerou o token.

### `valor` (opcional)
Representação em string da representação binária do token. Como é usado em poucos casos, este campo aparecerá somente se houve algum valor para ser exibido.

Ex.: o lexema `'\x41'` tem seu valor convertido para `65`, que por sua vez é convertido de volta para string com o valor de `'A'`.

---
Ao final da execução, é exibido na tela a quantidade de erros e warnings.

---
## Tabela de símbolos
A tabela de símbolos foi dividida em várias sub-tabelas, sendo uma para cada subtipo. Apenas o índice do token na lista de tokens é armazenado na tabela para evitar redundância dos dados.

---
## Exemplos

### Sucesso

Entrada: `main.c`
```C
#include <stdio.h>

int main(int argc, char const *argv[]) {
	printf("Hello, world!\n");

	return 0;
}
```

Saída: `./pcc -O stdout main.c`
```
TOKEN: Pré-processador
	Subtipo: include
	Arquivo: main.c
	Linha: 1 | Coluna: 1
	Lexema: #include <stdio.h>

	Valor: #include <stdio.h>

TOKEN: Palavra-chave
	Subtipo: int
	Arquivo: main.c
	Linha: 3 | Coluna: 1
	Lexema: int

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 3 | Coluna: 5
	Lexema: main

TOKEN: Extra
	Subtipo: parenteses-abrir
	Arquivo: main.c
	Linha: 3 | Coluna: 9
	Lexema: (

TOKEN: Palavra-chave
	Subtipo: int
	Arquivo: main.c
	Linha: 3 | Coluna: 10
	Lexema: int

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 3 | Coluna: 14
	Lexema: argc

TOKEN: Extra
	Subtipo: virgula
	Arquivo: main.c
	Linha: 3 | Coluna: 18
	Lexema: ,

TOKEN: Palavra-chave
	Subtipo: char
	Arquivo: main.c
	Linha: 3 | Coluna: 20
	Lexema: char

TOKEN: Palavra-chave
	Subtipo: const
	Arquivo: main.c
	Linha: 3 | Coluna: 25
	Lexema: const

TOKEN: Operador
	Subtipo: ast
	Arquivo: main.c
	Linha: 3 | Coluna: 31
	Lexema: *

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 3 | Coluna: 32
	Lexema: argv

TOKEN: Extra
	Subtipo: colchete-abrir
	Arquivo: main.c
	Linha: 3 | Coluna: 36
	Lexema: [

TOKEN: Extra
	Subtipo: colchete-fechar
	Arquivo: main.c
	Linha: 3 | Coluna: 37
	Lexema: ]

TOKEN: Extra
	Subtipo: parenteses-fechar
	Arquivo: main.c
	Linha: 3 | Coluna: 38
	Lexema: )

TOKEN: Extra
	Subtipo: chave-abrir
	Arquivo: main.c
	Linha: 3 | Coluna: 40
	Lexema: {

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 4 | Coluna: 2
	Lexema: printf

TOKEN: Extra
	Subtipo: parenteses-abrir
	Arquivo: main.c
	Linha: 4 | Coluna: 8
	Lexema: (

TOKEN: Constante
	Subtipo: string-literal
	Arquivo: main.c
	Linha: 4 | Coluna: 9
	Lexema: "Hello, world!\n"
	Valor: Hello, world!


TOKEN: Extra
	Subtipo: parenteses-fechar
	Arquivo: main.c
	Linha: 4 | Coluna: 26
	Lexema: )

TOKEN: Extra
	Subtipo: ponto-virgula
	Arquivo: main.c
	Linha: 4 | Coluna: 27
	Lexema: ;

TOKEN: Palavra-chave
	Subtipo: return
	Arquivo: main.c
	Linha: 6 | Coluna: 2
	Lexema: return

TOKEN: Constante
	Subtipo: inteiro
	Arquivo: main.c
	Linha: 6 | Coluna: 9
	Lexema: 0
	Valor: 0

TOKEN: Extra
	Subtipo: ponto-virgula
	Arquivo: main.c
	Linha: 6 | Coluna: 10
	Lexema: ;

TOKEN: Extra
	Subtipo: chave-fechar
	Arquivo: main.c
	Linha: 7 | Coluna: 1
	Lexema: }

Fim
Warnings: 0
Erros: 0
```
---

### Erros e warnings

Entrada: `main.c`
```C
int main(int argc, char const *argv[]) {
	char c = '\777';

	@

	return 0;
}
```

Saída: `./pcc -O stdout main.c`
```
main.c:2:11: warning: sequência de escape octal fora de alcance
'\777'
main.c:4:2: erro: símbolo '@' inválido

TOKEN: Palavra-chave
	Subtipo: int
	Arquivo: main.c
	Linha: 1 | Coluna: 1
	Lexema: int

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 1 | Coluna: 5
	Lexema: main

TOKEN: Extra
	Subtipo: parenteses-abrir
	Arquivo: main.c
	Linha: 1 | Coluna: 9
	Lexema: (

TOKEN: Palavra-chave
	Subtipo: int
	Arquivo: main.c
	Linha: 1 | Coluna: 10
	Lexema: int

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 1 | Coluna: 14
	Lexema: argc

TOKEN: Extra
	Subtipo: virgula
	Arquivo: main.c
	Linha: 1 | Coluna: 18
	Lexema: ,

TOKEN: Palavra-chave
	Subtipo: char
	Arquivo: main.c
	Linha: 1 | Coluna: 20
	Lexema: char

TOKEN: Palavra-chave
	Subtipo: const
	Arquivo: main.c
	Linha: 1 | Coluna: 25
	Lexema: const

TOKEN: Operador
	Subtipo: ast
	Arquivo: main.c
	Linha: 1 | Coluna: 31
	Lexema: *

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 1 | Coluna: 32
	Lexema: argv

TOKEN: Extra
	Subtipo: colchete-abrir
	Arquivo: main.c
	Linha: 1 | Coluna: 36
	Lexema: [

TOKEN: Extra
	Subtipo: colchete-fechar
	Arquivo: main.c
	Linha: 1 | Coluna: 37
	Lexema: ]

TOKEN: Extra
	Subtipo: parenteses-fechar
	Arquivo: main.c
	Linha: 1 | Coluna: 38
	Lexema: )

TOKEN: Extra
	Subtipo: chave-abrir
	Arquivo: main.c
	Linha: 1 | Coluna: 40
	Lexema: {

TOKEN: Palavra-chave
	Subtipo: char
	Arquivo: main.c
	Linha: 2 | Coluna: 2
	Lexema: char

TOKEN: Identificador
	Subtipo: N/A
	Arquivo: main.c
	Linha: 2 | Coluna: 7
	Lexema: c

TOKEN: Operador
	Subtipo: atrib
	Arquivo: main.c
	Linha: 2 | Coluna: 9
	Lexema: =

TOKEN: Constante
	Subtipo: char
	Arquivo: main.c
	Linha: 2 | Coluna: 11
	Lexema: '\777'
	Valor: �

TOKEN: Extra
	Subtipo: ponto-virgula
	Arquivo: main.c
	Linha: 2 | Coluna: 17
	Lexema: ;

TOKEN: Palavra-chave
	Subtipo: return
	Arquivo: main.c
	Linha: 6 | Coluna: 2
	Lexema: return

TOKEN: Constante
	Subtipo: inteiro
	Arquivo: main.c
	Linha: 6 | Coluna: 9
	Lexema: 0
	Valor: 0

TOKEN: Extra
	Subtipo: ponto-virgula
	Arquivo: main.c
	Linha: 6 | Coluna: 10
	Lexema: ;

TOKEN: Extra
	Subtipo: chave-fechar
	Arquivo: main.c
	Linha: 7 | Coluna: 1
	Lexema: }

Fim
Warnings: 1
Erros: 1
```
