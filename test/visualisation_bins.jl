using GLMakie
using CSV
using DataFrames
using ColorSchemes
# using CairoMakie

raw = CSV.read("../rawdata/control_bins.csv", DataFrame)
tdd = CSV.read("../qbinning_binlist.csv", DataFrame) # C:/Users/unisys/Documents/ ; G:/_
notbinned = CSV.read("../qbinning_notbinned.csv", DataFrame)

mzsplit1 = CSV.read("../control_cpp_mzsplit1.csv", DataFrame)
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
size_missing = 3568033 - size_total # size of the dataset


ratios = [binned_both, FNs_cpp, notbinned_both, FPs_cpp, size_missing]
colors = [:green, :blue, :orange, :red, :black]

f, ax, plt = pie(ratios,
                 color = colors,
                 radius = 4,
                 inner_radius = 2,
                 strokecolor = :white,
                 strokewidth = 0,
                 axis = (autolimitaspect = 1, )
                )

# Binning Plot :glasbey_bw_minc_20_n256
fig = Figure()
Axis(fig[1, 1])

scatter!(match.mz,match.scan,color = "black")
scatter!(FP.mz,FP.scan,color = "red")
scatter!(FN.mz,FN.scan,color = "blue")

DataInspector(fig)
fig

mzsplit1.close = abs.(mzsplit1.maxOS - mzsplit1.vcrit)
mzsplit1_close = filter(:close => <(1), mzsplit1)
size(filter(:splitYN => ==(1), mzsplit1))
size(filter(:splitYN => ==(0), mzsplit1))

hist(mzsplit1_close.close; bins = 100)

#                Y       N
# all   : 264486; 220244; 44242
# <1000 : 171277; 127690; 43587
# <100  : 83869;  51390;  32479
# <10   : 18156;  9199;   8957
# >1    : 2022;   1008;   1014