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

  stack.c
  Pitao I

  Referências: Com exceção das rotinas fornecidas no esqueleto e em sala
  de aula, caso você tenha utilizado alguma referência, liste-as abaixo
  para que o seu programa não seja considerado plágio.

  Exemplo:
  - função mallocc retirada de: 
 
    http://www.ime.usp.br/~pf/algoritmos/aulas/aloca.html

 \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__
*/

/* interface para o uso de uma pilha */
#include "stack.h"
#include <stdlib.h>

/* 
 * 
 * STACK.c: IMPLEMENTACAO DE UMA PILHA 
 *
 * TAREFA
 *
 * Faca aqui a implementacao de uma pilha atraves de uma 
 * __lista encadeada com cabeca__. 
 * 
 * A implementacao deve respeitar a interface que voce declarar em 
 * stack.h. 
 *
 */

Stack stackInit() {
    Stack pilha = mallocSafe(sizeof(CelObjeto));
    pilha->categoria = INDEFINIDA;
    pilha->valor.pStr = NULL;
    pilha->prox = NULL;
    return pilha;
}

int stackEmpty(Stack pilha) {
    return (pilha->prox == NULL);
}

void stackPush(Stack pilha, CelObjeto celula){
    CelObjeto *aux;

    aux = pilha->prox;
    pilha->prox = mallocSafe(sizeof(CelObjeto));
    *(pilha->prox) = celula;
    /*pilha->prox->categoria = celula.categoria;
    pilha->prox->valor.pStr = celula.valor.pStr;
    pilha->prox->valor.vFloat = celula.valor.vFloat;
    pilha->prox->valor.vInt = celula.valor.vInt;
    */
    pilha->prox->prox = aux;
}

CelObjeto stackPop(Stack pilha) {
    CelObjeto objeto;

    objeto = *(pilha->prox);
    free(pilha->prox);
    pilha->prox = objeto.prox;
    return objeto;
}

CelObjeto stackTop(Stack pilha) {
    return *(pilha->prox);
}

void stackFree(Stack pilha) {
    Stack aux;
    while (pilha != NULL) {
        aux = pilha->prox;
        free(pilha);
        pilha = aux;
    }

}

void stackDump(Stack pilha) {
    Stack aux;
    for (aux = pilha->prox; aux != NULL; aux = aux->prox)
        printf("  %.6lf\n", aux->valor.vFloat);

}