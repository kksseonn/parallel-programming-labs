import numpy as np
import os

def save_matrix_to_file(matrix, filename):
    """Сохраняет матрицу в текстовый файл."""
    np.savetxt(filename, matrix, fmt='%d')

def generate_and_save_matrices(sizes, base_dir="data"):
    """Генерирует квадратные матрицы заданных размеров и сохраняет их в файлы."""
    os.makedirs(base_dir, exist_ok=True)
    
    for size in sizes:
        dir_path = os.path.join(base_dir, str(size))
        os.makedirs(dir_path, exist_ok=True)
        
        matrix_A = np.random.randint(0, 999, (size, size))
        matrix_B = np.random.randint(0, 999, (size, size))
        
        save_matrix_to_file(matrix_A, os.path.join(dir_path, "matrix_A.txt"))
        save_matrix_to_file(matrix_B, os.path.join(dir_path, "matrix_B.txt"))
        
        print(f"Матрицы {size}x{size} сохранены в {dir_path}")

sizes = [10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
generate_and_save_matrices(sizes)