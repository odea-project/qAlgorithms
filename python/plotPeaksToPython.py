import sys
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def readData(filename):
    # check if data is in .csv or .txt format
    if filename.endswith('.csv'):
        data = pd.read_csv(filename)
    
    elif filename.endswith('.txt'):
        data = pd.read_csv(filename, sep='\t')

    else:
        print("File format not supported")
        sys.exit(1)

    return data

def createFittedData(coefficients, height):
    # create the fitted data
    ydata = np.array([])
    xdata = np.array([])
    # right side of the peak
    # check for valley point
    vp = -coefficients.iloc[1] / (2 * coefficients.iloc[3]) if coefficients.iloc[3] > 0 else 100

    for i in np.arange(0,int(vp),.1):
        y_tmp = np.exp(coefficients.iloc[0] + coefficients.iloc[1] * i + coefficients.iloc[3] * i**2)
        if y_tmp < 0.1 * height:
            break
        ydata = np.append(ydata, y_tmp)
        xdata = np.append(xdata, i)
    
    # left side of the peak
    # check for valley point
    vp = -coefficients.iloc[1] / (2 * coefficients.iloc[2]) if coefficients.iloc[2] > 0 else -100
    for i in np.arange(0,int(vp),-.1):
        y_tmp = np.exp(coefficients.iloc[0] + coefficients.iloc[1] * i + coefficients.iloc[2] * i**2)
        if y_tmp < 0.1 * height:
            break
        ydata = np.append(y_tmp, ydata)
        xdata = np.append(i, xdata)
    
    return xdata, ydata
    



def main():
    filename_input = sys.argv[1]
    filename_output = sys.argv[2]
    includeFits = bool(sys.argv[3])
    featureMap = bool(sys.argv[4])

    # user input to chose the scan (0 is default)
    scan_choice = int(input("Enter the scan number: "))
    # validate scan choice, i.e. if not integer or negative number, repeat the input
    while scan_choice < 0:
        print("Invalid scan number, please enter a positive integer")
        scan_choice = int(input("Enter the scan number: "))

    # read the data from input file
    data = readData(filename_input)
   
    # validate scan choice, i.e. if scan_choice is not available in "scan", repeat the input
    while scan_choice not in data['scan'].values:
        print("Invalid scan number, please enter a valid scan number")
        scan_choice = int(input("Enter the scan number: "))

     # filter data based on scan column == 0 to get only the first scan
    data_extract = data[data['scan'] == scan_choice]
    # plot the data 1st and 2nd columns
    fig, ax = plt.subplots()
    ax.plot(data_extract.iloc[:, 0], data_extract.iloc[:, 1], 'r-')
    plt.xlabel('x-axis')
    plt.ylabel('y-axis')

    # plot the peaks
    # read the peaks from the output file
    peaks = readData(filename_output)
    # filter the peaks based on the SampleID column == 0 to get only the first scan
    peaks = peaks[peaks['SampleID'] == scan_choice] 

    # plot the peaks as vertial lines using Position and Height columns
    for index, row in peaks.iterrows():
        ax.axvline(x=row['Position'], color='b', linestyle='--', linewidth=0.25)
        ax.scatter(row['Position'], row['Height'], color='b', marker='v')
    
    # plot the fits
    if includeFits:
        # iterate over the peaks
        for i in range(0, len(peaks)):
          x,y = createFittedData(peaks.iloc[i, 8:12], peaks.iloc[i, 2]) 
          x = x*peaks.iloc[i,14] + peaks.iloc[i, 13]
          ax.plot(x, y, '-', linewidth=6, alpha=0.5)

    plt.show()


if __name__ == "__main__":
    main()