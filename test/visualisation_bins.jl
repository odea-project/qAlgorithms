using GLMakie
using CSV
using DataFrames
using ColorSchemes


tdd = CSV.read("G:/_Studium/Analytik-Praktikum/qbinning/qbinning_binlist.csv", DataFrame) # C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_binlist.csv
notbinned = CSV.read("G:/_Studium/Analytik-Praktikum/qbinning/qbinning_notbinned.csv", DataFrame)
# control = CSV.read("../rawdata/df_qBinning_test.csv", DataFrame)
# wrongbins = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_faultybins.csv", DataFrame)

match = filter(:control_ID => !=( -1), tdd)
FN = filter(:control_ID => !=(-1), notbinned)
FP  = filter(:control_ID => ==( -1), tdd)


# tdd .= sort!(tdd, [:ID,:mz])
# mz = tdd.mz
# rt = tdd.rt
# ID = tdd.ID
# colour = tdd.color
# shape = tdd.shape

# c_mz = control.mz
# c_rt = control.rt
# c_color = control.ID 

# # w_mz = wrongbins.mz
# # w_rt = wrongbins.rt

# n_mz = notbinned.mz
# n_rt = notbinned.rt

# Binning Plot :glasbey_bw_minc_20_n256
fig = Figure()
Axis(fig[1, 1])
# scatter!(mz,rt,color = colour,colormap=:tab10)
# scatter!(w_mz, w_rt, color = "red", shape = "L")
# scatter!(n_mz, n_rt, color = "black", alpha = 0.7)
# scatter!(c_mz,c_rt,color = c_color, colormap=:tab10)

scatter!(match.mz,match.scan,color = "black")
scatter!(FP.mz,FP.scan,color = "red")
scatter!(FN.mz,FN.scan,color = "blue")

DataInspector(fig)
fig