using CSV
using DataFrames

splits1_control = CSV.read("../cuts.csv", DataFrame)
splits1_cpp = CSV.read("../bins_mzonly1.csv", DataFrame)
splits1_control.nos = splits1_control.mos * 1000000
splits_cpp_full = CSV.read("../control_cpp_mzsplit1.csv", DataFrame)

mergeBack = innerjoin(splits1_cpp, splits1_control, on = :mzlast, makeunique = true)
mergeFront = innerjoin(splits1_cpp, splits1_control, on = :mzfirst, makeunique = true)

errorBinsFront = findall(mergeFront.size .!= mergeFront.length) # . for element-wise comparison
errorBinsBack = findall(mergeBack.size .!= mergeBack.length)

EDPs_front = mergeFront[errorBinsFront,:]
EDPs_back = mergeBack[errorBinsBack,:]



size(filter(:splitYN => !=(1), merge))

cumsum_control = [0.0000094433211674,0.0000118582349755,0.0000098067573031,0.0000096514984490,0.0000085687029743,0.0000074769447851,0.0000095699479231,0.0000101887965052,0.0000086916595881,0.0000042250043020,0.0000128773220939,0.0000063031591851,0.0000109133955746,0.0000055852710537,0.0000149257738649,0.0000382025333611]