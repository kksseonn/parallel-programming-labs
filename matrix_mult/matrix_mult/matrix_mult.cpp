#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

typedef vector<vector<int>> Matrix;

Matrix read_matrix(const string& filename, int size) {
    ifstream file(filename);
    Matrix matrix(size, vector<int>(size));
    if (file.is_open()) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                file >> matrix[i][j];
            }
        }
        file.close();
    }
    else {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
    }
    return matrix;
}

void save_matrix(const string& filename, const Matrix& matrix) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : matrix) {
            for (const auto& elem : row) {
                file << elem << " ";
            }
            file << endl;
        }
        file.close();
    }
    else {
        cerr << "Failed to open file for writing: " << filename << endl;
        exit(1);
    }
}

Matrix multiply_matrices(const Matrix& A, const Matrix& B, int size) {
    Matrix C(size, vector<int>(size, 0));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

int main(int argc, char* argv[]) {

    int size = stoi(argv[1]);
    string file_A = argv[2];
    string file_B = argv[3];
    string file_C = argv[4];

    Matrix A = read_matrix(file_A, size);
    Matrix B = read_matrix(file_B, size);
    Matrix C = multiply_matrices(A, B, size);
    save_matrix(file_C, C);
    cout << "The result is saved in: " << file_C << endl;
    return 0;
}
