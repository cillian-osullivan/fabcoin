// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <chainparams.h>
#include <crypto/equihash.h>
#include <primitives/block.h>
#include <streams.h>
#include <uint256.h>
#include <crypto/equihash.h>
#include <util.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.PowLimit(true)).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    uint32_t nHeight = pindexLast->nHeight + 1;

    if (nHeight < params.LWMAHeight) 
    {
        // Digishield v3.
        return DigishieldGetNextWorkRequired(pindexLast, pblock, params);
    } 
    else if (nHeight < params.LWMAHeight + params.nZawyLwmaAveragingWindow) 
    {
        // Reduce the difficulty of the first forked block by 100x and keep it for N blocks.
        if (nHeight == params.LWMAHeight) 
        {
            return ReduceDifficultyBy(pindexLast, 100, params);
        } 
        else 
        {
            return pindexLast->nBits;
        }
    } 
    else 
    {
        // Zawy's LWMA.
        return LwmaGetNextWorkRequired(pindexLast, pblock, params);
    }
}

unsigned int LwmaGetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    // Special difficulty rule for testnet:
    // If the new block's timestamp is more than 10 * T minutes
    // then allow mining of a min-difficulty block.
    if ( //params.fPowAllowMinDifficultyBlocks &&
        pblock->GetBlockTime() > pindexLast->GetBlockTime() + Params().GetnPowTargetSpacing(pindexLast->nHeight+1) * params.MaxBlockInterval) 
    {
        LogPrintf("The new block(height=%d) will come too late. Use minimum difficulty.\n", pblock->nHeight);
        return UintToArith256(params.PowLimit(true)).GetCompact();
    }
    return LwmaCalculateNextWorkRequired(pindexLast, params);
}

unsigned int LwmaCalculateNextWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
    {
        return pindexLast->nBits;
    }

    const int height = pindexLast->nHeight + 1;
    
    const int64_t T = Params().GetnPowTargetSpacing(height);
    const int N = params.nZawyLwmaAveragingWindow;
    const int k = (N+1)/2 * 0.998 * T;  // ( (N+1)/2 * adjust * T )

    assert(height > N);

    arith_uint256 sum_target, sum_last10_target,sum_last5_target;;
    int sum_time = 0, nWeight = 0;

    int sum_last10_time=0;  //Solving time of the last ten block
    int sum_last5_time=0;

    // Loop through N most recent blocks.
    for (int i = height - N; i < height; i++) {
        const CBlockIndex* block = pindexLast->GetAncestor(i);
        const CBlockIndex* block_Prev = block->GetAncestor(i - 1);
        int64_t solvetime = block->GetBlockTime() - block_Prev->GetBlockTime();

        if (params.bZawyLwmaSolvetimeLimitation && solvetime > 6 * T) {
            solvetime = 6 * T;
        }

        nWeight++;
        sum_time += solvetime * nWeight;  // Weighted solvetime sum.

        // Target sum divided by a factor, (k N^2).
        // The factor is a part of the final equation. However we divide sum_target here to avoid
        // potential overflow.
        arith_uint256 target;
        target.SetCompact(block->nBits);
        sum_target += target / (k * N * N);

        if(i >= height-10) 
        {
            sum_last10_time += solvetime;
            sum_last10_target += target;
            if(i >= height-5) 
            {
                sum_last5_time += solvetime;
                sum_last5_target += target;
            }     
        }       
    }

    // Keep sum_time reasonable in case strange solvetimes occurred.
    if (sum_time < N * k / 10) {
        sum_time = N * k / 10;
    }

    const arith_uint256 pow_limit = UintToArith256(params.PowLimit(true));
    arith_uint256 next_target = sum_time * sum_target;

#if 1
    /*if the last 10 blocks are generated in 5 minutes, we tripple the difficulty of average of the last 10 blocks*/
    if( sum_last5_time <= T ) 
    {
        arith_uint256 avg_last5_target;
        avg_last5_target = sum_last5_target/5;
        if(next_target > avg_last5_target/4)  next_target = avg_last5_target/4;   
    }
    else if(sum_last10_time <= 2 * T)
    {  
        arith_uint256 avg_last10_target;
        avg_last10_target = sum_last10_target/10;
        if(next_target > avg_last10_target/3)  next_target = avg_last10_target/3;
    }
    else if(sum_last10_time <= 5 * T)
    {            
        arith_uint256 avg_last10_target;
        avg_last10_target = sum_last10_target/10;
        if(next_target > avg_last10_target*2/3)  next_target = avg_last10_target*2/3;   
    }
    
    arith_uint256 last_target;
    last_target.SetCompact(pindexLast->nBits);       
    if( next_target > last_target * 13/10 ) next_target = last_target * 13/10;    
    /*in case difficulty drops too soon compared to the last block, especially
     *when the effect of the last rule wears off in the new block
     *DAA will switch to normal LWMA and cause dramatically diff drops*/
#endif

    if (next_target > pow_limit) {
        next_target = pow_limit;
    }

    return next_target.GetCompact();
}

unsigned int DigishieldGetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock,
                                           const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.PowLimit(true)).GetCompact();

    // Find the first block in the averaging interval
    const CBlockIndex* pindexFirst = pindexLast;
    arith_uint256 bnTot {0};
    for (int i = 0; pindexFirst && i < params.nDigishieldPowAveragingWindow; i++) {
        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindexFirst->nBits);
        bnTot += bnTmp;
        pindexFirst = pindexFirst->pprev;
    }

    // Check we have enough blocks
    if (pindexFirst == NULL)
        return nProofOfWorkLimit;

    arith_uint256 bnAvg {bnTot / params.nDigishieldPowAveragingWindow};
    return DigishieldCalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast(), params);
}


unsigned int DigishieldCalculateNextWorkRequired(arith_uint256 bnAvg, int64_t nLastBlockTime, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    // Limit adjustment
    // Use medians to prevent time-warp attacks
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    nActualTimespan = params.DigishieldAveragingWindowTimespan() + (nActualTimespan - params.DigishieldAveragingWindowTimespan())/4;

    if (nActualTimespan < params.DigishieldMinActualTimespan())
        nActualTimespan = params.DigishieldMinActualTimespan();
    if (nActualTimespan > params.DigishieldMaxActualTimespan())
        nActualTimespan = params.DigishieldMaxActualTimespan();

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.PowLimit(true));
    arith_uint256 bnNew {bnAvg};
    bnNew /= params.DigishieldAveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int ReduceDifficultyBy(const CBlockIndex* pindexLast, int64_t multiplier, const Consensus::Params& params) 
{
    arith_uint256 target;

    target.SetCompact(pindexLast->nBits);
    target *= multiplier;

    const arith_uint256 pow_limit = UintToArith256(params.PowLimit(true));
    if (target > pow_limit) 
    {
        target = pow_limit;
    }
    return target.GetCompact();
}


bool CheckEquihashSolution(const CBlockHeader *pblock, const CChainParams& params)
{

    unsigned int n = params.EquihashN(pblock->nHeight );
    unsigned int k = params.EquihashK(pblock->nHeight );

    // Hash state
    crypto_generichash_blake2b_state state;
    EhInitialiseState(n, k, state);

    // I = the block header minus nonce and solution.
    CEquihashInput I{*pblock};
    // I||V
    int ser_flags = (pblock->nHeight < (uint32_t)params.GetConsensus().ContractHeight) ? SERIALIZE_BLOCK_NO_CONTRACT : 0;
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION |ser_flags );
    ss << I;
    ss << pblock->nNonce;

    // H(I||V||...
    crypto_generichash_blake2b_update(&state, (unsigned char*)&ss[0], ss.size());

    bool isValid;
    EhIsValidSolution(n, k, state, pblock->nSolution, isValid);
    if (!isValid) {
        LogPrintf("EhIsValidSolution is invalid, n=%d k=%d ss.size=%d \npblock=%s ", n,k, ss.size(), pblock->ToString() );
        return error("CheckEquihashSolution(): invalid solution");
    }

    return true;
}


bool CheckProofOfWork(uint256 hash, unsigned int nBits, bool postfork, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.PowLimit(postfork)))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
