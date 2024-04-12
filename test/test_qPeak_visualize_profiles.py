import argparse
import pandas as pd
import csv
import numpy as np
import matplotlib.pyplot as plt

def plot_mz_vs_intensity(file1, file2, file3, file4, file5):
    # Read the CSV files
    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)
    df3 = pd.read_csv(file3)

    # Read the CSV files 4 and 5. These files have another layout, first column is sampleID and the variable number of columns to the individual row's end are x and y values. The first row is the header. I need them as vector of vectors.
    with open(file4, 'r') as f:
        reader = csv.reader(f)
        df4 = list(reader)

    with open(file5, 'r') as f:
        reader = csv.reader(f)
        df5 = list(reader)
    
    # Remove the header from the dataframes
    df4 = df4[1:]
    df5 = df5[1:]

    # remove the last column from the dataframes
    df4 = [row[:-1] for row in df4]
    df5 = [row[:-1] for row in df5]

    ## convert the strings to floats
    df4 = [[float(i) for i in row] for row in df4]
    df5 = [[float(i) for i in row] for row in df5]


    # group df1 by 'scan' and aggregate 'mz' and 'int'
    df1 = df1.groupby('scan').agg({'mz': list, 'int': list}).reset_index()

    # group df2 by 'ID' and aggregate 'MZ' and 'Intensity'
    df2 = df2.sort_values('MZ')
    df2 = df2.groupby('ScanNumber').agg({'MZ': list, 'Intensity': list}).reset_index()

    # group df3 by 'SampleID' and aggregate 'position' and 'height'
    df3['height'] *= -1
    df3 = df3.groupby('sampleID').agg({'position': list, 'height': list}).reset_index()
    

    # Plot the first row of the dataframes
    plt.figure()

    # For df1
    row = df1.iloc[0]
    # plt.plot(row['mz'], row['int'], 'o', label='File 1')
    # plt.title(f"Scan: {row['scan']}")

    # For df2
    row = df2.iloc[0]
    plt.plot(row['MZ'], row['Intensity'], '-', label='File 2')
    # plt.title(f"ScanNumber: {row['ScanNumber']}")

    # For df3
    row = df3.iloc[0]
    plt.stem(row['position'], row['height'], 'k', markerfmt='', linefmt='r-', basefmt='k-', label='File 3')

    # For df4 and df5 which are x and y values
    for i in range(len(df4)):
        if df4[i][0] == 0:
            x = np.array(df4[i][1:])
            y = np.array(df5[i][1:])
            _filt = np.where(x > 0)
            plt.plot(x[_filt], y[_filt], 'k--')
    plt.axis('off')
    plt.xlim(0, 1000)
    plt.tight_layout(pad=1)
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Visualize LCMS profiles')
    parser.add_argument('file1', type=str, help='Path to the first CSV file')
    parser.add_argument('file2', type=str, help='Path to the second CSV file')
    parser.add_argument('file3', type=str, help='Path to the third CSV file')
    parser.add_argument('file4', type=str, help='Path to the fourth CSV file')
    parser.add_argument('file5', type=str, help='Path to the fifth CSV file')
    args = parser.parse_args()
    plot_mz_vs_intensity(args.file1, args.file2, args.file3, args.file4, args.file5)

# # Example usage
# file1 = 'test/data/test_lcms_data.csv'
# file2 = 'test/data/test_lcms_data_out.csv'
# plot_mz_vs_intensity(file1, file2)