#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <omp.h>            

using namespace std;

typedef vector<vector<int>> Matrix;

Matrix read_matrix(const string& filename, int size) {
    ifstream file(filename);
    Matrix matrix(size, vector<int>(size));
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
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
        exit(1);
    }
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            file << elem << " ";
        }
        file << "\n";
    }
}

Matrix multiply_matrices(const Matrix& A, const Matrix& B, int size) {
    Matrix C(size, vector<int>(size, 0));

#pragma omp parallel for schedule(dynamic) default(none) shared(A, B, C, size)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int sum = 0;
            for (int k = 0; k < size; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    return C;
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <size> <fileA> <fileB> <fileC> [num_threads]\n";
        return 1;
    }

    int size = stoi(argv[1]);
    string file_A = argv[2];
    string file_B = argv[3];
    string file_C = argv[4];

    if (argc >= 6) {
        int nt = stoi(argv[5]);
        if (nt < 1) nt = 1;
        if (nt > 6) nt = 6;
        omp_set_num_threads(nt);
        cout << "OpenMP: threads = " << nt << "\n";
    }
    else {
        cout << "OpenMP: threads = " << omp_get_max_threads() << "\n";
    }

    Matrix A = read_matrix(file_A, size);
    Matrix B = read_matrix(file_B, size);
    Matrix C = multiply_matrices(A, B, size);
    save_matrix(file_C, C);

    cout << "Result saved to: " << file_C << endl;

    return 0;
}