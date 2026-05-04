import torch

# Game/Project: Simple Tensor Visualization
# A simple project where the user visualizes a tensor operation (e.g., gradient flow).

# 1. Setup a random tensor
x = torch.rand(5, 5, requires_grad=True)

# 2. Perform a simple operation
y = x.mean()

# 3. Calculate gradients
y.backward()

# 4. Display the gradient
print("Original Tensor x:\n", x)
print("Gradient of x:\n", x.grad)