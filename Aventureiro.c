// TEMA 4 - AVENTUREIRO

#include <stdio.h>      // printf, fgets, fprintf — entrada/saída padrão.
#include <stdlib.h>     // malloc, free, exit — alocação e encerramento.
#include <string.h>     // strlen, memcpy, strcmp — manipulação de strings.
#include <ctype.h>      // isspace — classificação de caracteres (espaços, tabs, newlines).

/* ---------- Estruturas ---------- */

/* Estrutura que representa uma sala (nó da árvore binária) */
typedef struct Sala {          // início da definição do tipo 'struct Sala'
    char *nome;               // ponteiro para string que guarda o nome da sala (alocado dinamicamente)
    char *pista;              // ponteiro para string com a pista opcional (NULL se não houver)
    struct Sala *esq;         // ponteiro para o filho à esquerda (subárvore esquerda)
    struct Sala *dir;         // ponteiro para o filho à direita (subárvore direita)
    struct Sala *pai;         // ponteiro para o nó pai (NULL se for a raiz)
} Sala;                       // typedef para simplificar o uso do tipo como 'Sala'

/* Nó da árvore BST que guarda as pistas coletadas */
typedef struct NoPista {      // início da definição do nó da BST de pistas
    char *texto;              // texto da pista (alocado dinamicamente)
    struct NoPista *esq;      // filho esquerdo (itens "menores" alfabeticamente)
    struct NoPista *dir;      // filho direito (itens "maiores" alfabeticamente)
} NoPista;                    // typedef para usar 'NoPista' diretamente

/* ---------- Funções utilitárias ---------- */

/* duplicar string (substitui strdup para portabilidade) */
static char *str_dup(const char *s) {   // função estática que duplica uma string
    if (!s) return NULL;               // se o argumento for NULL, retorna NULL (proteção)
    size_t n = strlen(s) + 1;          // tamanho necessário incluindo caractere nulo '\0'
    char *r = malloc(n);               // aloca memória para a cópia
    if (!r) {                          // se malloc falhar...
        fprintf(stderr, "Erro: memória insuficiente ao duplicar string.\n"); // informa erro
        exit(EXIT_FAILURE);            // encerra o programa com código de erro
    }
    memcpy(r, s, n);                   // copia os bytes (incluindo '\0') para a nova área
    return r;                          // retorna o ponteiro para a string duplicada
}

/* lê primeira letra não-branca da entrada do usuário; retorna '\0' se falhar */
char get_choice(void) {                // lê uma linha de stdin e retorna primeiro caractere não branco
    char buf[128];                     // buffer temporário para a linha de entrada
    if (!fgets(buf, sizeof(buf), stdin)) return '\0'; // se leitura falhar, retorna '\0'
    for (size_t i = 0; buf[i] != '\0'; ++i) { // percorre até o fim da string lida
        if (!isspace((unsigned char)buf[i])) return buf[i]; // retorna o primeiro caractere não branco
    }
    return '\0';                       // se só houver espaços, retorna '\0'
}

/* ---------- Funções para salas (mapa) ---------- */

/*
 * criarSala - cria, de forma dinâmica, uma sala com o nome informado e pista opcional.
 * Parâmetros:
 *   nome  - nome da sala (string)
 *   pista - pista associada à sala (string) ou NULL para sem pista
 * Retorno: ponteiro para Sala criada (filhos e pai inicializados em NULL)
 */
Sala *criarSala(const char *nome, const char *pista) { // cria uma Sala e inicializa campos
    Sala *s = malloc(sizeof(Sala));    // aloca memória para a estrutura Sala
    if (!s) {                          // verifica se alocação ocorreu com sucesso
        fprintf(stderr, "Erro: memória insuficiente ao criar sala.\n"); // mensagem de erro
        exit(EXIT_FAILURE);            // encerra o programa se falhar alocação
    }
    s->nome = str_dup(nome);           // duplica e armazena o nome da sala
    s->pista = pista ? str_dup(pista) : NULL; // duplica a pista se fornecida, senão NULL
    s->esq = NULL;                     // inicializa filho esquerdo como NULL
    s->dir = NULL;                     // inicializa filho direito como NULL
    s->pai = NULL;                     // inicializa ponteiro para pai como NULL
    return s;                          // retorna o ponteiro para a sala criada
}

/*
 * conectarFilhos - conecta filhos a um nó pai e ajusta ponteiros 'pai' dos filhos.
 */
void conectarFilhos(Sala *pai, Sala *esq, Sala *dir) { // conecta esq/dir a pai e ajusta 'pai' dos filhos
    if (!pai) return;                // proteção: se pai for NULL, não faz nada
    pai->esq = esq;                  // atribui filho esquerdo
    pai->dir = dir;                  // atribui filho direito
    if (esq) esq->pai = pai;         // se filho esquerdo existe, define seu ponteiro para o pai
    if (dir) dir->pai = pai;         // se filho direito existe, define seu ponteiro para o pai
}

/*
 * liberarSalas - libera recursivamente a memória da árvore de salas,
 * incluindo strings de nome e pista.
 */
void liberarSalas(Sala *raiz) {      // libera recursivamente todas as salas da árvore
    if (!raiz) return;               // caso base: nó NULL -> nada a fazer
    liberarSalas(raiz->esq);         // libera subárvore esquerda
    liberarSalas(raiz->dir);         // libera subárvore direita
    free(raiz->nome);                // libera string do nome alocada
    if (raiz->pista) free(raiz->pista); // libera string da pista, se existir
    free(raiz);                      // libera a estrutura Sala em si
}

/* ---------- Funções para a árvore de pistas (BST) ---------- */

/*
 * criarNoPista - cria um nó de BST para armazenar uma pista (texto).
 */
NoPista *criarNoPista(const char *texto) { // cria e inicializa um NoPista com a string fornecida
    if (!texto) return NULL;          // proteção: não cria se texto for NULL
    NoPista *n = malloc(sizeof(NoPista)); // aloca memória para o nó
    if (!n) {                         // verifica sucesso da alocação
        fprintf(stderr, "Erro: memória insuficiente ao criar nó de pista.\n");
        exit(EXIT_FAILURE);           // encerra em caso de falha
    }
    n->texto = str_dup(texto);        // duplica e guarda o texto da pista
    n->esq = NULL;                    // inicializa filho esquerdo
    n->dir = NULL;                    // inicializa filho direito
    return n;                         // retorna o nó criado
}

/*
 * inserirPista - insere uma pista na BST em ordem alfabética.
 * Se a pista já existir (strcmp == 0), não insere duplicata.
 * Retorna a raiz (possivelmente alterada) da BST.
 */
NoPista *inserirPista(NoPista *raiz, const char *texto) { // insere texto na BST mantendo ordem
    if (!texto) return raiz;          // se texto inválido, retorna raiz sem alteração
    if (!raiz) {                      // se árvore vazia, cria novo nó como raiz
        return criarNoPista(texto);
    }
    int cmp = strcmp(texto, raiz->texto); // compara alfabeticamente com nó atual
    if (cmp == 0) {                    // se igual -> já existe, não insere duplicata
        return raiz;
    } else if (cmp < 0) {              // se texto < raiz->texto -> vai para subárvore esquerda
        raiz->esq = inserirPista(raiz->esq, texto);
    } else {                           // se texto > raiz->texto -> vai para subárvore direita
        raiz->dir = inserirPista(raiz->dir, texto);
    }
    return raiz;                       // retorna a raiz (inalterada na maioria dos casos)
}

/*
 * exibirPistas - imprime as pistas em ordem alfabética (percorrendo BST em-order).
 */
void exibirPistas(NoPista *raiz) {    // percorre a BST em ordem e imprime cada pista
    if (!raiz) return;                // caso base: nó NULL -> retorna
    exibirPistas(raiz->esq);          // visita recursivamente a subárvore esquerda (menores)
    printf(" - %s\n", raiz->texto);   // imprime o texto da pista do nó atual
    exibirPistas(raiz->dir);          // visita recursivamente a subárvore direita (maiores)
}

/*
 * liberarPistas - libera recursivamente a BST de pistas e suas strings.
 */
void liberarPistas(NoPista *raiz) {   // libera toda a BST de pistas
    if (!raiz) return;                // caso base: nó NULL -> nada a fazer
    liberarPistas(raiz->esq);         // libera subárvore esquerda
    liberarPistas(raiz->dir);         // libera subárvore direita
    free(raiz->texto);                // libera string do texto
    free(raiz);                       // libera estrutura NoPista
}

/* ---------- Exploração: coleta de pistas ---------- */

/*
 * explorarSalasComPistas - permite a navegação do jogador pela árvore a partir da raiz.
 * Quando o jogador entra em uma sala que contém pista, a pista é inserida automaticamente
 * na árvore de pistas (BST passada por referência).
 *
 * Comandos:
 *   e - esquerda
 *   d - direita
 *   v - voltar (pai)
 *   s - encerrar exploração atual
 *
 * Parâmetros:
 *   raiz     - raiz do mapa (Sala*)
 *   pistas   - ponteiro para a raiz da BST de pistas coletadas (NoPista**).
 *             A função atualiza *pistas para incluir as novas pistas.
 */
void explorarSalasComPistas(Sala *raiz, NoPista **pistas) { // inicia sessão de exploração com coleta de pistas
    if (!raiz) {                      // se o mapa for vazio, informa e retorna
        printf("Mapa vazio. Nada a explorar.\n");
        return;
    }

    const int MAX_VISITAS = 1024;     // limite para histórico de visitas (prevenção de overflow)
    const char *visitadas[MAX_VISITAS]; // array de ponteiros para nomes das salas visitadas
    int cont = 0;                     // contador de visitas registradas

    Sala *atual = raiz;               // sala atual (inicia na raiz)
    printf("\n--- Iniciando exploração da mansão (coleta de pistas) ---\n"); // cabeçalho
    printf("Você começa no Hall de entrada: \"%s\"\n\n", atual->nome); // mostra a sala inicial

    // inserir pista da sala inicial (se houver)
    if (atual->pista) {               // se a sala inicial tem pista...
        *pistas = inserirPista(*pistas, atual->pista); // insere na BST de pistas
        printf("[Pista encontrada] %s\n\n", atual->pista); // informa o jogador
    }

    while (1) {                       // loop principal da exploração (até 's' ser escolhido)
        if (cont < MAX_VISITAS) visitadas[cont++] = atual->nome; // registra visita atual

        printf("Você está na sala: %s\n", atual->nome); // informa a sala atual

        // mostrar opções dinâmicas dependendo de filhos/pai
        printf("Opções:\n");
        if (atual->esq) printf("  (e) Ir para a esquerda -> %s\n", atual->esq->nome); // mostra esquerda se existir
        if (atual->dir) printf("  (d) Ir para a direita -> %s\n", atual->dir->nome);  // mostra direita se existir
        if (atual->pai) printf("  (v) Voltar para a sala anterior -> %s\n", atual->pai->nome); // mostra voltar se houver pai
        else printf("  (v) Voltar (não disponível - você está no Hall de entrada)\n"); // senão, informa indisponibilidade
        printf("  (s) Encerrar exploração atual e mostrar pistas coletadas\n"); // opção para encerrar
        printf("Escolha (e/d/v/s): "); // prompt para o usuário

        char c = get_choice();        // lê a escolha (primeiro caractere não branco)
        if (c == '\0') {              // se leitura falhar ou só espaços
            printf("\nEntrada finalizada. Retornando ao menu principal.\n");
            break;
        }

        if (c == 's' || c == 'S') {   // encerrar exploração
            printf("Encerrando exploração e compilando pistas...\n\n");
            break;                    // sai do loop e volta ao menu
        } else if ((c == 'e' || c == 'E')) { // ir para a esquerda
            if (atual->esq) {          // se há sala à esquerda
                atual = atual->esq;    // atualiza ponteiro para a sala à esquerda
                printf("\n-- Indo para a esquerda... --\n\n");
                if (atual->pista) {   // se a nova sala contém pista
                    *pistas = inserirPista(*pistas, atual->pista); // insere na BST
                    printf("[Pista encontrada] %s\n\n", atual->pista); // informa o jogador
                }
            } else {
                printf("Não há sala à esquerda. Tente outra opção.\n\n"); // caminho inexistente
            }
        } else if ((c == 'd' || c == 'D')) { // ir para a direita
            if (atual->dir) {           // se há sala à direita
                atual = atual->dir;     // atualiza ponteiro para a sala à direita
                printf("\n-- Indo para a direita... --\n\n");
                if (atual->pista) {     // se a nova sala contém pista
                    *pistas = inserirPista(*pistas, atual->pista); // insere na BST
                    printf("[Pista encontrada] %s\n\n", atual->pista); // informa o jogador
                }
            } else {
                printf("Não há sala à direita. Tente outra opção.\n\n"); // caminho inexistente
            }
        } else if ((c == 'v' || c == 'V')) { // voltar ao pai
            if (atual->pai) {           // se existe nó pai
                atual = atual->pai;     // atualiza para o nó pai
                printf("\n-- Voltando para a sala anterior... --\n\n");
            } else {
                printf("Você está na raiz (Hall de entrada). Não é possível voltar.\n\n"); // aviso na raiz
            }
        } else {
            printf("Opção inválida. Use 'e', 'd', 'v' ou 's'.\n\n"); // entrada inválida
        }
    }

    // exibir percurso
    printf("\n--- Salas visitadas nesta exploração ---\n");
    for (int i = 0; i < cont; ++i) {     // percorre histórico de visitas
        printf("%d) %s\n", i + 1, visitadas[i]); // imprime cada sala visitada
    }
    printf("----------------------------------------\n\n");
}

/* ---------- Mapa da mansão (com pistas) ---------- */

/*
 * montarMapaComPistas - cria e retorna o mapa fixo da mansão (árvore binária),
 * com pistas associadas a algumas salas.
 */
Sala *montarMapaComPistas(void) {     // monta a árvore do mapa com pistas em algumas salas
    // criar nós com nome e pista (pista = NULL se não houver)
    Sala *hall = criarSala("Hall de entrada", "Uma luva de couro com sangue seco"); // raiz com pista

    Sala *salaEstar = criarSala("Sala de estar", "Vidro quebrado perto do lareira"); // nó com pista
    Sala *biblioteca = criarSala("Biblioteca", NULL); // sem pista

    Sala *cozinha = criarSala("Cozinha", "Pegadas molhadas levando à despensa"); // nó com pista
    Sala *salaJantar = criarSala("Sala de jantar", "Uma vela apagada com cera vermelha"); // nó com pista

    Sala *escritorio = criarSala("Escritório", "Um bilhete amassado com iniciais 'R.M.'"); // pista
    Sala *observatorio = criarSala("Observatório", "Lentes riscada e uma gota de óleo"); // pista

    Sala *despensa = criarSala("Despensa", "Caixa vazia de comprimidos"); // folha com pista
    Sala *jardimInterno = criarSala("Jardim interno", NULL); // sem pista
    Sala *torre = criarSala("Torre de vigia", "Pegada solitária no corrimão"); // folha com pista

    // conectar árvore e ajustar ponteiros 'pai'
    conectarFilhos(hall, salaEstar, biblioteca);       // hall -> (salaEstar, biblioteca)
    conectarFilhos(salaEstar, cozinha, salaJantar);    // salaEstar -> (cozinha, salaJantar)
    conectarFilhos(biblioteca, escritorio, observatorio); // biblioteca -> (escritorio, observatorio)
    conectarFilhos(cozinha, despensa, jardimInterno); // cozinha -> (despensa, jardimInterno)
    conectarFilhos(observatorio, NULL, torre);         // observatorio -> (NULL, torre)

    return hall;                                      // retorna a raiz do mapa montado
}

/* ---------- Função principal (menu) ---------- */

int main(void) {                   // função principal do programa
    Sala *mapa = montarMapaComPistas();    // monta o mapa com pistas já associadas
    char opcao[16];                 // buffer para leitura da opção do menu

    while (1) {                     // loop do menu principal (repete até escolher sair)
        printf("=====================================\n"); // cabeçalho do menu
        printf("        DETECTIVE QUEST - MENU       \n"); // título
        printf("=====================================\n"); // divisor visual
        printf("1 - Explorar a mansão (coletar pistas)\n"); // opção 1: explorar e coletar pistas
        printf("2 - Sair do jogo\n");      // opção 2: encerrar o programa
        printf("Escolha: ");               // prompt para o usuário
        if (!fgets(opcao, sizeof(opcao), stdin)) break; // leitura da opção; se falhar, sai do loop

        if (opcao[0] == '1') {        // se o usuário escolheu '1'
            NoPista *arvorePistas = NULL;    // BST vazia para armazenar pistas desta exploração
            explorarSalasComPistas(mapa, &arvorePistas); // inicia exploração e coleta pistas

            // mostrar pistas coletadas em ordem alfabética
            printf("Pistas coletadas (em ordem alfabética):\n");
            if (!arvorePistas) {     // se nenhuma pista foi coletada
                printf(" (nenhuma pista encontrada nesta exploração)\n");
            } else {
                exibirPistas(arvorePistas); // imprime a BST em ordem (alfabética)
            }
            printf("\n");

            // liberar a árvore de pistas desta exploração (libera memória)
            liberarPistas(arvorePistas);
        } else if (opcao[0] == '2') { // se o usuário escolheu '2'
            printf("Saindo do jogo... até a próxima!\n"); // mensagem de despedida
            break;                    // sai do loop principal, encerra o programa
        } else {
            printf("Opção inválida! Tente novamente.\n\n"); // aviso para entrada inválida
        }
    }

    liberarSalas(mapa);              // libera todo o mapa da mansão e pistas associadas
    return 0;                        // retorna 0 indicando término normal
}
