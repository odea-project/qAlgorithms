using CSV
using DataFrames

cpp = CSV.read("./qbinning_binlist.csv", DataFrame)
cpp = select!(cpp, Not(:control_ID, :control_DQSB))

mdist_jl = mean_distances(cpp.mz)
mdist_cpp = [3.57794068060519e-05,
3.56945540883916e-05,
3.42360129960443e-05,
3.28061535387033e-05,
3.00693226284404e-05,
2.67107184454703e-05,
2.67107184506379e-05,
2.95463195450132e-05,
3.09746809999855e-05,
3.62851790007632e-05,
4.02883193621826e-05,
5.24503411944786e-05]

cpp_longd = [3.57794068060519e-05,
3.56945540870997e-05,
3.42360129973362e-05,
3.28061535438709e-05,
3.0069322633608e-05,
2.67107184506379e-05,
2.67107184519298e-05,
2.95463195437213e-05,
3.09746809974017e-05,
3.62851790007632e-05,
4.02883193608907e-05,
5.24503411931867e-05]

cpp_longd_fmal = [3.57794068112195e-05,
3.56945540948511e-05,
3.42360129947524e-05,
3.28061535438709e-05,
3.0069322633608e-05,
2.67107184532217e-05,
2.67107184519298e-05,
2.95463195463051e-05,
3.09746809974017e-05,
3.62851789981795e-05,
4.02883193557231e-05,
5.24503411918948e-05]

mdist_diff = (mdist_jl .- mdist_cpp) .* 100000

dqscheck = innerjoin(cpp, DF, on = :mz, makeunique = true)

matchDQS = findall(dqscheck.DQS .== dqscheck.DQSbin)

# meandist as implemented in julia
function mean_distances(sorted_array::Vector{Float64})::Vector{Float64}
    # This function calculates the mean distance from one point
    # in a data set to all other points. In total this process
    # is repeated for all the individual points.
    n = length(sorted_array)
    distances = zeros(n)
    for i in 1:n # do n times
        sum = 0.0
        for j in 1:n
            if i != j
                sum += abs(sorted_array[i] - sorted_array[j]) # rounds n-1 times per i
            end
        end
        distances[i] = sum / (n - 1) # round once per n
    end
    return distances
end
# -> 

# meandist as implemented in c++

function dist_cpp(sorted_array::Vector{Float64})::Vector{Float64}
    binsize = length(sorted_array)
    totalSum = 0.0
    output = zeros(binsize)
    for i in 1:binsize
        totalSum += sorted_array[i]
    end
    beforeSum = 0.0
    for i in 1:binsize
        v1 = totalSum - sorted_array[i] * (binsize - i + 1)
        v2 = sorted_array[i] * (i - 1) - beforeSum
        beforeSum += sorted_array[i]
        totalSum -= sorted_array[i]
        output[i] = (v1 + v2) / (binsize - 1)
    end
    return output
end

dist_cpp([1.0,2.0,3.0,4.0])
mean_distances([1.0,2.0,3.0,4.0])

dist_cpp(cpp.mz)
mean_distances(cpp.mz)




print(DF)

testvec = [100.014897197396, 100.014947355816, 100.014866571885, 100.014871198788, 100.014918737361, 100.014933977592, 100.014866478547, 100.014899213561, 100.014878725073, 100.014868577379, 100.014928473274, 100.014914809367]
mean_distances(sort(testvec))