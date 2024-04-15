import numpy as np
import matplotlib.pyplot as plt

"""
This script investigates the optimal window size and position according to the regression error to compare the performance of the different regressions used to describe the same peak.
"""

def regression(scale, y):
    """
    This function calculates the regression error of a gaussian fit to the data.
    scale: window size
    y: data
    """
    # create design Matrix
    x = np.linspace(-scale, scale, 2*scale +1) # x values for the data
    X = np.c_[np.ones(len(x)), x, x**2, x**2] # design matrix
    X[x<0, 3] = 0 # set negative values to 0
    X[x>0, 2] = 0 # set positive values to 0
    b = np.linalg.inv(X.T @ X) @ X.T @ np.log(y) # regression coefficients
    y_pred = np.exp(X @ b) # predicted y values

    """add a weight to the error based on the distance to the regression apex"""
    # calculate the apex of the regression
    if b[1] < 0:
        apex = -b[1] / (2*b[2])
    else:
        apex = -b[1] / (2*b[3])

    # calculate the distance to the apex
    distance = np.abs(x - apex)

    # calculate the weight thereby a distance higher than scale should be almost 0
    weight = np.exp(-.5 * (distance / scale)**2)
    # normalize the weight
    weight /= np.sum(weight)

    error = np.sum((y - y_pred)**2 * weight) # regression error
    mse = error
    return mse

# create some experimental data
sigma = 3 # standard deviation
mu = 0 # mean
a = 1 # amplitude

scale = 20 # window half size
x = np.linspace(-scale, scale, 2*scale +1) # x values for the data
y = a * np.exp(-0.5 * ((x - mu) / sigma) ** 2) # y values for the data

# add gaussian noise to the data
noise = .001 # standard deviation of the noise
y += np.random.normal(0, noise, len(y)) # add noise to the data

# add offset, that minimum is not at 0
offset = 2*min(y) # offset
y -= offset # add offset to the data

# calculate regression error for different window sizes at the center of the peak
scales = np.arange(2, scale) # window sizes
errors = np.zeros(len(scales)) # regression errors
for i, scale in enumerate(scales):
    errors[i] = regression(scale, y[abs(x) <= scale])

# plot regression error
plt.plot(scales, errors)
## add the 3 sigma line
plt.axvline(3*sigma, color='r', linestyle='--', label='3 sigma')
plt.xlabel('Window size')
plt.ylabel('Regression error')
plt.title('Regression error depending on window size')
plt.show()


# calculate regression error for different window positions with a fixed window size
scale = 3*sigma # window size
errors = np.zeros(len(y) - 2*scale) # regression errors
for i in range(len(y) - 2*scale):
    errors[i] = regression(scale, y[i:i+2*scale+1])

# plot regression error
plt.plot(np.arange(len(y) - 2*scale), errors)
## add the peak position
plt.axvline(mu+20-scale, color='r', linestyle='--', label='Peak position')
plt.xlabel('Window position')
plt.ylabel('Regression error')
plt.title('Regression error depending on window position')
plt.show()
