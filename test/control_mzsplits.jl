using CSV
using DataFrames

splits1_control = CSV.read("../cuts.csv", DataFrame)
splits1_cpp = CSV.read("../bins_mzonly.csv", DataFrame)
splits1_control.nos = splits1_control.nos * 1000000
splits_cpp_full = CSV.read("../control_cpp_mzsplit1.csv", DataFrame)

mergeBack = innerjoin(splits1_cpp, splits1_control, on = :mzlast, makeunique = true)
mergeFront = innerjoin(splits1_cpp, splits1_control, on = :mzfirst, makeunique = true)

errorBinsFront = findall(mergeFront.size .!= mergeFront.length) # . for element-wise comparison
errorBinsBack = findall(mergeBack.size .!= mergeBack.length)

EDPs_front = mergeFront[errorBinsFront,:]
EDPs_back = mergeBack[errorBins,:]



size(filter(:splitYN => !=(1), merge))