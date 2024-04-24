### REQUIRED PACKAGES
using SpecialFunctions
using DataFrames
using Dictionaries
using Statistics

### FOR IMPORT DATA
using CSV

### FOR PLOTTING
using GLMakie
using ColorSchemes

mutable struct qBin
    idx::Vector{Int64}
    os::Vector{Float64} ## order space
    critV::Float64
end

mutable struct qBinScore
    idx::Vector{Int64}
    scan::Vector{Int64}
    scanMin::Int64
    scanMax::Int64 
end

struct qBinCrit
    val::Vector{Float64}
end


function fsort!(df::DataFrame, cols::String)
    x = df[!,cols]
    df[sortperm(x),:]
end

function findBins!(
        mz::Vector{Float64},
        err_mz::Vector{Float64},
        rt::Vector{Float64},
        minBinSize::Int64,
        critVal_lib::qBinCrit,
        idxDictHash::Vector{Float64},
        idxDict::Dictionary{Int64, qBin},
        )
    idxDict_old_hash = copy(idxDictHash)
    # Check each openBin if it can be cut into two parts
    @time evalBins!(idxDict,idxDictHash,minBinSize,critVal_lib,:gaussian,err_mz)
    deleteSmallBins!(idxDict)
    sortQBin!(idxDict,rt,Vector{Float64}(undef,0),:discrete,critVal_lib)
    evalBins!(idxDict,idxDictHash,minBinSize,critVal_lib,:discrete,Vector{Float64}(undef,0))
    deleteSmallBins!(idxDict)
    sortQBin!(idxDict,mz,err_mz,:gaussian,critVal_lib)

    if idxDict_old_hash != idxDictHash
        findBins!(mz,err_mz,rt,minBinSize,critVal_lib,idxDictHash,idxDict)     
    end
    nothing
end

function deleteSmallBins!(idxDict::Dictionary{Int64, qBin})
    filter!(u-> u.critV > -200, idxDict)
    nothing
end



function sortQBin!(idxDict::Dictionary{Int64, qBin},criterion_val::Vector{Float64},criterion_err::Vector{Float64},model_nos::Symbol,critVal_lib::qBinCrit)
    for u in idxDict
        if model_nos == :discrete
            u.idx .= u.idx[sortperm(criterion_val[u.idx])]
            u.critV = critVal_lib.val[end] # worst case probability (0.5^(n+1) (n=6) => 0.0078125)
        end
        if model_nos == :gaussian
            sort!(u.idx)
            u.critV = critVal_lib.val[length(u.idx)]
        end
        u.os .= append!(diff(criterion_val[u.idx]),-255) 
    end
end

function createIndex(x::Vector{Vector{Float64}},critVal_lib::qBinCrit)::Dictionary{Int64,qBin}
    nTotal = length(x[1])
    crit = critVal_lib.val[nTotal]
    idx = Dictionary{Int64, qBin}()
    insert!(idx, 1, qBin(collect(1:nTotal),x[2],crit))
    return(idx)
end

function evalBins!(
        idxDict::Dictionary{Int64, qBin},
        idxDictHash::Vector{Float64},
        minBinSize::Int64,
        critVal_lib::qBinCrit,
        model_nos::Symbol,
        criterion_err::Vector{Float64}
        )
    # store current Hash from Data
    idxDict_old_hash = copy(idxDictHash)
    flag = false
    for (t,u) in zip(keys(idxDict),idxDict)
        if u.critV > 0
            if model_nos == :gaussian
                err = mean(criterion_err[u.idx])
            end
            if model_nos == :discrete
                err = 1.
            end
            if (maximum(u.os) / err) > u.critV
                updateCutIdx!(u,idxDict,minBinSize,critVal_lib,model_nos)
                flag = true
            else
                idxDict[t].critV = -1. # the -1. means that no split is required
            end
        end
    end

    if flag
        # change Hash from Data
        idxDictHash .= rand(1)
    end

    if idxDict_old_hash != idxDictHash
        evalBins!(idxDict,idxDictHash,minBinSize,critVal_lib,model_nos,criterion_err)
    end
    nothing
end

function updateCutIdx!(
                bin::qBin, 
                idxDict::Dictionary{Int64, qBin},
                minBinSize::Int64,
                critVal_lib::qBinCrit,
                model_nos::Symbol
                )
    cutIdx = argmax(bin.os)
    N = length(bin.os)
    if (cutIdx >= minBinSize) | ((N-cutIdx) >= minBinSize)
        if cutIdx >= minBinSize
            if (N-cutIdx) >= minBinSize
                model_nos == :gaussian ? insert!(idxDict, lastindex(idxDict)+1, qBin(bin.idx[cutIdx+1:end],bin.os[cutIdx+1:end],critVal_lib.val[N-cutIdx])) : nothing
                model_nos == :discrete ? insert!(idxDict, lastindex(idxDict)+1, qBin(bin.idx[cutIdx+1:end],bin.os[cutIdx+1:end],critVal_lib.val[end])) : nothing
            end
            for u in [:idx,:os]
                deleteat!(getproperty(bin,u), collect(cutIdx+1:N))
            end
            bin.os[end] = -255.
            model_nos == :gaussian ? bin.critV = critVal_lib.val[cutIdx] : nothing
            model_nos == :discrete ? bin.critV = critVal_lib.val[end] : nothing
        else
            for u in [:idx,:os]
                deleteat!(getproperty(bin,u), collect(1:cutIdx))
            end
            model_nos == :gaussian ? bin.critV = critVal_lib.val[N-cutIdx] : nothing
            model_nos == :discrete ? bin.critV = critVal_lib.val[end] : nothing
        end
    else
        bin.critV = -255. # -255. means here that the split will lead to too small bins and therefore it will be deleted later
    end
    nothing
end

function critVal(n::Vector{Int64}) ::Vector{Float64}
    3.050371658420704 * log.(n).^(-0.4771864667153769) 
end

function loadData(df::DataFrame)
    DF = deepcopy(df)
    mz = deepcopy(DF[!, "mz"])
    err_mz = deepcopy(DF[!, "error"])
    rt = deepcopy(DF[!, "scans"]) 
    nos = append!(diff(DF[!, "mz"]),-255.)#./mean(df[!, "error"])
    minBinSize = 5 # Minimum Bin Size
    minGapSize_scans = 6
    I = deepcopy(DF[!, "intensity"])
    critVal_lib = qBinCrit(append!(critVal(collect(1:length(df.mz))),Float64(minGapSize_scans)))
    idxDictHash = [1.] # Initial HashValue 
    idxDict = createIndex([mz,nos, err_mz],critVal_lib)
    nnd = deepcopy(DF[!, "nearestNeighbourDistance"])
    return (mz, err_mz, rt, nos, minBinSize, critVal_lib, idxDictHash, idxDict, I, nnd)
end

function nearestNeighbourDistance(x ::Vector{Float64})
    dist = diff(x)
    dist[dist .< 0] .= 1e50
    [i < j ? -i : j for (i,j) in zip(append!([1e50],dist),append!(dist,[1e50]))]
end

function sortLists!(x ::Vector{Vector{Float64}}, idx ::Vector{Vector{Int64}})
    for (u,v) in zip(x, idx)
        u .= u[v]
    end
    nothing
end

function mean_distances(sorted_array::Vector{Float64})::Vector{Float64}
    # This function calculates the mean distance from one point
    # in a data set to all other points. In total this process
    # is repeated for all the individual points.
    n = length(sorted_array)
    distances = zeros(n)
    for i in 1:n
        sum = 0.0
        for j in 1:n
            if i != j
                sum += abs(sorted_array[i] - sorted_array[j])
            end
        end
        distances[i] = sum / (n - 1)
    end
    return distances
end

function calculateDQSbin(mz ::Vector{Float64},nnd::Vector{Float64})::Vector{Float64}
    a = mean_distances(mz)
    A = 1 ./ (1 .+ a)
    nominator = [i > j ? i : j for (i,j) in zip(a,nnd)]
    return ((nnd .- a) .* A ./ nominator .+ 1) ./ 2
end

function nearestNeighbourDistanceWindow!(nndw ::Vector{Float64},mz ::Vector{Float64}, nnd ::Vector{Float64}, window ::Int)
    n = length(mz)
    for (u,t) in zip(1:n,nnd,nndw)
        if u-window < 1
            nndw[u] = minimum(abs.(mz[u] .- (mz[1:u+window]+nnd[1:u+window])))
        elseif u + window > n
            nndw[u] = minimum(abs.(mz[u] .- (mz[u-window:n]+nnd[u-window:n])))
        else
            nndw[u] = minimum(abs.(mz[u] .- (mz[u-window:u+window]+nnd[u-window:u+window])))
        end
    end
    nothing
end

function qBinning(df::DataFrame) ::DataFrame
    mz, err_mz, rt, nos, minBinSize, critVal_lib, idxDictHash, idxDict, I, nnd = loadData(df)
    @time findBins!(mz,err_mz,rt,minBinSize,critVal_lib,idxDictHash,idxDict)
    nndw = zeros(length(nnd))
    nearestNeighbourDistanceWindow!(nndw,mz,nnd,0)

    RT = [df[!,"rt"][u.idx] for u in idxDict]
    MZ = [mz[u.idx] for u in idxDict]
    scans = [df[!,"scans"][u.idx] for u in idxDict]
    error_fin = [df[!,"error"][u.idx] for u in idxDict]
    I = [I[u.idx] for u in idxDict]
    NND = [nndw[u.idx] for u in idxDict]
    DQScen = [df[!,"DQScen"][u.idx] for u in idxDict]
    sortRT_idxVec = [sortperm(u) for u in RT]
    sortLists!(RT, sortRT_idxVec)
    sortLists!(MZ, sortRT_idxVec)
    sortLists!(I, sortRT_idxVec)
    sortLists!(NND, sortRT_idxVec)
    sortLists!(DQScen, sortRT_idxVec)
    sortLists!(error_fin, sortRT_idxVec)
    sortLists!(scans, sortRT_idxVec)

    DQSbin = [calculateDQSbin(u,v) for (u,v) in zip(MZ, NND)]
    BinID = [ones(length(u))*v for (u,v) in zip(MZ,1:length(MZ))]

    DF = DataFrame([])
    DF[!, "mz"] = reduce(vcat, MZ)
    # DF[!, "error"] = reduce(vcat, error_fin)
    DF[!, "rt"] = reduce(vcat, RT)
    # DF[!, "scans"] = reduce(vcat, scans)
    DF[!, "intensity"] = reduce(vcat, I)
    # DF[!, "ID"] = reduce(vcat, BinID)
    # DF[!, "DQScen"] = reduce(vcat, DQScen)
    DF[!, "DQSbin"] = reduce(vcat, DQSbin)
    DF[!, "NND"] = reduce(vcat, NND)
    return DF
end

### RUN

    import_file ="./rawdata/monobin.csv" # reduced_DQSog
    # The CSV must contain at least the following labels:
    # mz, error, intensity, DQScen, rt
    df = DataFrame(CSV.File(import_file))
    # df = export_fp
    sort!(df, "rt")
    df[!, "nearestNeighbourDistance"] = nearestNeighbourDistance(df[!,"mz"])
    df.scans = cumsum(append!([1.], sign.(diff(df[!,"rt"]))))
    sort!(df, :mz)

DF = qBinning(df)

"complete"
