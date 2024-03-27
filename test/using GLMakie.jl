using GLMakie
using CSV
using DataFrames
using ColorSchemes


tdd = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_binlist_full.csv", DataFrame)

tdd .= sort!(tdd, [:ID,:rt])
mz = tdd.mz
rt = tdd.rt
ID = tdd.ID



#Binning Plot :glasbey_bw_minc_20_n256
fig = plot(mz,rt,color = ID,colormap=:Set1_9)
DataInspector(fig)
fig


# Binning EICs
# f = Figure()
# Axis(f[1, 1])
# lines!(rt[tdd.ID.>0],I[tdd.ID.>0],color = ID[tdd.ID.>0],colormap=ColorSchemes.tab10)
# f


# A = [2.0  1.0  1.0  3.0;     1.0  2.0 -1.0  1.0;     1.0 -1.0  2.0  2.0;     3.0  1.0  2.0  3.0]

# # initialize L with zeros
# L = zeros(4, 4)

# # first column of L
# L[1, 1] = sqrt(A[1, 1])
# L[2, 1] = A[2, 1] / L[1, 1]
# L[3, 1] = A[3, 1] / L[1, 1]
# L[4, 1] = A[4, 1] / L[1, 1]

# # second column of L
# L[2, 2] = sqrt(A[2, 2] - L[2, 1]^2)
# L[3, 2] = (A[3, 2] - L[3, 1] * L[2, 1]) / L[2, 2]
# L[4, 2] = (A[4, 2] - L[4, 1] * L[2, 1]) / L[2, 2]

# # third column of L
# L[3, 3] = sqrt(A[3, 3] - L[3, 1]^2 - L[3, 2]^2)
# L[4, 3] = (A[4, 3] - L[4, 1] * L[3, 1] - L[4, 2] * L[3, 2]) / L[3, 3]

# # fourth column of L
# L[4, 4] = sqrt(A[4, 4] - L[4, 1]^2 - L[4, 2]^2 - L[4, 3]^2)


