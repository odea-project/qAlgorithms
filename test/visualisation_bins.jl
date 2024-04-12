using GLMakie
using CSV
using DataFrames
using ColorSchemes
using CairoMakie


tdd = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qbinning_binlist.csv", DataFrame) # C:/Users/unisys/Documents/ ; G:/_
notbinned = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qbinning_notbinned.csv", DataFrame)
# control = CSV.read("../rawdata/df_qBinning_test.csv", DataFrame)
# wrongbins = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_faultybins.csv", DataFrame)

match = filter(:control_ID => !=( -1), tdd)
FN = filter(:control_ID => !=(-1), notbinned)
FP  = filter(:control_ID => ==( -1), tdd)


binned_cpp = size(tdd)[1]
binned_both = size(match)[1]
FPs_cpp = size(FP)[1]
notbinned_cpp = size(notbinned)[1]
FNs_cpp = size(FN)[1]
notbinned_both = notbinned_cpp - FNs_cpp 
binned_R = binned_both + FNs_cpp
notbinned_R = notbinned_both + FPs_cpp
size_total = binned_cpp + notbinned_cpp

ratios = [binned_both, FNs_cpp, notbinned_both, FPs_cpp]
colors = [:green, :blue, :orange, :red]

f, ax, plt = pie(ratios,
                 color = colors,
                 radius = 4,
                 inner_radius = 2,
                 strokecolor = :white,
                 strokewidth = 0,
                 axis = (autolimitaspect = 1, )
                )


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