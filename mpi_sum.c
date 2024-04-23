
// sudo apt install mpich
// mpi_sum.c
//1. mpicc mpi_sum.c -o mpi_sum
// 2. mpirun -np 4 ./mpi_sum

// gcc -v
//sudo apt-get install build-essential gdb


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int unitsize = 5;
    int root = 0;
    int *send_buffer = NULL;
    int *receive_buffer = NULL;
    int *new_receive_buffer = NULL;
    int total_elements = 0;
    int total_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocate memory for send buffer
    if (rank == root) {
        total_elements = unitsize * size;
        send_buffer = (int *)malloc(total_elements * sizeof(int));
        printf("Enter %d elements:\n", total_elements);
        for (int i = 0; i < total_elements; i++) {
            send_buffer[i] = i;
            printf("Element %d = %d\n", i, i);
        }
    }

    // Allocate memory for receive buffers
    receive_buffer = (int *)malloc(unitsize * sizeof(int));
    new_receive_buffer = (int *)malloc(size * sizeof(int));

    // Scatter data to processes
    MPI_Scatter(send_buffer, unitsize, MPI_INT, receive_buffer, unitsize, MPI_INT, root, MPI_COMM_WORLD);

    // Calculate sum at non-root processes and store result in first index of array
    int local_sum = 0;
    for (int i = 0; i < unitsize; i++) {
        local_sum += receive_buffer[i];
    }
    printf("Intermediate sum at process %d is %d\n", rank, local_sum);

    // Gather data from processes
    MPI_Gather(&local_sum, 1, MPI_INT, new_receive_buffer, 1, MPI_INT, root, MPI_COMM_WORLD);

    // Aggregate output from all non-root processes
    if (rank == root) {
        total_sum = 0;
        for (int i = 0; i < size; i++) {
            total_sum += new_receive_buffer[i];
        }
        printf("Final sum: %d\n", total_sum);
    }

    // Free allocated memory
    if (rank == root) {
        free(send_buffer);
    }
    free(receive_buffer);
    free(new_receive_buffer);

    MPI_Finalize();
    return 0;
}
