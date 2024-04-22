using GLMakie
using CSV
using DataFrames
using ColorSchemes
# using CairoMakie

raw = CSV.read("../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv", DataFrame)
tdd = CSV.read("../qbinning_binlist.csv", DataFrame) # C:/Users/unisys/Documents/ ; G:/_
notbinned = CSV.read("../qbinning_notbinned.csv", DataFrame)

# mzsplit1 = CSV.read("../control_cpp_mzsplit1.csv", DataFrame)
# control = CSV.read("../rawdata/df_qBinning_test.csv", DataFrame)
# wrongbins = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_faultybins.csv", DataFrame)

match = filter(:control_ID => !=( -1), tdd)
FN = filter(:control_ID => !=(-1), notbinned)
FP  = filter(:control_ID => ==( -1), tdd)

matchDQS = match[tdd.DQS .== tdd.control_DQSB, :] # 0.87% of DQS match exactly, not all DQS in a bin with matching DQS are identical


export_fp = filter(:Scans => >=(1913), raw)
export_fp = filter(:Scans => <=(1957), export_fp)
export_fp = filter(:Centroid => <=(200), export_fp)
CSV.write("../rawdata/reduced_DQSdiff.csv", DF)


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