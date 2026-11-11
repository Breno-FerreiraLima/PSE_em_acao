#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ACOES 100
#define TAM 100

typedef struct {
    int codigo;
    char escola[TAM];
    char tema[TAM];
    char data[TAM];
    char publico[TAM];
    char responsavel[TAM];
    int previstos;
    int participantes;
    int situacao; /* 1 = planejada, 2 = realizada, 3 = cancelada */
} Acao;

Acao acoes[MAX_ACOES];
int totalAcoes = 0;

void lerTexto(const char *mensagem, char *destino, int tamanho) {
    int valido = 0;
    while (!valido) {
        printf("%s", mensagem);
        if (fgets(destino, tamanho, stdin) == NULL) {
            destino[0] = '\0';
            return;
        }
        destino[strcspn(destino, "\n")] = '\0';
        if (strlen(destino) == 0) {
            printf("Campo obrigatorio. Tente novamente.\n");
        } else {
            valido = 1;
        }
    }
}

int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char linha[80], *fim;
    long valor;
    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) return minimo;
        valor = strtol(linha, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (fim == linha || *fim != '\0' || valor < minimo || valor > maximo) {
            printf("Valor invalido. Informe um numero entre %d e %d.\n", minimo, maximo);
        } else {
            return (int)valor;
        }
    }
}

int buscarIndiceCodigo(int codigo) {
    int i;
    for (i = 0; i < totalAcoes; i++) {
        if (acoes[i].codigo == codigo) return i;
    }
    return -1;
}

const char *nomeSituacao(int situacao) {
    if (situacao == 1) return "Planejada";
    if (situacao == 2) return "Realizada";
    return "Cancelada";
}

void cadastrarAcao(void) {
    Acao nova;
    if (totalAcoes >= MAX_ACOES) {
        printf("\nLimite de armazenamento atingido (%d acoes).\n", MAX_ACOES);
        return;
    }

    printf("\n=== CADASTRO DE ACAO ===\n");
    nova.codigo = lerInteiro("Codigo (maior que zero): ", 1, 999999);
    while (buscarIndiceCodigo(nova.codigo) != -1) {
        printf("Esse codigo ja esta cadastrado. Escolha outro.\n");
        nova.codigo = lerInteiro("Novo codigo: ", 1, 999999);
    }

    lerTexto("Escola: ", nova.escola, TAM);
    lerTexto("Tema da acao: ", nova.tema, TAM);
    lerTexto("Data prevista (DD/MM/AAAA): ", nova.data, TAM);
    lerTexto("Publico-alvo: ", nova.publico, TAM);
    lerTexto("Responsavel: ", nova.responsavel, TAM);
    nova.previstos = lerInteiro("Quantidade prevista de participantes: ", 0, 1000000);
    nova.participantes = 0;
    nova.situacao = 1;

    acoes[totalAcoes] = nova;
    totalAcoes++;
    printf("Acao cadastrada com sucesso!\n");
}

void exibirAcao(const Acao *a) {
    printf("\nCodigo: %d\nEscola: %s\nTema: %s\nData prevista: %s\n"
           "Publico-alvo: %s\nResponsavel: %s\nParticipantes previstos: %d\n"
           "Situacao: %s\n",
           a->codigo, a->escola, a->tema, a->data, a->publico,
           a->responsavel, a->previstos, nomeSituacao(a->situacao));
    if (a->situacao == 2) printf("Participantes efetivos: %d\n", a->participantes);
}

void listarAcoes(void) {
    int i;
    printf("\n=== ACOES CADASTRADAS ===\n");
    if (totalAcoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }
    for (i = 0; i < totalAcoes; i++) {
        printf("\n--- Acao %d de %d ---", i + 1, totalAcoes);
        exibirAcao(&acoes[i]);
    }
}

void pesquisarAcoes(void) {
    int opcao, codigo, i, encontrou = 0;
    char termo[TAM], campo[TAM];
    printf("\n=== PESQUISA DE ACOES ===\n");
    printf("1 - Por codigo\n2 - Por escola\n3 - Por tema\n");
    opcao = lerInteiro("Escolha: ", 1, 3);

    if (opcao == 1) {
        codigo = lerInteiro("Informe o codigo: ", 1, 999999);
        i = buscarIndiceCodigo(codigo);
        if (i >= 0) { exibirAcao(&acoes[i]); encontrou = 1; }
    } else {
        lerTexto(opcao == 2 ? "Informe o nome da escola: " : "Informe o tema: ", termo, TAM);
        for (i = 0; i < totalAcoes; i++) {
            strcpy(campo, opcao == 2 ? acoes[i].escola : acoes[i].tema);
            if (strstr(campo, termo) != NULL) {
                exibirAcao(&acoes[i]);
                encontrou = 1;
            }
        }
    }
    if (!encontrou) printf("Nenhuma acao encontrada para o criterio informado.\n");
}

void atualizarSituacao(void) {
    int codigo, indice, situacao;
    printf("\n=== ATUALIZACAO DE SITUACAO ===\n");
    if (totalAcoes == 0) { printf("Nao ha acoes cadastradas.\n"); return; }
    codigo = lerInteiro("Codigo da acao: ", 1, 999999);
    indice = buscarIndiceCodigo(codigo);
    if (indice == -1) { printf("Registro inexistente.\n"); return; }

    printf("1 - Planejada\n2 - Realizada\n3 - Cancelada\n");
    situacao = lerInteiro("Nova situacao: ", 1, 3);
    acoes[indice].situacao = situacao;
    if (situacao == 2) {
        acoes[indice].participantes = lerInteiro(
            "Quantidade efetiva de participantes: ", 0, 1000000);
    } else {
        acoes[indice].participantes = 0;
    }
    printf("Situacao atualizada para %s.\n", nomeSituacao(situacao));
}

void gerarResumo(void) {
    int i, planejadas = 0, realizadas = 0, canceladas = 0;
    long previstos = 0, participantes = 0;
    double percentual;
    for (i = 0; i < totalAcoes; i++) {
        previstos += acoes[i].previstos;
        if (acoes[i].situacao == 1) planejadas++;
        else if (acoes[i].situacao == 2) {
            realizadas++;
            participantes += acoes[i].participantes;
        } else canceladas++;
    }
    percentual = previstos > 0 ? (100.0 * participantes / previstos) : 0.0;

    printf("\n=== RESUMO GERAL ===\n");
    printf("Total de acoes cadastradas: %d\n", totalAcoes);
    printf("Acoes planejadas: %d\n", planejadas);
    printf("Acoes realizadas: %d\n", realizadas);
    printf("Acoes canceladas: %d\n", canceladas);
    printf("Total de participantes previstos: %ld\n", previstos);
    printf("Total de participantes efetivos: %ld\n", participantes);
    printf("Participacao em relacao ao previsto: %.2f%%\n", percentual);
}

void exibirMenu(void) {
    printf("\n========== PSE EM ACAO ==========\n");
    printf("1 - Cadastrar acao\n");
    printf("2 - Listar acoes\n");
    printf("3 - Pesquisar acoes\n");
    printf("4 - Atualizar situacao / participantes\n");
    printf("5 - Gerar resumo geral\n");
    printf("0 - Encerrar\n");
}

int main(void) {
    int opcao;
    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 5);
        switch (opcao) {
            case 1: cadastrarAcao(); break;
            case 2: listarAcoes(); break;
            case 3: pesquisarAcoes(); break;
            case 4: atualizarSituacao(); break;
            case 5: gerarResumo(); break;
            case 0: printf("Programa encerrado. Obrigado!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
    return 0;
}
