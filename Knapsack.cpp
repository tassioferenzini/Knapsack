/*
---Comando para compilar com o gcc deve-se executar o seguinte comando:

gcc Knapsack.cpp -o Knapsack

---Comando para rodar as instancias dos problemos fornecidos:

./Knapsack "path_instance" "question"

---por exemplo, para executar a instancia "Data-1000-Q4.txt" da questão 2:

./Knapsack Data-1000-Q4.txt 2

---O output do programa é o arquivo "Output.txt" que é gerado na pasta onde está o executável.

---P.S: Em algumas versões do Ubuntu há uma restrição para não se usar todos os recursos de memória do computador. Então para ter acesso completo aa memória é necessário rodar o seguinte comando:

ulimit -s unlimited
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int numberItens_ = -1;
int knapsackSize_ = -1;

struct item {
    unsigned int id;
    int weight;
    int value;
    float rate; //razão entre value e weight.
    float fractionItem;
    int *conflicts;
    int numberConflicts;
};

struct knapsack {
    int numberItens;
    int weightItens;
    float valueItens;
    struct item *itens;
    int heuristic;
};

void heapifyNumberConflictsMin(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].numberConflicts > a[left].numberConflicts) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].numberConflicts > a[left].numberConflicts || a[index].numberConflicts > a[right].numberConflicts) {
        if (a[left].numberConflicts > a[right].numberConflicts) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyNumberConflictsMin(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyNumberConflictsMin(a, len, left);
        }
    }
}

void heapifyNumberConflictsMax(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].numberConflicts < a[left].numberConflicts) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].numberConflicts < a[left].numberConflicts || a[index].numberConflicts < a[right].numberConflicts) {
        if (a[left].numberConflicts < a[right].numberConflicts) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyNumberConflictsMax(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyNumberConflictsMax(a, len, left);
        }
    }
}

void buildHeapNumberConflicts(struct item *a, int length, bool asc) {/// O(n)
    int i;

    if (asc) {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyNumberConflictsMax(a, length, i);
        }
    } else {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyNumberConflictsMin(a, length, i);
        }
    }

    return;
}

void heapsortNumberConflicts(struct item *list, int length, bool asc) {/// O(nlog n)
    struct item aux;
    int cunrrentLength = length;

    buildHeapNumberConflicts(list, length, asc);

    if (asc) { //Ordem crescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyNumberConflictsMax(list, cunrrentLength, 0);
        }

    } else { //Ordem decrescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyNumberConflictsMin(list, cunrrentLength, 0);
        }
    }
    return;
}

void heapifyValueMin(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].value > a[left].value) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].value > a[left].value || a[index].value > a[right].value) {
        if (a[left].value > a[right].value) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyValueMin(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyValueMin(a, len, left);
        }
    }
}

void heapifyValueMax(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].value < a[left].value) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].value < a[left].value || a[index].value < a[right].value) {
        if (a[left].value < a[right].value) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyValueMax(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyValueMax(a, len, left);
        }
    }
}

void buildHeapValue(struct item *a, int length, bool asc) {/// O(n)
    int i;

    if (asc) {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyValueMax(a, length, i);
        }
    } else {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyValueMin(a, length, i);
        }
    }

    return;
}

void heapsortValue(struct item *list, int length, bool asc) {/// O(nlog n)
    struct item aux;
    int cunrrentLength = length;

    buildHeapValue(list, length, asc);

    if (asc) { //Ordem crescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyValueMax(list, cunrrentLength, 0);
        }

    } else { //Ordem decrescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyValueMin(list, cunrrentLength, 0);
        }
    }
    return;
}

void heapifyRateMin(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].rate > a[left].rate) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].rate > a[left].rate || a[index].rate > a[right].rate) {
        if (a[left].rate > a[right].rate) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyRateMin(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyRateMin(a, len, left);
        }
    }
}

void heapifyRateMax(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].rate < a[left].rate) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].rate < a[left].rate || a[index].rate < a[right].rate) {
        if (a[left].rate < a[right].rate) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyRateMax(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyRateMax(a, len, left);
        }
    }
}

void buildHeapRate(struct item *a, int length, bool asc) {/// O(n)
    int i;

    if (asc) {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyRateMax(a, length, i);
        }
    } else {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyRateMin(a, length, i);
        }
    }

    return;
}

void heapsortRate(struct item *list, int length, bool asc) {/// O(nlog n)
    struct item aux;
    int cunrrentLength = length;

    buildHeapRate(list, length, asc);

    if (asc) { //Ordem crescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyRateMax(list, cunrrentLength, 0);
        }

    } else { //Ordem decrescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyRateMin(list, cunrrentLength, 0);
        }
    }
    return;
}

void heapifyWeightMin(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].weight > a[left].weight) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].weight > a[left].weight || a[index].weight > a[right].weight) {
        if (a[left].weight > a[right].weight) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyWeightMin(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyWeightMin(a, len, left);
        }
    }
}

void heapifyWeightMax(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].weight < a[left].weight) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].weight < a[left].weight || a[index].weight < a[right].weight) {
        if (a[left].weight < a[right].weight) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyWeightMax(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyWeightMax(a, len, left);
        }
    }
}

void buildHeapWeight(struct item *a, int length, bool asc) {/// O(n)
    int i;

    if (asc) {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyWeightMax(a, length, i);
        }
    } else {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyWeightMin(a, length, i);
        }
    }

    return;
}

void heapsortWeight(struct item *list, int length, bool asc) {/// O(nlog n)
    struct item aux;
    int cunrrentLength = length;

    buildHeapWeight(list, length, asc);

    if (asc) {
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyWeightMax(list, cunrrentLength, 0);
        }

    } else {
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyWeightMin(list, cunrrentLength, 0);
        }
    }
    return;
}

void heapifyIdMin(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].id > a[left].id) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].id > a[left].id || a[index].id > a[right].id) {
        if (a[left].id > a[right].id) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyIdMin(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyIdMin(a, len, left);
        }
    }
}

void heapifyIdMax(struct item *a, int len, int index) { /// O(log n)
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    struct item aux;

    if (left > len - 1) {
        return;
    } else if (right == len) {
        if (a[index].id < a[left].id) {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
        }
        return;
    } else if (a[index].id < a[left].id || a[index].id < a[right].id) {
        if (a[left].id < a[right].id) {
            //swap right with parent
            aux = a[index];
            a[index] = a[right];
            a[right] = aux;
            heapifyIdMax(a, len, right);
        } else {
            //swap left with parent
            aux = a[index];
            a[index] = a[left];
            a[left] = aux;
            heapifyIdMax(a, len, left);
        }
    }
}

void buildHeapId(struct item *a, int length, bool asc) {/// O(n)
    int i;

    if (asc) {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyIdMax(a, length, i);
        }
    } else {
        for (i = length - 1; i >= 0; i--) {
            if (2 * i + 1 > length - 1)
                continue;

            heapifyIdMin(a, length, i);
        }
    }

    return;
}

void heapsortId(struct item *list, int length, bool asc) {/// O(nlog n)
    struct item aux;
    int cunrrentLength = length;

    buildHeapId(list, length, asc);

    if (asc) { //Ordem crescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyIdMax(list, cunrrentLength, 0);
        }

    } else { //Ordem decrescente.
        while (cunrrentLength > 1) {
            //swap first with last
            aux = list[0];
            list[0] = list[cunrrentLength - 1];
            list[cunrrentLength - 1] = aux;
            cunrrentLength--;

            //heapify new heap
            heapifyIdMin(list, cunrrentLength, 0);
        }
    }
    return;
}

void verifyConflict(struct item *itens, FILE *file) {
    char line[3000];
    char *result;
    char *pch;
    int error = 0;
    int i = 0;
    int j = 0;
    int countConflict = 0;

    int number;
    int knapsack;

    result = fgets(line, 3000, file); //Pegando a primeira linha
    sscanf(result, "%d %d", &number, &knapsack);

    printf("\t Memory - \tFile\n");
    if (number != numberItens_) {
        printf("Number Itens: \t%d - \t%d\n", numberItens_, number);
        error++;
    }

    if (knapsack != knapsackSize_) {
        printf("Knapsack Size: \t%d - \t%d\n", knapsackSize_, knapsack);
        error++;
    }

    while (!feof(file)) {
        result = fgets(line, 3000, file); //Pegando a próxima linha.

        pch = strtok(result, " ");

        while (pch != NULL) { //Iterando na linha.

            if (strpbrk(pch, "0123456789") == NULL) {
                //                printf("if: %d; String: %s\n",error,pch);
            } else {
                //                printf("else: %d; Number: %d\n",error++,atoi(pch));
                switch (j) {
                    case 0:
                        if (itens[i].id != atoi(pch)) {
                            printf("Id: \t%d - \t%d\n", itens[i].id, atoi(pch));
                            error++;
                        }
                        break;
                    case 1:
                        if (itens[i].value != atoi(pch)) {
                            printf("Id %d; Value: \t%d - \t%d\n", itens[i].id, itens[i].value, atoi(pch));
                            error++;
                        }
                        break;
                    case 2:
                        if (itens[i].weight != atoi(pch)) {
                            printf("Id %d; Weight: \t%d - \t%d\n", itens[i].id, itens[i].weight, atoi(pch));
                            error++;
                        }
                        break;
                    default:
                        if (itens[i].conflicts[countConflict] != atoi(pch)) {
                            printf("Id %d; Conflict: \t%d - \t%d \t(Conflict Number:%d)\n", itens[i].id, itens[i].conflicts[countConflict], atoi(pch), countConflict + 1);
                            error++;
                        }
                        countConflict++;
                        break;
                }

                j++;
            }
            pch = strtok(NULL, " ");
        }

        if (itens[i].numberConflicts != countConflict) {
            printf("Id %d; Number Conflicts: \t%d - \t%d\n", itens[i].id, itens[i].numberConflicts, countConflict);
            error++;
        }

        i++;
        j = 0;
        countConflict = 0;
    }
    printf("\n\nTotal: %d\n\n", error);

}

void detailItens(struct item *a, int length) {
    printf("%d %d %d\n", numberItens_, knapsackSize_, length);

    heapsortId(a, length, true);
    int totalWeight = 0;
    int totalProfit = 0;

    for (int i = 0; i < length; i++) {
        printf("Line %d: ", i + 2);
        printf("Id: %d ; ", a[i].id);
        printf("Value: %d ; ", a[i].value);
        printf("Weight: %d ; ", a[i].weight);
        printf("Total Conflicts: %d\n", a[i].numberConflicts);
        printf("Conflits: ");
        for (int j = 0; j < a[i].numberConflicts; j++) {
            printf("%d ", a[i].conflicts[j]);
        }
        printf("\n \n");

        totalWeight += a[i].weight;
        totalProfit += a[i].value;
    }
    printf("Total de objetos: %d; Peso total: %d; Lucro total: %d\n", length, totalWeight, totalProfit);
}

void showItens(struct knapsack *knapsack, FILE *file) {/// O(n)
    //    heapsortId(knapsack->itens,knapsack->numberItens,true);

    //    printf("Heurisct: ");
    //    switch (knapsack->heuristic) {
    //    case 1:
    //        printf("1 value/weight desc\n");
    //        break;
    //    case 2:
    //        printf("2 value/weight asc\n");
    //        break;
    //    case 3:
    //        printf("3 weight desc\n");
    //        break;
    //    case 4:
    //        printf("4 weight asc\n");
    //        break;
    //    case 5:
    //        printf("5 value desc\n");
    //        break;
    //    case 6:
    //        printf("6 value asc\n");
    //        break;
    //    case 7:
    //        printf("7 conflict Max\n");
    //        break;
    //    case 8:
    //        printf("8 conflict Min\n");
    //        break;

    //    default:
    //        break;
    //    }
    //    printf("\n");

    printf("%d %d %f\n", knapsack->numberItens, knapsack->weightItens, knapsack->valueItens);

    for (int i = 0; i < knapsack->numberItens; i++) {
        printf("%d %f\n", knapsack->itens[i].id, knapsack->itens[i].fractionItem);
    }

    fprintf(file, "%d %d %f\n", knapsack->numberItens, knapsack->weightItens, knapsack->valueItens);

    for (int i = 0; i < knapsack->numberItens; i++) {
        fprintf(file, "%d %f\n", knapsack->itens[i].id, knapsack->itens[i].fractionItem);
    }
}

struct knapsack *compareMaxValue(struct knapsack *knapsack_A, struct knapsack *knapsack_B) {
    if (knapsack_A->valueItens > knapsack_B->valueItens)
        return knapsack_A;

    return knapsack_B;
}

struct knapsack *greedyKnapsackIntegerConflictsHeuristic(struct item *itens, int length, int max, int heuristic) { /// O(nm)
    int valueItens = 0;
    int weightItens = 0;
    int count = 0;

    struct knapsack *knapsack = (struct knapsack *) malloc(sizeof (struct knapsack)); //Vetor de incidencia de conflitos;
    knapsack->itens = (struct item *) malloc(sizeof (struct item));

    bool *conflict = (bool *) malloc(sizeof (bool) * length); //Vetor de incidencia de conflitos

    switch (heuristic) {
        case 1:
            heapsortRate(itens, length, false); /// O(nlogn)
            break;
        case 2:
            heapsortRate(itens, length, true); /// O(nlogn)
            break;
        case 3:
            heapsortWeight(itens, length, false); /// O(nlogn)
            break;
        case 4:
            heapsortWeight(itens, length, true); /// O(nlogn)
            break;
        case 5:
            heapsortValue(itens, length, false); /// O(nlogn)
            break;
        case 6:
            heapsortValue(itens, length, true); /// O(nlogn)
            break;
        case 7:
            heapsortNumberConflicts(itens, length, false); /// O(nlogn)
            break;
        case 8:
            heapsortNumberConflicts(itens, length, true); /// O(nlogn)
            break;
        default:
            break;
    }


    for (int i = 0; i < length; i++) { /// O(nm)

        if (conflict[itens[i].id - 1] == true || max < itens[i].weight) { //(Verifica se o item já foi marcado como conflito) e (Verifica se o peso do item é maior que o peso que ainda resta na mochila)
            continue;
        }

        knapsack->itens = (struct item *) realloc(knapsack->itens, sizeof (struct item)*(count + 1));
        knapsack->itens[count].id = itens[i].id;
        knapsack->itens[count].weight = itens[i].weight;
        knapsack->itens[count].value = itens[i].value;
        knapsack->itens[count].rate = itens[i].rate;
        knapsack->itens[count].fractionItem = itens[i].fractionItem;
        knapsack->itens[count].conflicts = itens[i].conflicts;
        knapsack->itens[count].numberConflicts = itens[i].numberConflicts;

        valueItens += itens[i].value;
        weightItens += itens[i].weight;
        count++;

        max = max - itens[i].weight;

        if (max == 0)
            break;

        for (int z = 0; z < itens[i].numberConflicts; z++) { /// O(m)
            conflict[itens[i].conflicts[z] - 1] = true; //Marca no vetor de incidencia os itens que possuem conflito com o item que foi colocado na mochila.
        }

    }

    knapsack->numberItens = count;
    knapsack->valueItens = (float) valueItens;
    knapsack->weightItens = weightItens;
    knapsack->heuristic = heuristic;

    return knapsack;
}

struct knapsack *dynamicProgrammingKnapsackInteger(struct item *item, int length, int max) {/// O(nW)
    heapsortWeight(item, length, true); /// O(nlog n)

    int table[length + 1][max + 1];

    for (int i = 0; i <= max; i++) { /// O(W)
        table[0][i] = 0;
    }

    for (int j = 0; j <= length; j++) { /// O(n)
        table[j][0] = 0;
    }

    for (int j = 1; j <= length; j++) { /// O(nW)
        for (int i = 1; i <= max; i++) {
            if (item[j - 1].weight <= i) {
                table[j][i] = item[j - 1].value + table[j - 1][i - item[j - 1].weight];
            } else {
                table[j][i] = table[j - 1][i];
                continue;
            }

            if (table[j][i] < table[j - 1][i])
                table[j][i] = table[j - 1][i];
        }
    }

    int weightItens = 0;
    int valueItens = 0;
    int j = length;
    int i = max;
    int count = 0;

    struct knapsack *knapsack = (struct knapsack *) malloc(sizeof (struct knapsack));
    knapsack->itens = (struct item *) malloc(sizeof (struct item));

    while (i != 0 && j != 0 && table[j][i] != 0) { /// O(W)
        if (table[j][i] != table[j - 1][i]) {
            knapsack->itens = (struct item *) realloc(knapsack->itens, sizeof (struct item)*(count + 1));
            knapsack->itens[count].id = item[j - 1].id;
            knapsack->itens[count].fractionItem = 1;

            weightItens += item[j - 1].weight;
            valueItens += item[j - 1].value;
            count++;
            i = i - item[j - 1].weight;
        }
        j--;
    }

    knapsack->numberItens = count;
    knapsack->valueItens = (float) valueItens;
    knapsack->weightItens = weightItens;

    return knapsack;
}

struct knapsack *greedyKnapsackFractional(struct item *item, int length, int max) {/// O(nlog n)
    heapsortRate(item, length, false); /// O(nlog n)

    float valueItens = 0;
    int weightItens = 0;
    int count = 0;

    struct knapsack *knapsack = (struct knapsack *) malloc(sizeof (struct knapsack)); //Vetor de incidencia de conflitos;
    knapsack->itens = (struct item *) malloc(sizeof (struct item));

    for (int i = 0; i < length; i++) { /// O(n)
        if (item[i].weight > max) {
            knapsack->itens = (struct item *) realloc(knapsack->itens, sizeof (struct item)*(count + 1));
            knapsack->itens[count].id = item[i].id;
            knapsack->itens[count].fractionItem = (float) max / item[i].weight;

            valueItens += (float) item[i].value * max / item[i].weight;
            weightItens += max;

            count++;

            break;
        }

        knapsack->itens = (struct item *) realloc(knapsack->itens, sizeof (struct item)*(count + 1));
        knapsack->itens[count].id = item[i].id;
        knapsack->itens[count].fractionItem = item[i].fractionItem;

        valueItens += (float) item[i].value;
        weightItens += item[i].weight;

        count++;

        max = max - item[i].weight;

        if (max == 0)
            break;
    }

    knapsack->numberItens = count;
    knapsack->valueItens = valueItens;
    knapsack->weightItens = weightItens;

    return knapsack;
}

struct item *loadItens(FILE *file) { ///O(n²)
    char line[3000];
    char *result;
    char *pch;

    int i = 0;
    int j = 0;
    int countConflict = 0;

    result = fgets(line, 3000, file); //Pegando a primeira linha
    sscanf(result, "%d %d", &numberItens_, &knapsackSize_);

    struct item *itens = (struct item *) malloc(sizeof (struct item) * numberItens_); //Vetor de itens
    int **conflicts = (int **) malloc(sizeof (int*) * numberItens_);

    bool **graphConflict = (bool **) malloc(sizeof (bool*) * numberItens_); //Grafo de conflitos
    for (int i = 0; i < numberItens_; i++) ///O(n)
        graphConflict[i] = (bool *) malloc(sizeof (bool) * numberItens_ + 1);

    while (!feof(file)) { ///O(n²)
        result = fgets(line, 3000, file); //Pegando a próxima linha.
        pch = strtok(result, " ");

        while (pch != NULL) { /*Iterando na linha.*/ ///O(m+3)
            if (strpbrk(pch, "0123456789") == NULL) { //Verifica se a string possui algum número
                //                printf("String: %s\n",pch);
            } else {
                switch (j) {
                    case 0: //Primeira string lida na linha
                        itens[i].id = atoi(pch); //Atribui o índice do item
                        itens[i].numberConflicts = 0;
                        itens[i].fractionItem = 1;
                        break;
                    case 1: //Segunda string lida na linha
                        itens[i].value = atoi(pch); //Atribui o valor do item
                        break;
                    case 2: //Terceira string lida na linha
                        itens[i].weight = atoi(pch); //Atribui o peso do item
                        itens[i].rate = (float) itens[i].value / itens[i].weight; //Atribui a razão entre o valor e peso do item.
                        break;
                    default: //Outras strings lida na linha
                        conflicts[i] = (int *) realloc(conflicts[i], sizeof (int)*(countConflict + 1));
                        conflicts[i][countConflict] = atoi(pch); //Atribui a lista de conflitos do item o índice lido na linha.
                        countConflict++;

                        graphConflict[itens[i].id - 1][atoi(pch) - 1] = true; //Atribui ao grafo de conflitos uma aresta entre os itens i e o índice lido na linha.
                        graphConflict[atoi(pch) - 1][itens[i].id - 1] = true; //Atribui ao grafo de conflitos uma aresta entre o índice lido na linha e o itens i.

                        break;
                }

                j++;
            }
            pch = strtok(NULL, " ");
        }

        if (j > 0) {
            int z = itens[i].id - 1;
            while (z > 0) { /// O(n)
                if (graphConflict[itens[i].id - 1][z - 1] == true) { //Verifica no grafo de conflitos se o item i tem conflito com os itens anteriores a ele
                    conflicts[i] = (int *) realloc(conflicts[i], sizeof (int)*(countConflict + 1));
                    conflicts[i][countConflict] = z; //Atribui a lista de conflitos do item i o índice z.
                    countConflict++;
                }

                z--;
            }

            itens[i].conflicts = conflicts[i];
            itens[i].numberConflicts = countConflict;
            i++;
            j = 0;
            countConflict = 0;
        }
    }

    //    detailItens(itens,numberItens_);

    return itens;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Programa sem parametros\n");
        return 0;
    }

    FILE *fileIn = fopen(argv[1], "r"); //Arquivo de entrada
    if (fileIn == NULL) {
        printf("Erro, nao foi possivel abrir o arquivo de entrada\n");
        return 0;
    }

    struct item *itens = loadItens(fileIn); /*Carrega os valores da instância.*/ /// O(n²)

    fclose(fileIn);

    FILE *fileOut = fopen("Output.txt", "w"); //Arquivo de saída
    if (fileOut == NULL) {
        printf("Erro, nao foi possivel criar o arquivo de saída\n");
        abort();
    }

    struct knapsack *knapsack1;
    struct knapsack *knapsack2;
    struct knapsack *knapsack3;
    struct knapsack *knapsack4;
    struct knapsack *knapsack5;
    struct knapsack *knapsack6;
    struct knapsack *knapsack7;
    struct knapsack *knapsack8;

    switch (atoi(argv[2])) {
        case 1: //Questão 1
            showItens(greedyKnapsackFractional(itens, numberItens_, knapsackSize_), fileOut); /// O(nlog n)
            break;

        case 2: //Questão 2
            showItens(dynamicProgrammingKnapsackInteger(itens, numberItens_, knapsackSize_), fileOut); /// O(nW)
            break;

        case 3: //Questão 3
            knapsack1 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 1); /// O(nm)
            knapsack2 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 2); /// O(nm)
            knapsack3 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 3); /// O(nm)
            knapsack4 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 4); /// O(nm)
            knapsack5 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 5); /// O(nm)
            knapsack6 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 6); /// O(nm)
            knapsack7 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 7); /// O(nm)
            knapsack8 = greedyKnapsackIntegerConflictsHeuristic(itens, numberItens_, knapsackSize_, 8); /// O(nm)

            //        printf("%d %d %f\n",knapsack1->numberItens,knapsack1->weightItens,knapsack1->valueItens);
            //        printf("%d %d %f\n",knapsack2->numberItens,knapsack2->weightItens,knapsack2->valueItens);
            //        printf("%d %d %f\n",knapsack3->numberItens,knapsack3->weightItens,knapsack3->valueItens);
            //        printf("%d %d %f\n",knapsack4->numberItens,knapsack4->weightItens,knapsack4->valueItens);
            //        printf("%d %d %f\n",knapsack5->numberItens,knapsack5->weightItens,knapsack5->valueItens);
            //        printf("%d %d %f\n",knapsack6->numberItens,knapsack6->weightItens,knapsack6->valueItens);
            //        printf("%d %d %f\n",knapsack7->numberItens,knapsack7->weightItens,knapsack7->valueItens);
            //        printf("%d %d %f\n",knapsack8->numberItens,knapsack8->weightItens,knapsack8->valueItens);
            //        printf("\n\n");

            showItens(compareMaxValue(compareMaxValue(compareMaxValue(knapsack1, knapsack2), compareMaxValue(knapsack3, knapsack4)), compareMaxValue(compareMaxValue(knapsack5, knapsack6), compareMaxValue(knapsack7, knapsack8))), fileOut); /// O(n)
            break;
        default:
            break;
    }

    fclose(fileOut);

    return 0;
}
