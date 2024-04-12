import numpy as np
import matplotlib.pyplot as plt

"""
This script investigates the optimal window size and position according to the regression error to compare the performance of the different regressions used to describe the same peak.
"""

# create some experimental data
sigma = 3 # standard deviation
mu = 0 # mean
a = 1 # amplitude

scale = 20 # window half size
x = np.linspace(-scale, scale, 2*scale +1) # x values for the data
y = a * np.exp(-0.5 * ((x - mu) / sigma) ** 2) # y values for the data

# add gaussian noise to the data
noise = .05 # standard deviation of the noise
y += np.random.normal(0, noise, len(y)) # add noise to the data

# add offset, that minimum is not at 0
offset = 2*min(y) # offset
y -= offset # add offset to the data

plt.plot(x, y, 'o')
plt.show()