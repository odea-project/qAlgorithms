import argparse
import pandas as pd

import matplotlib.pyplot as plt

def plot_mz_vs_intensity(file1, file2, file3):
    # Read the CSV files
    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)
    df3 = pd.read_csv(file3)

    # group df1 by 'scan' and aggregate 'mz' and 'int'
    df1 = df1.groupby('scan').agg({'mz': list, 'int': list}).reset_index()

    # group df2 by 'ID' and aggregate 'MZ' and 'Intensity'
    df2 = df2.groupby('ScanNumber').agg({'MZ': list, 'Intensity': list}).reset_index()

    # group df3 by 'SampleID' and aggregate 'position' and 'height'
    df3 = df3.groupby('sampleID').agg({'position': list, 'height': list}).reset_index()

    # Plot the first row of the dataframes
    plt.figure()

    # For df1
    row = df1.iloc[0]
    plt.plot(row['mz'], row['int'], 'o', label='File 1')
    # plt.title(f"Scan: {row['scan']}")

    # For df2
    row = df2.iloc[0]
    plt.plot(row['MZ'], row['Intensity'], 'x', label='File 2')
    # plt.title(f"ScanNumber: {row['ScanNumber']}")

    # For df3
    row = df3.iloc[0]
    plt.stem(row['position'], row['height'], 'k', markerfmt='ko', linefmt='r-', basefmt='k-', label='File 3')

    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Visualize LCMS profiles')
    parser.add_argument('file1', type=str, help='Path to the first CSV file')
    parser.add_argument('file2', type=str, help='Path to the second CSV file')
    parser.add_argument('file3', type=str, help='Path to the third CSV file')
    args = parser.parse_args()
    plot_mz_vs_intensity(args.file1, args.file2, args.file3)

# # Example usage
# file1 = 'test/data/test_lcms_data.csv'
# file2 = 'test/data/test_lcms_data_out.csv'
# plot_mz_vs_intensity(file1, file2)