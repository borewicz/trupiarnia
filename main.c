#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void broadcast(int *data, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
        MPI_Send( data, 3, MPI_INT, i, i, MPI_COMM_WORLD);
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
        int message[3] = { rank, 1, trup_id };
        broadcast(message, size);
        U[rank] = 1;
        T[trup_id] = 1;
        int remained = size;
        do
        {
            int response[3];
            MPI_Recv( &response, 3, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("rank %d: dostałem od %d, trup_id: %d\n", rank, response[0], response[2]);
            if ((response[2] == trup_id) && (response[0] < rank))
                continue;
            else if (response[1] == 2)
                U[response[0]] == 1;
            remained--;
        }
        while (remained > 0);
        sleep(2);
    }
    MPI_Finalize();
}
