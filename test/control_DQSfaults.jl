using CSV
using DataFrames
using Statistics

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

faultySummary = CSV.read("../selectbins_summary.csv", DataFrame) 
faultyBins = CSV.read("../selectbins_full.csv", DataFrame) 
notBinned = CSV.read("../qbinning_notbinned.csv", DataFrame)
rename!(notBinned, Dict(:control_DQSB => :DQSB, :control_ID => :DQSC))
# roughly the same amount of points in both datasets! - ~50% of all binned points is affected in some way

badDQSidx = faultySummary.ID[findall(faultySummary.errorcode .== 2)]

badDQS_single = filter(:ID => ==(308), faultyBins)[!,1:5]
n = size(badDQS_single)[1]
sort!(badDQS_single, :scan)
scanMin = badDQS_single.scan[1] - 6
scanMax = badDQS_single.scan[n] + 6
MIDs = dist_cpp(badDQS_single.mz)

notBinned_subset = filter(:scan => <=(scanMax), notBinned)
filter!(:scan => <=(scanMin), notBinned_subset)

badDQS = vcat(badDQS_single, notBinned)
sort!(badDQS, :mz)
idxtmp = findall(badDQS.DQSC .!= -1)
mindist_out_low = idxtmp[1]
mindist_out_up = idxtmp[lastindex(idxtmp)]
for i in 1:n
    if badDQS_single.mz[i] - badDQS.mz[mindist_out_low - 1] < badDQS.mz[mindist_out_up + 1] - badDQS_single.mz[i]
        print("a")
    else
        print("b")
    end
    
end


# badDQS[mindist_out_low, :]
# badDQS[mindist_out_up, :]