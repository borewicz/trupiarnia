#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACK 3

typedef enum {false, true} bool;

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

void sendAllInU(int *data, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
        if (data[i] == FREE || data[i] == PROCESSED)
            MPI_Send( data, MSG_LENGTH, MPI_INT, i, i, MPI_COMM_WORLD);
}

bool ready(int *table, int size)
{
    int i = 0;
    for (i; i < size; i++)
        if (table[i] == 0)
            return 0;
    return 1;
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


    while(!ready(T, 100))
    {
        int trup_id = rand() % 100; 
        int message[MSG_LENGTH] = { rank, PROCESSED, trup_id, C };
        sendAllInU(message, size);
        U[rank] = 1;
        T[trup_id] = 1;

        do
        {
            int response[4];
            MPI_Recv( &response, MSG_LENGTH, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            C = MAX(C, response[3]);
            printf("rank %d: dostałem od %d, trup_id: %d, C=%d\n", rank, response[0], response[2], response[3]);

            if ((response[2] == trup_id) && (response[0] < rank))
            {
                continue;
            }
            else if (response[1] == BURIED)
            {
                U[response[0]] = -1;
                T[response[2]] = BURIED;
            }
            else if (response[1] == BACK)
            {
                U[response[0]] = 0;
            }
            else 
            {
                T[response[2]] = PROCESSED;
                U[response[0]] = 1;
            }
        }
        while (!ready(U,size));

        C++;
        int request[4] = {
            rank, BURIED, trup_id, C
        };
        broadcast(request, size); 
        sleep(2); 
        do
        {
            int response[4];
            MPI_Recv( &response, MSG_LENGTH, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // tutaj użyć tablicy R żeby sprawdzić czy dostaliśmy od wszystkich
            // i elo
            break;
        }
        while (1);

    }
    MPI_Finalize();
}
