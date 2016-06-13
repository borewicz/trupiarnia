#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// do tablicy T
#define FREE 0
#define PROCESSED 1
#define BURIED 2

//do tablicy U
#define WAITING 1
#define PROCESS -1

#define MSG_LENGTH 4

void broadcast(int *data, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
        MPI_Send( data, MSG_LENGTH, MPI_INT, i, i, MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    // lista procesów ubiegających się o trupa
    int U[size];
    // tablica trupów - założyłem 100
    int T[100] = {0};
    // zegar lamporta
    int C = 0;

    int i = 0;
    for (i = 0; i < size; i++)
        U[i] = 0;

    /*
     * śmieszna struktura
     * - rank
     * - stan
     * - trup_id
     * - zegar lamporta
     */

    while(1)
    {
        int trup_id = rand() % 100; //tmp
        //format: id_procesu, status, id_trupa
//        int message[3] = { rank, State.Processed, trup_id };
        int message[MSG_LENGTH] = { rank, PROCESSED, trup_id, C };
        //wysyłamy wszystkim info, że chcemy podjąć trupa
        broadcast(message, size);
        //zaznaczamy, że ubiegamy się o trupa
        U[rank] = 1;
        //i wpisujemy jedynkę do trupa o którego prosime
        T[trup_id] = 1;
        int remained = size;
        do
        {
            int response[4];
            MPI_Recv( &response, MSG_LENGTH, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            C = MAX(C, response[3]);
            printf("rank %d: dostałem od %d, trup_id: %d, C=%d\n", rank, response[0], response[2], response[3]);
            if ((response[2] == trup_id) && (response[0] < rank))
                continue;
            else if (response[1] == 2)
                U[response[0]] == 1;
            //else if ()
            //sekcja krytyczna?
            //itd.
            remained--;
        }
        while (remained > 0);
        //na razie omijamy to poniżej xD
        continue;
        C++;
       /*
        * POCZĄTEK SEKCJI KRYTYCZNEJ
        */
        int request[4] = {
            rank, BURIED, trup_id, C
        };
        broadcast(request, size); //wysyłamy że chowamy
        sleep(2); // chowamy
        do
        {
            int response[4];
            MPI_Recv( &response, MSG_LENGTH, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // tutaj użyć tablicy R żeby sprawdzić czy dostaliśmy od wszystkich
            // i elo
            break;
        }
        while (1);
        /*
         * KONIEC SEKCJI KRYTYCZNEJ, ELO
         */
    }
    MPI_Finalize();
}
