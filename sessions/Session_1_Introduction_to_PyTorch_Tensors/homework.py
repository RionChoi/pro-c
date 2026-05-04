import torch

# Homework Exercise: Matrix Multiplication
# Given two matrices, perform matrix multiplication (dot product).
# Matrix A: [[1, 0], [0, 1]] (Identity matrix)
# Matrix B: [[5, 6], [7, 8]]
# Calculate C = A @ B

A = torch.tensor([[1.0, 0.0], [0.0, 1.0]])
B = torch.tensor([[5.0, 6.0], [7.0, 8.0]])

C = torch.matmul(A, B)
print("Result of A @ B:\n", C)

# Extra Challenge: Reshaping a tensor
# Create a 1D tensor of 12 elements and reshape it into a 3x4 matrix.
flat_tensor = torch.arange(12)
reshaped_tensor = flat_tensor.reshape(3, 4)
print("Reshaped Tensor:\n", reshaped_tensor)