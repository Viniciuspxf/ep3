/*
 \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__

  AO PREENCHER ESSE CABEÇALHO COM O MEU NOME E O MEU NÚMERO USP, 
  DECLARO QUE SOU O ÚNICO AUTOR E RESPONSÁVEL POR ESSE PROGRAMA. 
  TODAS AS PARTES ORIGINAIS DESSE EXERCÍCIO PROGRAMA (EP) FORAM 
  DESENVOLVIDAS E IMPLEMENTADAS POR MIM SEGUINDO AS INSTRUÇÕES DESSE EP
  E QUE PORTANTO NÃO CONSTITUEM PLÁGIO. DECLARO TAMBÉM QUE SOU RESPONSÁVEL
  POR TODAS AS CÓPIAS DESSE PROGRAMA E QUE EU NÃO DISTRIBUI OU FACILITEI A
  SUA DISTRIBUIÇÃO. ESTOU CIENTE QUE OS CASOS DE PLÁGIO SÃO PUNIDOS COM 
  REPROVAÇÃO DIRETA NA DISCIPLINA.

  Nome:

  eval.c
  Pitao I

  Referências: Com exceção das rotinas fornecidas no esqueleto e em sala
  de aula, caso você tenha utilizado alguma referência, liste-as abaixo
  para que o seu programa não seja considerado plágio.
  Exemplo:
  - função mallocc retirada de: http://www.ime.usp.br/~pf/algoritmos/aulas/aloca.html

 \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__
*/

/*
  NAO EDITE OU MODIFIQUE NADA QUE ESTA ESCRITO NESTE ESQUELETO
*/

/*------------------------------------------------------------*/
/* interface para as funcoes deste modulo */
#include "eval.h" 

/*------------------------------------------------------------*/
#include <stdlib.h>  /* atoi(), atof() */
#include <string.h>  /* strncmp(), strlen(), strncpy(), strcpy(), strcmp() */
#include <math.h>    /* pow() */

/*------------------------------------------------------------*/
#include "categorias.h" /* Categoria, MAX_OPERADORES */
#include "util.h"       /* ERRO(), AVISO(), mallocSafe() */
#include "objetos.h"    /* CelObjeto, mostreObjeto(), freeObjeto()
                           mostreListaObjetos(), 
                           freeListaObjetos() */

#include "stack.h"      /* funcoes que manipulam uma pilha */ 
                        /* essa pilha sera usada para implementar 
                           a pilha de execucao */


/*------------------------------------------------------------*/
/* Protipos de funcoes auxiliares */

/*------------------------------------------------------------*/
/* Tabela com uma representacao da precedencia dos operadores
 * atraves de valores inteiros. 
 * Quanto maior o valor, maior a precedencia.
 *
 *  http://www.ime.usp.br/~pf/algoritmos/apend/precedence.html
 */
static const int precedencia[MAX_OPERADORES] =
{
    /* 4 operadores relacionais com 2 simbolos  */
     4   /* "==" */ /* da esquerda para a direita */
    ,4   /* "!=" */ /* da esquerda para a direita */
    ,5   /* ">=" */ /* da esquerda para a direita */
    ,5   /* "<=" */ /* da esquerda para a direita */
    
    /* 2 operadores aritmeticos com 2 simbolos */
    ,8 /* ,"**" */ /* da direita para a esquerda */
    ,7 /* ,"//" */ /* da esquerda para a direita */
 
    /* 2 operadores relacionais com 1 simbolo */
    ,5  /* ">"  */ /* da esquerda para a direita */
    ,5  /* "<"  */ /* da esquerda para a direita */ 
    
    /* 6 operadores aritmeticos */
    ,7  /* "%" */ /* da esquerda para a direita */
    ,7  /* "*" */ /* da esquerda para a direita */
    ,7  /* "/" */ /* da esquerda para a direita */
    ,6  /* "+" */ /* da esquerda para a direita */
    ,6  /* "-" */ /* da esquerda para a direita */
    ,8  /* "_" */ /* da direita para a esquerda */
    
    /* 3 operadores logicos  */
    ,3  /* "and" */ /* da esquerda para a direita */ 
    ,2  /* "or" */ /* da esquerda para a direita */
    ,8  /* "not"  */ /* da direita para a esquerda */
    
    /* operador de indexacao */
    ,9  /* "$"  */ /* da esquerda para a direita (IGNORAR)*/

    /* atribuicao */ 
    ,1  /* "=" */ /* da direita para a esquerda EP4 */ 
}; 

/*-------------------------------------------------------------
 *  itensParaValores
 *  
 *  RECEBE uma lista ligada com cabeca INIFILAITENS representando uma
 *  fila de itens lexicos. Inicialmente, o campo VALOR de cada celula
 *  contem um string com o item lexico.
 *
 *  A funcao CONVERTE o campo VALOR de cada celula para um inteiro 
 *  ou double, como descrito a seguir. 
 *
 *  IMPLEMENTACAO
 *
 *  O campo VALOR de uma CelObjeto tem 3 subcampos:
 *
 *      - valor.vInt   (int)
 *      - valor.vFloat (float)
 *      - valor.pStr   (string)
 *
 *  Nessa conversao, o campo VALOR de cada celula recebe um valor 
 *  da seguinte maneira.
 *
 *     - Se o campo CATEGORIA da celula indica que o item e' um
 *       string representando um int (INT_STR) entao o campo 
 *       valor.vFloat deve receber esse numero inteiro. 
 *
 *       Nesse caso a CATEGORIA do item deve ser alterada para 
 *       FLOAT.
 *
 *     - se o campo CATEGORIA da celula indica que o item e' um
 *       string representando um float (FLOAT_STR) entao o campo 
 *       valor.vFloat deve receber esse float;
 *
 *       Nesse caso a CATEGORIA do item deve ser alterada para 
 *       FLOAT.
 *
 *     - se o campo CATEGORIA da celula indica que o item e' um
 *       string representando um Bool (BOOL_STR) entao o campo 
 *       valor.vFloat deve receber o inteiro correspondente 
 *       (False = 0, True = 1);
 *
 *       Nesse caso a CATEGORIA do item deve ser alterada para 
 *       FLOAT.
 *
 *     - se o campo CATEGORIA da celula indica que o item e' um
 *       string representando um operador (OPER_*) entao o campo 
 *       valor.vInt deve receber o inteiro correspondente 
 *       a precedencia desse operador. 
 *       
 *       Para isto utilize o vetor PRECEDENCIA declarado antes 
 *       desta funcao. 
 *
 * Nesta funcao (e nas outras) voce pode utilizar qualquer funcao da
 * biblioteca string do C, como, por exemplo, atoi(), atof().
 *
 * Esta funcao (e todas as outras) devem 'dar' free nas estruturas que
 * deixarem de ser necessarias.
 *
 * Esse e o caso dos strings dos itens das categorias FLOAT_STR e INT_STR.  
 *
 * Os campos strings de objetos OPER_* e BOOL_STR apontam para
 * strings em tabelas definidas no modulo lexer.h.  Nesse
 * caso, tentar liberar essa memoria e' um erro.
 *
 */

void itensParaValores(CelObjeto *iniFilaItens) {
    CelObjeto *aux;
    for (aux = iniFilaItens->prox; aux != NULL; aux = aux->prox) {
        switch (aux->categoria) {
            case (INT_STR):
                aux->valor.vFloat = atoi(aux->valor.pStr);
                aux->categoria = FLOAT;
                if (aux->valor.pStr == NULL) printf("AAAAAAH");
                /*printf("%s\n", aux->valor.pStr);*/
                /*free(aux->valor.pStr);*/
                break;
            case (FLOAT_STR):
                aux->valor.vFloat = atof(aux->valor.pStr);
                aux->categoria = FLOAT;
                if (aux->valor.pStr == NULL) printf("AAAAAAH");
                /*printf("%s\n", aux->valor.pStr);*/
                /*free(aux->valor.pStr);*/
                break;
            case (BOOL_STR):
                if (strncmp("True", aux->valor.pStr, strlen("True")))
                    aux->valor.vFloat = 1;
                else aux->valor.vFloat = 0;
                aux->categoria = FLOAT;
                break;
            default:
                aux->valor.vInt = precedencia[aux->categoria];
                break;

        }
    }
}

/*-------------------------------------------------------------
 * eval
 * 
 * RECEBE uma lista ligada como cabeca INIPOSFIXA que representa
 * uma expressao em notacao posfixa. 
 *
 * RETORNA o endereco de uma celula com o valor da expressao.
 *
 * PRE-CONDICAO: a funcao supoe que a expressao esta' sintaticamente
 *               correta.
 *
 * IMPLEMENTACAO
 *
 * Para o calculo do valor da expressao, deve ser usada uma pilha. 
 * O endereco retornado sera' o da celula no topo no topo dessa  
 * "pilha de execucao".
 * 
 * A funcao percorre a expressao calculando os valores resultantes.
 * Para isto e' utilizada uma pilha de execucao. 
 * 
 * A implementacao das funcoes que manipulam a pilha devem ser
 * escritas no modulo stack.c.
 * 
 * O arquivo stack.h contem a interface para essas funcoes. 
 * A pilha de execucao so deve ser acessada atraves dessa interface
 * (caso contrario nao chamariamos stack.h de interface).
 * 
 * O parametro mostrePilhaExecucao indica se os valores
 * na pilha de execucao devem ser exibidos depois de qualquer 
 * alteracao.
 * 
 * Esta funcao deve "dar free" nas estruturas que deixarem de ser 
 * necessarias.
 *
 */

CelObjeto * eval (CelObjeto *iniPosfixa, Bool mostrePilhaExecucao) {
    Stack pilha = stackInit();
    CelObjeto *aux, entrada, *resultado = NULL;
    double operandoA, operandoB;

    entrada.categoria = FLOAT;

    for (aux = iniPosfixa->prox; aux != NULL; aux = aux->prox) {
        if (aux->categoria == OPER_MENOS_UNARIO || aux->categoria == OPER_LOGICO_NOT) {
            operandoA = stackPop(pilha).valor.vFloat;
        }
        else if (aux->categoria != FLOAT) {
            operandoB = stackPop(pilha).valor.vFloat;
            operandoA = stackPop(pilha).valor.vFloat; 
        }
        switch (aux->categoria) {
            case (OPER_IGUAL): 
                if (operandoA == operandoB) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            case (OPER_DIFERENTE):
                if (operandoA != operandoB) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            case (OPER_MAIOR_IGUAL):
                if (operandoA >= operandoB) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            case (OPER_EXPONENCIACAO):
                entrada.valor.vFloat = pow(operandoA, operandoB);
                break;
            case (OPER_DIVISAO_INT):
                entrada.valor.vFloat = (int) (operandoA/operandoB);
                break;
            case (OPER_MAIOR):
                if (operandoA > operandoB) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            case (OPER_MENOR):
                if (operandoA < operandoB) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            case (OPER_RESTO_DIVISAO):
                entrada.valor.vFloat = operandoA - operandoB*((int) (operandoA/operandoB));
                break;
            case (OPER_MULTIPLICACAO):
                entrada.valor.vFloat = operandoA*operandoB;
                break;
            case (OPER_DIVISAO):
                entrada.valor.vFloat = operandoA/operandoB;
                break;
            case (OPER_ADICAO):
                entrada.valor.vFloat = operandoA+operandoB;
                break;
            case (OPER_SUBTRACAO):
                entrada.valor.vFloat = operandoA-operandoB;
                break;
            case (OPER_MENOS_UNARIO):
                entrada.valor.vFloat = -operandoA;
                break;
            case (OPER_LOGICO_AND):
                if (operandoA*operandoB == 0) entrada.valor.vFloat = 0;
                else entrada.valor.vFloat = 1;
                break;
            case (OPER_LOGICO_OR):
                if (operandoA == 0  && operandoB == 0) entrada.valor.vFloat = 0;
                else entrada.valor.vFloat = 1;
                break;
            case (OPER_LOGICO_NOT):
                if (operandoA == 0) entrada.valor.vFloat = 1;
                else entrada.valor.vFloat = 0;
                break;
            default:
                entrada.valor.vFloat = aux->valor.vFloat;
                break;
        }
        stackPush(pilha, entrada);
        if (mostrePilhaExecucao) {
            printf("\n==========================\n");
            printf("  Pilha de execucao\n");
            printf("  valor\n");
            printf(". . . . . . . . . . . . . . .\n");
            stackDump(pilha);
            printf("\n");
        }
    }
    if (!stackEmpty(pilha)) {
        resultado = mallocSafe(sizeof(CelObjeto));
        *resultado = stackPop(pilha);
    }
    stackFree(pilha);
    return resultado; 
}
