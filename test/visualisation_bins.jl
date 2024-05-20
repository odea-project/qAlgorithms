using GLMakie
using CSV
using DataFrames
using ColorSchemes
# using CairoMakie

raw = CSV.read("../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv", DataFrame)
tdd = CSV.read("../qbinning_binlist.csv", DataFrame) # C:/Users/unisys/Documents/ ; G:/_
notbinned = CSV.read("./qbinning_notbinned.csv", DataFrame)
early_cutoff = CSV.read("./selectBins_full.csv", DataFrame)

# mzsplit1 = CSV.read("../control_cpp_mzsplit1.csv", DataFrame)
# control = CSV.read("../rawdata/df_qBinning_test.csv", DataFrame)
# wrongbins = CSV.read("C:/Users/unisys/Documents/Studium/Analytik-Praktikum/qBinning_faultybins.csv", DataFrame)

match = filter(:control_ID => !=( -1), tdd)
FN = filter(:control_ID => !=(-1), notbinned)
FP  = filter(:control_ID => ==( -1), tdd)

matchDQS = match[tdd.DQS .== tdd.control_DQSB, :] 


export_fp = filter(:scans => >=(1910), df)
export_fp = filter(:scans => <=(1960), export_fp)
export_fp = filter(:mz => <=(600), export_fp)
CSV.write("../rawdata/DQSis0.csv", DF)


# Binning Plot :glasbey_bw_minc_20_n256
fig = Figure()
Axis(fig[1, 1])

# scatter!(match.mz,match.scan,color = "black")
# scatter!(FP.mz,FP.scan,color = "red")
# scatter!(FN.mz,FN.scan,color = "blue")
scatter!(early_cutoff.mz, early_cutoff.scan, color = early_cutoff.ID)
scatter!(notbinned.mz, notbinned.scan, color = "red")

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
pwd()
