// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
		(0, uint256("0x000032661a7011e49ee28d45efce8208a9820a96ba9daf74d9a36451a740043d"))
		(1, uint256("0x99f0be26ff63a62280dfb9e8a041014591d74af2030017ab5177347afeaeb93d"))
		(2, uint256("0x1af29801364a632dd1539ab7e49ddffa78853c609e3da1ea2be6e6c21b813d4d"))
		(19, uint256("0x75f66cc2e70176db9a8ec72ac75935845b68ef8a2ef5d3b01f63e6964a8d2db5"))
		(69, uint256("0x42a46eb20103df84431327272700c5946321a06f71aae2e3c43226f1934c9e02"))
		(84, uint256("0xb21f8f6259b9b617717d4e6fb41f3378052864e21924aa73699427ade6ba9cd0"))
		(98, uint256("0x196b6b74d7462e4c4bc2286f5b7ac23e0e8222cf36e010ac0ccc6810432059ad"))
		(101, uint256("0x282a7f880e1d42b2f3e395a81c7d7ebdabac0cd27c730d35572cd1a4c7b0c569"))
		(220, uint256("0x4f1ea16d26a5f931f8674ce73de1bcafbc6b5047f32c3b2b0ee01b31a0960be8"))

    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
