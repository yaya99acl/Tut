#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // 初始化MPI环境
    MPI_Init(&argc, &argv);

    // 获取进程的rank和总进程数
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int token;
    // 根据进程号计算队友的rank
    int teammate_rank = (world_rank + 2) % world_size;

    if (world_rank == 0) {
        // 如果是进程0，设置token的初始值
        token = -1;
        // 发送token给下一个进程
        MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Process %d sent token %d to process %d\n", world_rank, token, 1);
        // 通知队友
        MPI_Send(&token, 1, MPI_INT, teammate_rank, 0, MPI_COMM_WORLD);
        printf("Process %d notified teammate %d\n", world_rank, teammate_rank);
    }

    // 同步点，确保0号进程完成发送
    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank != 0) {
        // 接收来自前一个进程的token
        MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n", world_rank, token, world_rank - 1);
        // 通知队友
        MPI_Send(&token, 1, MPI_INT, teammate_rank, 0, MPI_COMM_WORLD);
        printf("Process %d notified teammate %d\n", world_rank, teammate_rank);
    }

    // 所有进程完成接收和通知后的同步点
    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank != 0) {
        // 发送token给下一个进程前再次通知队友
        MPI_Send(&token, 1, MPI_INT, teammate_rank, 0, MPI_COMM_WORLD);
        printf("Process %d notified teammate %d with token %d before sending\n", world_rank, teammate_rank, token);
        // 发送token给下一个进程
        if (world_rank < world_size - 1) {
            MPI_Send(&token, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
            printf("Process %d sent token %d to process %d\n", world_rank, token, world_rank + 1);
        }
    }

    // 最后一个进程发送token回到进程0
    if (world_rank == world_size - 1) {
        MPI_Send(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Process %d sent token %d to process %d\n", world_rank, token, 0);
    }

    // 确保环形通信完成后，进程0从最后一个进程接收token
    if (world_rank == 0) {
        MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n", world_rank, token, world_size - 1);
    }

    MPI_Finalize();
    return 0;
}
