/* Tema 1 - Structuri de Date și Algoritmi - Banda Magica
LACHE Alexandra Florentina Georgiana, 311 CD, UPB ACS 2022-2023 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "magicStrip.h"
#include "UndoRedo.h"
#include "CommandQueue.h"

#define CMP(cmd_read, str) if (strcmp(cmd_read, str) == 0)
#define CMP2(cmd_read, str1, str2) if (strcmp(cmd_read, str1) == 0 \
                                    || strcmp(cmd_read, str2) == 0)

// functie care scoate din coada o comanda si o executa 
void execute(FILE *file, magicStrip strip, Stack UNDO, Stack REDO, Queue q) {
    QueueNode *aux = dequeue(q);

    if (aux != NULL) {
        CMP(aux->command, "MOVE_LEFT") {
            push(UNDO, strip->actual);
            moveLeft(strip);
            /* se verifica daca s-a modificat pozitia degetului
            in caz negativ se scoate din stiva */
            if (strip->actual == UNDO->head->node) 
                pop(UNDO);
        } else CMP(aux->command, "MOVE_RIGHT") {
            push(UNDO, strip->actual);
            moveRight(strip);
        } else CMP(aux->command, "WRITE") {
            write(strip, aux->argument);
        } else CMP(aux->command, "MOVE_LEFT_CHAR") {
            moveLeftChar(file, strip, aux->argument);
        } else CMP(aux->command, "MOVE_RIGHT_CHAR") {
            moveRightChar(strip, aux->argument);
        } else CMP(aux->command, "INSERT_LEFT") {
            insertLeft(file, strip, aux->argument);
        } else CMP(aux->command, "INSERT_RIGHT") {
            insertRight(strip, aux->argument);
        }
        free(aux);
    }
}

int main() {
    char input_filename[] = "tema1.in";
    char output_filename[] = "tema1.out";
    char command[21], argument;

    // deschidere fisiere de input/output cu verificare
    FILE *in = fopen(input_filename, "rt");
    if (in == NULL) {
        fprintf(stderr, "ERROR: Cannot open file %s\n", input_filename);
        return -1;
    }

    FILE *out = fopen(output_filename, "wt");
    if (out == NULL) {
        fprintf(stderr, "ERROR: Cannot open file %s\n", output_filename);
        return -1;
    }

    int number_op;
    fscanf(in,"%d", &number_op);

    magicStrip strip = init_strip();
    Stack UNDO = init_stack(), REDO = init_stack();
    Queue opQueue = init_queue();

    // citirea celor number_op comenzi din fisier (si a argumentelor specifice)
    for (int i = 1; i <= number_op; ++i) {
        fscanf(in, "%s", command);
        CMP(command, "SHOW_CURRENT") {
            showCurrent(out, strip);
        } else CMP(command, "SHOW") {
            show(out, strip);
        } else CMP(command, "EXECUTE") {
            execute(out, strip, UNDO, REDO, opQueue);
        } else CMP(command, "UNDO") {
            magicStripNode *auxNode = pop(UNDO);
            // se verifica ca nu s-a extras NULL din stiva
            if (auxNode != NULL) { 
                push(REDO, strip->actual);
                strip->actual = auxNode;
            }
        } else CMP(command, "REDO") {
            magicStripNode *auxNode = pop(REDO);
            // se verifica ca nu s-a extras NULL din stiva
            if (auxNode != NULL) {
                push(UNDO, strip->actual);
                strip->actual = auxNode;
            }
        } else CMP2(command, "MOVE_LEFT", "MOVE_RIGHT") {
            enqueue(opQueue, command, ' ');
        } else {
            do {
                fscanf(in, "%c", &argument);
            } while (argument == ' ');
            enqueue(opQueue, command, argument);
        }
    }
    
    // eliberare memorie pentru structuri si inchidere fisiere
    destroyStack(UNDO);
    destroyStack(REDO);
    destroyQueue(opQueue);
    freeMagicStrip(strip);
    fclose(in);
    fclose(out);

    return 0;
}