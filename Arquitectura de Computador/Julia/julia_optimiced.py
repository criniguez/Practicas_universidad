
import matplotlib.pyplot as plt
import numpy as np
# area of complex space to investigate
x1, x2, y1, y2 = -1.8, 1.8, -1.8, 1.8
c_real, c_imag =  -0.62772, -.42193

def calc_pure_python(desired_width, max_iterations):
  """Create a list of complex coordinates (zs) and complex parameters (cs), build Julia set, and display"""

  x_step = (float(x2 - x1) / float(desired_width))
  y_step = (float(y1 - y2) / float(desired_width))
  x = []
  y = []

  for i in range(desired_width):
    y.append(y2 + i * y_step)
    x.append(x1 + i * x_step)


  # Build a list of coordinates and the initial condition for each cell.
  # Note that our initial condition is a constant and could easily be removed;
  # we use it to simulate a real-world scenario with several inputs to
  # our function.
  zs = []
  cs = complex(c_real, c_imag)
  for ycoord in y:
    for xcoord in x[1:]:
      zs.append(complex(xcoord, ycoord))

  #print(np.reshape(calculate_z(max_iterations, zs2, cs), [desired_width, desired_width]))
  index   = (desired_width//2)*(desired_width-1)
  output  = calculate_z(max_iterations, zs[(desired_width-1):index], cs)
  output2 = calculate_z(max_iterations, [complex(y[0], x[0])] + zs[:desired_width-1], cs)
  output3 = calculate_z(max_iterations, zs[index:index+(desired_width-1)], cs)
  plott = np.reshape(output+output3+output[::-1], [desired_width-1, desired_width-1], order="C")
  plt.imshow(plott)
  plt.show()
  # This sum is expected for a 1000^2 grid with 300 iterations.
  # It catches minor errors we might introduce when we're
  # working on a fixed set of inputs.
  # assert sum(output) == 33219980
  print(sum(output2), sum(output), sum(output3),(sum(output) + sum(output2))* 2 + sum(output3))

def calculate_z(maxiter, zs, c):
  """Calculate output list using Julia update rule"""
  output = [0] * len(zs)
  for i in range(len(zs)):
    n = 0
    z = zs[i]
    while abs(z) < 2 and n < maxiter:
      z = z * z + c
      n += 1
    output[i] = n
  return output

if __name__ == "__main__":
  # Calculate the Julia set using a pure Python solution
  calc_pure_python(desired_width=2000, max_iterations=300)