import torch

# Practice Exercise 1: Create a tensor
# Create a tensor of size 3x3 filled with random values
random_tensor = torch.rand(3, 3)
print("Random Tensor:\n", random_tensor)

# Practice Exercise 2: Create a tensor from a list
data_list = [[1, 2], [3, 4]]
tensor_from_list = torch.tensor(data_list)
print("Tensor from List:\n", tensor_from_list)

# Practice Exercise 3: Basic operation (Addition)
tensor_a = torch.tensor([[1.0, 2.0], [3.0, 4.0]])
tensor_b = torch.tensor([[5.0, 6.0], [7.0, 8.0]])
result_sum = tensor_a + tensor_b
print("Sum Tensor:\n", result_sum)