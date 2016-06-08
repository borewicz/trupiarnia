#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum State
{
    Free = 0, // wolny
    Processed = 1, // będący obiektem pożądania grabarzy
    Buried = 2 // pochowany
};

void broadcast(int *data, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
        MPI_Send( data, 2, MPI_INT, i, i, MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    int U[size];
    int T[100] = {0};

    int i = 0;
    for (i = 0; i < size; i++)
        U[i] = 0;
    srand(time(NULL));

    while(1)
    {
        int trup_id = rand() % 100;
        int message[2] = { rank, 1 };
        //wysyłamy wszystkim info, że chcemy podjąć trupa
        broadcast(message, size);
        //zaznaczamy, że ubiegamy się o trupa
        U[rank] = 1;
        //i wpisujemy jedynkę do trupa o którego prosime
        T[trup_id] = 1;
        int response[2] = {0, 0};
        int remained = size;
        do
        {
            MPI_Recv( &response, 2, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("rank %d: dostałem od %d\n", rank, response[0]);
            //tutej warunki
            remained--;
        }
        while (remained > 0);
        sleep(2);
    }
    MPI_Finalize();
}
