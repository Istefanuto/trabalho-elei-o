
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#define MINIMO_VOTOS_SEGUNDO_TURNO 10
#define MINIMO_ELEITORES_SEGUNDO_TURNO 10


struct Chapa {
    char nomeCandidato[50];
    int numeroDeVoto;
    char dataNascimento[11];
    char nomeVice[50];
    int votosRecebidos;
    struct Chapa* proximo;
};

typedef struct Chapa chapa;

void cadastrarChapa(chapa **inicio) {
    chapa *novaChapa = (chapa *)malloc(sizeof(chapa));

    printf("Nome do Candidato [max 50 caracteres]: ");
    getchar();
    fgets(novaChapa->nomeCandidato, sizeof(novaChapa->nomeCandidato), stdin);
    novaChapa->nomeCandidato[strcspn(novaChapa->nomeCandidato, "\n")] = 0; // Remove a nova linha

    int numeroDuplicado = 0; 
    do {
        printf("Numero de voto do candidato [de 1 a 99]: ");
        scanf("%d", &novaChapa->numeroDeVoto);

        if (novaChapa->numeroDeVoto < 1 || novaChapa->numeroDeVoto > 99) {
            printf("Numero de voto invalido [somente valido numeros de 1 a 99].\n");
            continue;
        }

        chapa *atual = *inicio;
        numeroDuplicado = 0;
        while (atual != NULL) {
            if (atual->numeroDeVoto == novaChapa->numeroDeVoto) {
                numeroDuplicado = 1;
                break; 
            }
            atual = atual->proximo;
        }

        if (numeroDuplicado) {
            printf("Esse numero de candidato ja esta sendo usado. Por favor, escolha outro.\n");
        }

    } while (numeroDuplicado || novaChapa->numeroDeVoto < 1 || novaChapa->numeroDeVoto > 99);

    printf("Data de nascimento [DD/MM/AA]: ");
    scanf("%s", novaChapa->dataNascimento);

    printf("Nome do vice [max 50 caracteres]: ");
    getchar(); 
    fgets(novaChapa->nomeVice, sizeof(novaChapa->nomeVice), stdin);
    novaChapa->nomeVice[strcspn(novaChapa->nomeVice, "\n")] = 0;

    
    novaChapa->votosRecebidos = 0;  

    novaChapa->proximo = *inicio;
    *inicio = novaChapa;
}


void exibirChapas(chapa *inicio) {
    chapa *atual = inicio;
    printf("\nChapas cadastradas:\n");
    while (atual != NULL) {
        printf("Candidato: %s, Nº: %d, Data de Nascimento: %s, Vice: %s\n",
               atual->nomeCandidato, atual->numeroDeVoto,
               atual->dataNascimento, atual->nomeVice);
        atual = atual->proximo;
    }
}

void opcaoVoto(chapa *inicio) {
    chapa *atual = inicio;
    printf("\nOPCOES PARA VOTO\n");
    while (atual != NULL) {
        printf("CANDIDATO: %s, NUMERO: %d\n", atual->nomeCandidato, atual->numeroDeVoto);
        atual = atual->proximo;
    }
}

void votar(chapa *inicio, int *votosNulos, int *votosBrancos, int *votosValidos) {
    int voto;
    printf("Digite o numero do candidato que deseja votar ou [0 para votar em branco/ qualquer numero sem cadastro para nulo]: ");
    scanf("%d", &voto);
    
    if (voto == 0) {
        (*votosBrancos)++;
        printf("Seu voto em branco foi confirmado.\n");
    } else {
        chapa *atual = inicio;
        int votoValido = 0;
        while (atual != NULL) {
            if (atual->numeroDeVoto == voto) {
                atual->votosRecebidos++;
                votoValido = 1;
                (*votosValidos)++; 
                printf("Seu voto foi computado para %s\n", atual->nomeCandidato);
                break; 
            }
            atual = atual->proximo;
        }
        if (!votoValido) {
            (*votosNulos)++;
            printf("Voto nulo registrado\n");
        }    
    }    
}


void exibirResultados(chapa *inicio, int votosNulos, int votosBrancos) {
    chapa *atual = inicio;
    printf("\nRESULTADOS DA VOTACAO:\n");
    while (atual != NULL) {
        printf("Candidato: %s, Nº: %d, Votos recebidos: %d\n",
               atual->nomeCandidato, atual->numeroDeVoto, atual->votosRecebidos);
        atual = atual->proximo;
    }
    printf("Votos em branco: %d\n", votosBrancos);
    printf("Votos nulos: %d\n", votosNulos);
}

void liberarLista(chapa *inicio) {
    chapa *atual = inicio;
    while (atual != NULL) {
        chapa *temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}


void boletimPrimeiroTurno(chapa *inicio, int votosNulos, int votosBrancos, int votosValidos) {
    FILE *arquivo = fopen("boletimPrimeiroTurno.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    chapa *atual = inicio;
     fprintf(arquivo, "\n------------BOLETIM PRIMEIRO TURNO------------\n");

    while (atual != NULL) {
        fprintf(arquivo, "Candidato: %s, Nº: %d \n Votos recebidos: %d\n",
                atual->nomeCandidato, atual->numeroDeVoto, atual->votosRecebidos);
        atual = atual->proximo;
        fprintf(arquivo, "\n----------------------------------------------\n");
    }

    int votosTotais = votosValidos + votosBrancos + votosNulos;

  
    float porcentagemVotosBrancos = (votosBrancos > 0) ? ((float)votosBrancos / votosTotais) * 100 : 0.0;
    float porcentagemVotosNulos = (votosNulos > 0) ? ((float)votosNulos / votosTotais) * 100 : 0.0;
    float porcentagemVotosValidos = (votosValidos > 0) ? ((float)votosValidos / votosTotais) * 100 : 0.0;

    
   
    fprintf(arquivo, "Total de votos válidos: %d  ----------------Porcentagem: %.2f%%\n", votosValidos, porcentagemVotosValidos);
    fprintf(arquivo, "Total de votos brancos: %d  ----------------Porcentagem: %.2f%%\n", votosBrancos, porcentagemVotosBrancos);
    fprintf(arquivo, "Total de votos nulos: %d    ----------------Porcentagem: %.2f%%\n", votosNulos, porcentagemVotosNulos);
    

    fclose(arquivo);
    printf("Boletim salvo em 'boletimPrimeiroTurno.txt'.\n");
}

void exibirVencedorSegundoTurno(chapa *chapa1, chapa *chapa2) {
    if (chapa1->votosRecebidos > chapa2->votosRecebidos) {
        printf("\nO vencedor do segundo turno é: %s com %d votos!\n", chapa1->nomeCandidato, chapa1->votosRecebidos);
    } else if (chapa2->votosRecebidos > chapa1->votosRecebidos) {
        printf("\nO vencedor do segundo turno é: %s com %d votos!\n", chapa2->nomeCandidato, chapa2->votosRecebidos);
    } else {
        printf("\nEmpate no segundo turno entre %s e %s!\n", chapa1->nomeCandidato, chapa2->nomeCandidato);
    }
}

void boletimSegundoTurno(chapa *chapa1, chapa *chapa2, int votosNulos, int votosBrancos, int votosValidos) {
    FILE *arquivo = fopen("boletimSegundoTurno.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    
    fprintf(arquivo, "\n------------BOLETIM SEGUNDO TURNO------------\n");
    fprintf(arquivo, "Candidato 1: %s, Nº: %d, Votos recebidos: %d\n", chapa1->nomeCandidato, chapa1->numeroDeVoto, chapa1->votosRecebidos);
    fprintf(arquivo,"\n-----------------------------------------------\n");
    fprintf(arquivo, "Candidato 2: %s, Nº: %d, Votos recebidos: %d\n", chapa2->nomeCandidato, chapa2->numeroDeVoto, chapa2->votosRecebidos);
    fprintf(arquivo,"\n-----------------------------------------------\n");
    
    int votosTotais = votosValidos + votosBrancos + votosNulos;

    float porcentagemVotosBrancos = (votosBrancos > 0) ? ((float)votosBrancos / votosTotais) * 100 : 0.0;
    float porcentagemVotosNulos = (votosNulos > 0) ? ((float)votosNulos / votosTotais) * 100 : 0.0;
    float porcentagemVotosValidos = (votosValidos > 0) ? ((float)votosValidos / votosTotais) * 100 : 0.0;
    
    fprintf(arquivo, "Total de votos válidos: %d ------------------Porventagem: %f\n", votosValidos,porcentagemVotosValidos);
    fprintf(arquivo, "Total de votos brancos: %d ------------------Porcentagem: %f\n", votosBrancos,porcentagemVotosBrancos);
    fprintf(arquivo, "Total de votos nulos: %d   ------------------Porcentagem: %f\n", votosNulos,porcentagemVotosNulos);

    float porcentagemVotosChapa1 = (votosValidos > 0) ? ((float)chapa1->votosRecebidos / votosValidos) * 100 : 0.0;
    float porcentagemVotosChapa2 = (votosValidos > 0) ? ((float)chapa2->votosRecebidos / votosValidos) * 100 : 0.0;


    fprintf(arquivo, "Porcentagem de votos válidos da Chapa 1: %.2f%%\n", porcentagemVotosChapa1);
    fprintf(arquivo, "Porcentagem de votos válidos da Chapa 2: %.2f%%\n", porcentagemVotosChapa2);

    if (chapa1->votosRecebidos > chapa2->votosRecebidos) {
        fprintf(arquivo, "\nO vencedor do segundo turno é %s com %d votos!\n", chapa1->nomeCandidato, chapa1->votosRecebidos);
    } else if (chapa2->votosRecebidos > chapa1->votosRecebidos) {
        fprintf(arquivo, "\nO vencedor do segundo turno é %s com %d votos!\n", chapa2->nomeCandidato, chapa2->votosRecebidos);
    } else {
        fprintf(arquivo, "\nHouve um empate entre %s e %s!\n", chapa1->nomeCandidato, chapa2->nomeCandidato);
    }

    
    fclose(arquivo);
    printf("Boletim do segundo turno salvo em 'boletimSegundoTurno.txt'.\n");
}


void opcaoVotoSegundoTurno(chapa *chapa1, chapa *chapa2) {
    printf("\nOPCOES PARA VOTO NO SEGUNDO TURNO\n");
    printf("CANDIDATO: %s, NUMERO: %d\n", chapa1->nomeCandidato, chapa1->numeroDeVoto);
    printf("CANDIDATO: %s, NUMERO: %d\n", chapa2->nomeCandidato, chapa2->numeroDeVoto);
    printf("Voto em branco\n");
    printf("Voto nulo\n");
}

void votarSegundoTurno(chapa *chapa1, chapa *chapa2, int *votosNulos, int *votosBrancos, int *votosValidos) {
    int voto;
    
    printf("Digite o numero do candidato, 0 para voto em branco ou qualquer numero n cadastrado para voto nulo: ");
    scanf("%d", &voto);

    if (voto == chapa1->numeroDeVoto) {
        chapa1->votosRecebidos++;
        printf("Voto computado para %s\n", chapa1->nomeCandidato);
        (*votosValidos)++;
    } else if (voto == chapa2->numeroDeVoto) {
        chapa2->votosRecebidos++;
        printf("Voto computado para %s\n", chapa2->nomeCandidato);
        (*votosValidos)++;
    } else if (voto == 0) {
        (*votosBrancos)++;
         printf("Voto em branco computado.\n");

    } else {
        printf("Voto nulo computado!\n");
        (*votosNulos)++;
    }
}

void verificarSegundoTurno(chapa *inicio, int numeroEleitores) {
    if (inicio == NULL || inicio->proximo == NULL) {
        printf("Não há segundo turno.\n");
        return; 
    } 

    chapa *chapa1 = NULL;  
    chapa *chapa2 = NULL; 
    chapa *atual = inicio;
    
    int votosTotais = 0;
    
    while (atual != NULL) {
        votosTotais += atual->votosRecebidos;
        atual = atual->proximo;
    }

    atual = inicio;
    while (atual != NULL) {
        if (chapa1 == NULL || atual->votosRecebidos > chapa1->votosRecebidos) {
            chapa2 = chapa1;
            chapa1 = atual;
        } else if (chapa2 == NULL || atual->votosRecebidos > chapa2->votosRecebidos) {
            chapa2 = atual;
        }
        atual = atual->proximo;
    }

    if (chapa1 == NULL || chapa2 == NULL) {
        printf("Não há candidatos suficientes para um segundo turno.\n");
        return;
    }

    
    float porcentagemChapa1 = ((float)chapa1->votosRecebidos / votosTotais) * 100;
    float porcentagemChapa2 = ((float)chapa2->votosRecebidos / votosTotais) * 100;

    if (porcentagemChapa1 > 50.0) {
        printf("A Chapa 1 (%s) venceu no primeiro turno com %.2f%% dos votos válidos.\n", chapa1->nomeCandidato, porcentagemChapa1);
        return;
    }

    if (porcentagemChapa1 == porcentagemChapa2) {
        printf("Houve um empate entre %s e %s! Iniciando o segundo turno.\n", chapa1->nomeCandidato, chapa2->nomeCandidato);
    } else {
        printf("\nIniciando o segundo turno entre %s e %s!\n", chapa1->nomeCandidato, chapa2->nomeCandidato);
    }


    int votosNulos = 0, votosBrancos = 0, votosValidos = 0;
    
    for (int i = 0; i < numeroEleitores; i++) {
        opcaoVotoSegundoTurno(chapa1, chapa2);  
        votarSegundoTurno(chapa1, chapa2, &votosNulos, &votosBrancos, &votosValidos);  
        printf("\n");
        sleep(2);  
        limparTerminal();  
    }

    exibirResultados(inicio, votosNulos, votosBrancos); 
    boletimSegundoTurno(chapa1, chapa2, votosNulos, votosBrancos, votosValidos);  
    exibirVencedorSegundoTurno(chapa1, chapa2);  
}


void limparTerminal() {
#ifdef _WIN32
    system("cls");
#else
    system("clear"); 
#endif
}

int main() {
    int numeroEleitores, numeroChapas;
    int votosNulos = 0; int votosBrancos =0; int votosValidos=0;
    chapa * chapa1;
    chapa * chapa2;

    FILE *arquivo = fopen("boletimPrimeiroTurno.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 1;
    }

    printf("Qual o numero de eleitores dessa cidade? ");
    scanf("%d", &numeroEleitores);

    printf("Qual o numero de chapas cadastradas nessa cidade? ");
    scanf("%d", &numeroChapas);
    limparTerminal();
    
    chapa *inicio = NULL; 
    char continuar;
    int chapasCadastradas = 0;

    do {
        if (chapasCadastradas >= numeroChapas) {
            printf("O numero de chapas cadastradas foi atingido.\n");
            break; 
        } else {
            cadastrarChapa(&inicio); 
            chapasCadastradas++; 
            
            printf("Deseja cadastrar outra chapa? (s/n): ");
            scanf(" %c", &continuar); 
            limparTerminal();
        }
    } while (continuar == 's' || continuar == 'S');

    exibirChapas(inicio); 
    sleep(5);
    limparTerminal();
    
printf("\n");

printf("A ELEICAO VAI COMECAR !!! \n");

    for(int i =0; i< numeroEleitores;i++){
        opcaoVoto(inicio);
        printf("\n");
        votar(inicio, &votosNulos, &votosBrancos,&votosValidos);
        sleep(2);
        limparTerminal();
    }


exibirResultados(inicio,votosNulos,votosBrancos);
sleep(2);

 boletimPrimeiroTurno(inicio, votosNulos, votosBrancos, votosValidos);

  if (votosValidos >= MINIMO_ELEITORES_SEGUNDO_TURNO) {
        verificarSegundoTurno(inicio, numeroEleitores);
    } else {
        printf("Não haverá segundo turno.\n");
    }

 limparTerminal();
 
exibirVencedorSegundoTurno(chapa1,chapa2);

limparTerminal();


liberarLista(inicio); 
fclose(arquivo);

    return 0;
    
}