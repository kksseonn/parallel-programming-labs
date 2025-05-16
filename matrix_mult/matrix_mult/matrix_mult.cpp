#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mpi.h>

using namespace std;

typedef vector<vector<int>> Matrix;

Matrix read_matrix(const string& filename, int size) {
    ifstream file(filename);
    Matrix matrix(size, vector<int>(size));
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            file >> matrix[i][j];
    return matrix;
}

void save_matrix(const string& filename, const Matrix& matrix) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file for writing: " << filename << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (const auto& row : matrix) {
        for (const auto& elem : row)
            file << elem << " ";
        file << "\n";
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_proc);

    if (argc < 5) {
        if (rank == 0)
            cerr << "Usage: " << argv[0] << " <size> <fileA> <fileB> <fileC>\n";
        MPI_Finalize();
        return 1;
    }

    int N = stoi(argv[1]);
    string file_A = argv[2];
    string file_B = argv[3];
    string file_C = argv[4];

    // Все процессы получают B
    Matrix B(N, vector<int>(N));
    Matrix A_local;
    Matrix C_local;

    if (rank == 0) {
        // Только мастер читает матрицы
        Matrix A_full = read_matrix(file_A, N);
        B = read_matrix(file_B, N);

        // Рассылаем строки A другим процессам
        for (int i = 0; i < N; ++i)
            MPI_Bcast(B[i].data(), N, MPI_INT, 0, MPI_COMM_WORLD);

        int rows_per_proc = N / size_proc;
        int extra = N % size_proc;

        for (int i = 1; i < size_proc; ++i) {
            int start_row = i * rows_per_proc + min(i, extra);
            int rows = rows_per_proc + (i < extra ? 1 : 0);
            for (int r = 0; r < rows; ++r)
                MPI_Send(A_full[start_row + r].data(), N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Выделяем строки для себя
        int my_rows = rows_per_proc + (rank < extra ? 1 : 0);
        A_local.resize(my_rows, vector<int>(N));
        for (int r = 0; r < my_rows; ++r)
            A_local[r] = A_full[r];
    }
    else {
        for (int i = 0; i < N; ++i)
            MPI_Bcast(B[i].data(), N, MPI_INT, 0, MPI_COMM_WORLD);

        int rows_per_proc = N / size_proc;
        int extra = N % size_proc;
        int my_rows = rows_per_proc + (rank < extra ? 1 : 0);
        A_local.resize(my_rows, vector<int>(N));
        for (int r = 0; r < my_rows; ++r)
            MPI_Recv(A_local[r].data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Выполняем умножение
    int my_rows = A_local.size();
    C_local.resize(my_rows, vector<int>(N, 0));

    for (int i = 0; i < my_rows; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C_local[i][j] += A_local[i][k] * B[k][j];

    // Сбор результатов на rank 0
    if (rank == 0) {
        Matrix C(N, vector<int>(N));
        for (int i = 0; i < my_rows; ++i)
            C[i] = C_local[i];

        for (int p = 1; p < size_proc; ++p) {
            int rows_per_proc = N / size_proc;
            int extra = N % size_proc;
            int start_row = p * rows_per_proc + min(p, extra);
            int rows = rows_per_proc + (p < extra ? 1 : 0);

            for (int r = 0; r < rows; ++r)
                MPI_Recv(C[start_row + r].data(), N, MPI_INT, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        save_matrix(file_C, C);
        cout << "✅ Результат сохранён в файл: " << file_C << endl;
    }
    else {
        for (int i = 0; i < my_rows; ++i)
            MPI_Send(C_local[i].data(), N, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
