import argparse
import pandas as pd

import matplotlib.pyplot as plt

def plot_mz_vs_intensity(file1, file2):
    # Read the CSV files
    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    # group df1 by 'scan' and aggregate 'mz' and 'int'
    df1 = df1.groupby('scan').agg({'mz': list, 'int': list}).reset_index()

    # group df2 by 'ID' and aggregate 'MZ' and 'Intensity'
    df2 = df2.groupby('ScanNumber').agg({'MZ': list, 'Intensity': list}).reset_index()

    # iterate over each group in df1
    plt.figure()

    # For df1
    row = df1.iloc[0]
    plt.plot(row['mz'], row['int'], 'o', label='File 1')
    plt.title(f"Scan: {row['scan']}")

    # For df2
    row = df2.iloc[0]
    plt.plot(row['MZ'], row['Intensity'], 'x', label='File 2')
    plt.title(f"ScanNumber: {row['ScanNumber']}")

    # for index, row in df1.iterrows():
    #     # create a new figure for each plot
    #     # plot 'mz' against 'int' for this group
    #     plt.plot(row['mz'], row['int'], 'o', label='File 1')
    #     # set the title of the plot to the 'scan' value of this group
    #     plt.title(f"Scan: {row['scan']}")
    #     # display the plot

    # # iterate over each group in df2
    # for index, row in df2.iterrows():
    #     # create a new figure for each plot
    #     # plot 'MZ' against 'Intensity' for this group
    #     plt.plot(row['MZ'], row['Intensity'], 'x', label='File 2')
    #     # set the title of the plot to the 'ScanNumber' value of this group
    #     plt.title(f"ScanNumber: {row['ScanNumber']}")
    #     # display the plot
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Visualize LCMS profiles')
    parser.add_argument('file1', type=str, help='Path to the first CSV file')
    parser.add_argument('file2', type=str, help='Path to the second CSV file')
    args = parser.parse_args()
    plot_mz_vs_intensity(args.file1, args.file2)

# # Example usage
# file1 = 'test/data/test_lcms_data.csv'
# file2 = 'test/data/test_lcms_data_out.csv'
# plot_mz_vs_intensity(file1, file2)