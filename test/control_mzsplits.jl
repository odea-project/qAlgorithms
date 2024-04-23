using CSV
using DataFrames

cpp = CSV.read("../qbinning_binlist.csv", DataFrame)

dqscheck = innerjoin(cpp, DF, on = :mz, makeunique = true)

matchDQS = findall(dqscheck.DQS .== dqscheck.DQSbin)



errorBinsFront = findall(mergeFront.size .!= mergeFront.length)

print(DF)

testvec = [100.014897197396, 100.014947355816, 100.014866571885, 100.014871198788, 100.014918737361, 100.014933977592, 100.014866478547, 100.014899213561, 100.014878725073, 100.014868577379, 100.014928473274, 100.014914809367]
mean_distances(sort(testvec))