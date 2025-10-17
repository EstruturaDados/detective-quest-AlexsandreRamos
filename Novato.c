// TEMA 4 - NOVATO

#include <stdio.h>      // inclusão da biblioteca padrão para entrada/saída (printf, fgets, etc.).
#include <stdlib.h>     // inclusão para funções de alocação e saída (malloc, free, exit).
#include <string.h>     // inclusão para manipulação de strings (strlen, memcpy).
#include <ctype.h>      // inclusão para funções de classificação de caracteres (isspace).

/* Estrutura que representa uma sala (nó da árvore binária) */
typedef struct Sala {          // início da definição do tipo 'struct Sala'
    char *nome;               // ponteiro para string que guarda o nome da sala (alocado dinamicamente)
    struct Sala *esq;         // ponteiro para o filho à esquerda (subárvore esquerda)
    struct Sala *dir;         // ponteiro para o filho à direita (subárvore direita)
    struct Sala *pai;         // ponteiro para o nó pai (NULL se for a raiz)
} Sala;                       // typedef para simplificar o uso do tipo como 'Sala'

/* Função auxiliar: duplicar string (substitui strdup para portabilidade) */
static char *str_dup(const char *s) {   // função estática que duplica uma string de entrada
    if (!s) return NULL;               // se o ponteiro de entrada for NULL, retorna NULL (proteção)
    size_t n = strlen(s) + 1;          // calcula o tamanho necessário incluindo o caractere nulo '\0'
    char *r = malloc(n);               // aloca memória suficiente para a nova string
    if (!r) {                          // checa se malloc falhou
        fprintf(stderr, "Erro: memória insuficiente ao duplicar string.\n"); // mensagem de erro
        exit(EXIT_FAILURE);            // encerra o programa com código de erro
    }
    memcpy(r, s, n);                   // copia os bytes da string original (incluindo '\0') para a nova área
    return r;                          // retorna o ponteiro para a string duplicada
}

/*
 * criarSala - cria, de forma dinâmica, uma sala com o nome informado.
 * O campo pai é inicializado com NULL; deve ser ajustado ao conectar nós.
 */
Sala *criarSala(const char *nome) {    // função que cria e inicializa um nó (Sala)
    Sala *s = malloc(sizeof(Sala));    // aloca memória para a estrutura Sala
    if (!s) {                          // verifica se a alocação foi bem-sucedida
        fprintf(stderr, "Erro: memória insuficiente ao criar sala.\n"); // mensagem de erro
        exit(EXIT_FAILURE);            // encerra o programa em caso de falha
    }
    s->nome = str_dup(nome);           // duplica a string do nome e armazena no campo 'nome'
    s->esq = NULL;                     // inicializa o filho esquerdo como NULL
    s->dir = NULL;                     // inicializa o filho direito como NULL
    s->pai = NULL;                     // inicializa o ponteiro para o pai como NULL
    return s;                          // retorna o ponteiro para a sala criada
}

/*
 * conectarFilhos - função auxiliar para conectar filhos a um nó-pai e
 * ajustar os ponteiros 'pai' dos filhos.
 */
void conectarFilhos(Sala *pai, Sala *esq, Sala *dir) { // conecta os nós filhos ao pai e ajusta seus ponteiros pai
    if (!pai) return;                // se o ponteiro pai for NULL, não faz nada (proteção)
    pai->esq = esq;                  // atribui o ponteiro do filho esquerdo no nó pai
    pai->dir = dir;                  // atribui o ponteiro do filho direito no nó pai
    if (esq) esq->pai = pai;         // se existir filho esquerdo, define seu ponteiro 'pai' para o nó pai
    if (dir) dir->pai = pai;         // se existir filho direito, define seu ponteiro 'pai' para o nó pai
}

/*
 * liberarSalas - libera recursivamente a memória da árvore de salas.
 */
void liberarSalas(Sala *raiz) {      // função recursiva que libera toda a memória da árvore
    if (!raiz) return;               // caso base: se o nó for NULL, retorna sem fazer nada
    liberarSalas(raiz->esq);         // libera recursivamente a subárvore esquerda
    liberarSalas(raiz->dir);         // libera recursivamente a subárvore direita
    free(raiz->nome);                // libera a string do nome alocada dinamicamente
    free(raiz);                      // libera a própria estrutura Sala
}

/* 
 * get_choice - retorna a primeira letra não-branca da entrada do usuário,
 * ou '\0' se a leitura falhar.
 */
char get_choice(void) {              // lê uma linha do stdin e retorna o primeiro caractere não branco
    char buf[64];                    // buffer temporário para armazenar a linha lida
    if (!fgets(buf, sizeof(buf), stdin)) return '\0'; // se leitura falhar, retorna '\0'
    // procurar primeiro caractere não branco
    for (size_t i = 0; buf[i] != '\0'; ++i) { // percorre o buffer até o fim da string
        if (!isspace((unsigned char)buf[i])) return buf[i]; // retorna o primeiro caractere que não for espaço
    }
    return '\0';                     // se a linha só tiver espaços (ou estiver vazia), retorna '\0'
}

/*
 * explorarSalas - permite a navegação do jogador pela árvore a partir da raiz.
 * Interação por teclado: 'e' = esquerda, 'd' = direita, 'v' = voltar (pai),
 * 's' = encerrar exploração atual.
 *
 * Observações:
 * - Não encerramos automaticamente ao alcançar folha; jogador pode voltar.
 * - A exploração termina apenas quando o jogador escolhe 's'.
 */
void explorarSalas(Sala *raiz) {     // função que implementa a sessão de exploração interativa
    if (!raiz) {                      // se a raiz for NULL, informa que não há mapa
        printf("Mapa vazio. Nada a explorar.\n");
        return;                       // retorna imediatamente
    }

    const int MAX_VISITAS = 1024;     // constante que limita o número de entradas no histórico de visitas
    const char *visitadas[MAX_VISITAS]; // array para guardar nomes das salas visitadas (apenas ponteiros)
    int cont = 0;                     // contador de salas visitadas

    Sala *atual = raiz;               // ponteiro que representa a sala atual do jogador

    printf("\n--- Iniciando exploração da mansão ---\n"); // mensagem de início
    printf("Você começa no Hall de entrada: \"%s\"\n\n", atual->nome); // mostra a primeira sala

    while (1) {                       // laço principal de interação (roda até o jogador encerrar)
        // Registrar visita atual (pode repetir se voltar e visitar de novo)
        if (cont < MAX_VISITAS) visitadas[cont++] = atual->nome; // registra o nome da sala atual no histórico

        printf("Você está na sala: %s\n", atual->nome); // exibe o nome da sala atual

        // Mostrar opções dinamicamente dependendo dos filhos/pai existentes
        printf("Opções:\n");         // cabeçalho das opções
        if (atual->esq) printf("  (e) Ir para a esquerda -> %s\n", atual->esq->nome); // mostra opção esquerda se existir
        if (atual->dir) printf("  (d) Ir para a direita -> %s\n", atual->dir->nome);  // mostra opção direita se existir
        if (atual->pai) printf("  (v) Voltar para a sala anterior -> %s\n", atual->pai->nome); // mostra opção voltar se houver pai
        else printf("  (v) Voltar (não disponível - você está no Hall de entrada)\n"); // informa que voltar não é possível na raiz
        printf("  (s) Encerrar exploração atual e voltar ao menu principal\n"); // opção para sair da exploração
        printf("Escolha (e/d/v/s): "); // prompt para o usuário

        char c = get_choice();        // lê a escolha do usuário (primeiro caractere não-branco)
        if (c == '\0') {              // se leitura falhar ou apenas espaços, encerra a exploração
            printf("\nEntrada finalizada. Retornando ao menu principal.\n");
            break;
        }

        if (c == 's' || c == 'S') {   // se o usuário escolheu 's'
            printf("Encerrando exploração e retornando ao menu principal...\n\n");
            break;                    // sai do loop de exploração e retorna ao menu principal
        } else if ((c == 'e' || c == 'E')) { // opção 'e' para ir à esquerda
            if (atual->esq) {          // se existir filho esquerdo
                atual = atual->esq;    // move o ponteiro atual para a subárvore esquerda
                printf("\n-- Indo para a esquerda... --\n\n"); // feedback para o usuário
            } else {
                printf("Não há sala à esquerda. Tente outra opção.\n\n"); // erro: caminho inexistente
            }
        } else if ((c == 'd' || c == 'D')) { // opção 'd' para ir à direita
            if (atual->dir) {           // se existir filho direito
                atual = atual->dir;     // move o ponteiro atual para a subárvore direita
                printf("\n-- Indo para a direita... --\n\n"); // feedback para o usuário
            } else {
                printf("Não há sala à direita. Tente outra opção.\n\n"); // erro: caminho inexistente
            }
        } else if ((c == 'v' || c == 'V')) { // opção 'v' para voltar ao pai
            if (atual->pai) {           // se o nó atual tiver um pai
                atual = atual->pai;     // move o ponteiro atual para o nó pai
                printf("\n-- Voltando para a sala anterior... --\n\n"); // feedback para o usuário
            } else {
                printf("Você está na raiz (Hall de entrada). Não é possível voltar.\n\n"); // aviso caso esteja na raiz
            }
        } else {
            printf("Opção inválida. Use 'e', 'd', 'v' ou 's'.\n\n"); // opção não reconhecida
        }
    }

    // Exibir percurso
    printf("\n--- Salas visitadas nesta exploração ---\n"); // cabeçalho do histórico exibido
    for (int i = 0; i < cont; ++i) { // percorre o array de nomes armazenados
        printf("%d) %s\n", i + 1, visitadas[i]); // imprime cada sala visitada em ordem
    }
    printf("----------------------------------------\n\n"); // rodapé visual
}

/*
 * montarMapa - cria e retorna o mapa fixo da mansão (árvore binária),
 * e garante que ponteiros 'pai' sejam corretamente atribuídos.
 */
Sala *montarMapa(void) {             // função que monta a árvore estática do mapa
    Sala *hall = criarSala("Hall de entrada"); // cria a raiz do mapa

    // Nível 1
    Sala *salaEstar = criarSala("Sala de estar"); // cria nó 'Sala de estar'
    Sala *biblioteca = criarSala("Biblioteca");   // cria nó 'Biblioteca'
    conectarFilhos(hall, salaEstar, biblioteca);  // conecta salaEstar e biblioteca como filhos do hall

    // Nível 2 - lado esquerdo
    Sala *cozinha = criarSala("Cozinha");         // cria nó 'Cozinha'
    Sala *salaJantar = criarSala("Sala de jantar"); // cria nó 'Sala de jantar'
    conectarFilhos(salaEstar, cozinha, salaJantar);  // conecta cozinha e salaJantar ao nó salaEstar

    // Nível 2 - lado direito
    Sala *escritorio = criarSala("Escritório");   // cria nó 'Escritório'
    Sala *observatorio = criarSala("Observatório"); // cria nó 'Observatório'
    conectarFilhos(biblioteca, escritorio, observatorio); // conecta ao nó biblioteca

    // Nível 3 - folhas (algumas)
    Sala *despensa = criarSala("Despensa");       // cria nó 'Despensa'
    Sala *jardimInterno = criarSala("Jardim interno"); // cria nó 'Jardim interno'
    conectarFilhos(cozinha, despensa, jardimInterno); // conecta ao nó cozinha

    Sala *torre = criarSala("Torre de vigia");    // cria nó 'Torre de vigia'
    // colocar torre como filho direito do observatório
    conectarFilhos(observatorio, NULL, torre);    // conecta torre como filho direito do observatório (filho esquerdo = NULL)

    return hall;                                  // retorna a raiz montada (hall)
}

/*
 * main - exibe o menu principal e permite explorar a mansão repetidamente.
 * A exploração só termina quando o jogador escolhe sair no menu principal.
 */
int main(void) {                   // função principal do programa
    Sala *mapa = montarMapa();      // monta o mapa da mansão e obtém a raiz
    char opcao[16];                 // buffer para leitura da opção do menu

    while (1) {                     // loop do menu principal (repete até o jogador escolher sair)
        printf("=====================================\n"); // cabeçalho do menu
        printf("        DETECTIVE QUEST - MENU       \n"); // título do menu
        printf("=====================================\n"); // divisor visual
        printf("1 - Explorar a mansão\n"); // opção 1: iniciar exploração
        printf("2 - Sair do jogo\n");      // opção 2: encerrar o programa
        printf("Escolha: ");               // prompt para o usuário
        if (!fgets(opcao, sizeof(opcao), stdin)) break; // lê a opção; se falhar, sai do loop

        if (opcao[0] == '1') {        // se o usuário digitou '1' na primeira posição
            explorarSalas(mapa);      // inicia a exploração (retorna ao menu quando terminar)
        } else if (opcao[0] == '2') { // se o usuário digitou '2' na primeira posição
            printf("Saindo do jogo... até a próxima!\n"); // mensagem de despedida
            break;                    // sai do loop principal, fim do programa
        } else {
            printf("Opção inválida! Tente novamente.\n\n"); // aviso para entrada inválida
        }
    }

    liberarSalas(mapa);              // libera toda a memória alocada para o mapa antes de terminar
    return 0;                        // retorna 0 indicando término normal do programa
}
