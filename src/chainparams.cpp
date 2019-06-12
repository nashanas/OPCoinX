// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017 The OPCoinX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "random.h"
#include "util.h"
#include "utilstrencodings.h"
#include "base58.h"
#include "streams.h"
#include "clientversion.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
        (0     , uint256("000007c79348b5aaf2e08c5904e6751fdcb6d705402cb5b77fa68f2718390cae"))
	(47    , uint256("000000cf41c058aedac94c7293d3db5717c1d7223cdbed969653eb3df7a6af21"))
        (463   , uint256("000000d6dbd353db653f295d89b378e2624894eb2aade701e5e1451f7bc46b93"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1526335012, // * UNIX timestamp of last checkpoint block
    467,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x001"));

static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1525482708,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1525482708,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnTrustedModulus(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

    return &ZCParams;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x21;
        pchMessageStart[1] = 0xdd;
        pchMessageStart[2] = 0xae;
        pchMessageStart[3] = 0xe1;
        vAlertPubKey = ParseHex("04621f9882ab9dc7f316b30385e4695f2b686470ff11dd0e4a62866d4d084e2bfff546ed46a2330c806e464dea584fa6e4ddb75ccc8b1abef170b0ee060a4f1bf1");
        nDefaultPort = 18051;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // OPCoinX starting difficulty is 1 / 2^12
        bnProofOfStakeLimit = (~uint256(0) >> 24);
        nSubsidyHalvingInterval = 210000;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 5 * 60; // OPCoinX: 5 minute
        nTargetSpacing = 5 * 60;  // OPCoinX: 5 minute
        nPastBlocksMin = 24;
        nLastPOWBlock = 2016;
        nMaturity = 100;
        nMasternodeCountDrift = 20;
        nModifierUpdateBlock = 0;
        nMaxMoneyOut = int64_t(800000000) * COIN;
        nModifierInterval = 60;
        nModifierIntervalRatio = 3;
        nBudgetPercent = 0;
        nDevFundPercent = 10;
        nBudgetPaymentCycle = 60*60*24*30; // 1 month
        nMaxSuperBlocksPerCycle = 100;
        nMasternodePaymentSigTotal = 10;
        nMasternodePaymentSigRequired = 6;
        nRequiredMasternodeCollateral = 37500 * COIN; //37,500

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "5th May 2018 04:37:23 : OPCoin Block Hash for Height 133342 : 00000000000ab484d6bd29779d2d7224344415e5474a8e80de7e58ed6017662e";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 250 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04c10e83b2703ccf322f7dbd62dd5855ac7c10bd055814ce121ba32607d573b8810c02c0582aed05b4deb9c4b77b26d92428c61256cd42774babea0a073b2ed0c9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1525482708;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 400025;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x000007c79348b5aaf2e08c5904e6751fdcb6d705402cb5b77fa68f2718390cae"));
        assert(genesis.hashMerkleRoot == uint256("0xa15f3ecb4e6f79d2ab863192d1f021acb600ceb01d38b29f4bfc9a1775e646c5"));

        vSeeds.push_back(CDNSSeedData("24.21.56.209", "24.21.56.209"));
        vSeeds.push_back(CDNSSeedData("134.255.231.195", "134.255.231.195"));
        vSeeds.push_back(CDNSSeedData("149.28.44.76", "149.28.44.76"));
        vSeeds.push_back(CDNSSeedData("opc1.freeddns.org", "opc1.freeddns.org"));
        vSeeds.push_back(CDNSSeedData("opc2.freeddns.org", "opc2.freeddns.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 115);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 13);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 212);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;
        nPoolMaxTransactions = 3;

        strSporkKey = "04d2b1db171839f074bc85d751dd0663b90faa017f2d78ba9713b7428891204fe462ce11e78d3c3c44c809400fc559ce342a06819851faac636cca72cadd456029";
        strObfuscationPoolDummyAddress = "oe2brxts38pc9ZkraSJc5oE5ki6oeyuGnU";
        nStartMasternodePayments = genesis.nTime + 21600; // 24 hours after genesis creation;

        /** Zerocoin */
        zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * COIN;
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 42; // medium security level for accumulators
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee

        /** Height or Time Based Activations **/
        nBlockEnforceSerialRange = std::numeric_limits<int>::max(); //Enforce serial range starting this block
        nBlockRecalculateAccumulators = std::numeric_limits<int>::max(); //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = std::numeric_limits<int>::max(); //First block that bad serials emerged
        nBlockLastGoodCheckpoint = std::numeric_limits<int>::max(); //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = std::numeric_limits<int>::max(); //Start enforcing the invalid UTXO's

        strBootstrapUrl = "https://opcx.info/bootstrap/v1/main";
    }

    CBitcoinAddress GetDevFundAddress() const
    { return CBitcoinAddress("DBKqofwU8QUFYFwNYZetyBbj2Y7oAcWLbX"); }

    CBitcoinAddress GetTxFeeAddress() const
    { return CBitcoinAddress("DEKP7sVxwwuN1mtCpTXtjua77XqFBBRaKG"); }

    CBitcoinAddress GetUnallocatedBudgetAddress() const
    { return CBitcoinAddress("DE2nWCnyYyWxoUNRg5gEeA7Kx1kpBs2spB"); }

    CBitcoinAddress Get108MAddress() const
    { return CBitcoinAddress("DAbHsptawrVEgXy4USDzpUjugVYVQCENe1"); }

    int GetChainHeight(ChainHeight ch) const
    {
        switch (ch) {
        case ChainHeight::H1:
            return 1;

        case ChainHeight::H2:
            return 151202;

        case ChainHeight::H3:
            return 302401;

        case ChainHeight::H4:
            return 388800;

        case ChainHeight::H5:
            return 5000000;

        case ChainHeight::H6:
            return 500000;

        case ChainHeight::H7:
            return 550000;

        case ChainHeight::H8:
            return 907200;

        case ChainHeight::H9:
            return 950400;

        default:
            assert(false);
            return -1;
        }
    }

    int64_t GetMinStakeAge(int nTargetHeight) const
    {
        return 60*60*6; //6 hours
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x26;
        pchMessageStart[1] = 0x72;
        pchMessageStart[2] = 0xa6;
        pchMessageStart[3] = 0xbd;
        vAlertPubKey = ParseHex("04057c908a8b772c8f11b923cbc3b58d1f6896061ba0e4912dbd62d756baf7d1e052f1b1387e786a6318f7677ac5339cf1c28a936f9b65db23906b72871b3b2d22");
        nDefaultPort = 18053;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // OPCoinX: 1 hour
        nTargetSpacing = 1 * 60;
        nPastBlocksMin = 200;
        nLastPOWBlock = 200;
        nMaturity = 15;
        nModifierUpdateBlock = 0; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = int64_t(3500000000) * COIN;
        nModifierInterval = 60;
        nModifierIntervalRatio = 3;
        nBudgetPercent = 5;
        nDevFundPercent = 10;
        nBudgetPaymentCycle = 60*60*2; // 2 hours
        nRequiredMasternodeCollateral = 37500 * COIN; //37500
        nMasternodePaymentSigTotal = 10;
        nMasternodePaymentSigRequired = 1;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1520769358;
        genesis.nNonce = 823545;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000d0a3f1e85d21f9e6bb8a8c6ebc2b994c0ea5e99b2c7f3ceeabf1ef1ccab"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 139); // Testnet opcx addresses start with 'x' or 'y'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet opcx script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet opcx BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet opcx BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet opcx BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 3;
        strSporkKey = "046a6ef3c13c172242d6ebfaa95b0a1cdb9d26d15e042c6a2d954fcf4480b03d38b807a5036035fced8385e6e135dee73fc88743481706dc6452b9bbdb897f6a83";
        strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet

        /** Height or Time Based Activations **/
        
        // height at which we start checking the serials (of the spent), duplicates etc.
        // this should be at our current height (similar to last good checkpoint), but this needs testing
        //nBlockEnforceSerialRange = 53000; // for next release? this needs testing (not stable / tested)
        nBlockEnforceSerialRange = std::numeric_limits<int>::max(); //Enforce serial range starting this block

        // some background:
        // - height at which a recalc of the accu-s will be forced (from the last good checkpoint)
        // - last-good-check < first-bad-tx < block-recalc < current-height
        // - Basically, we recalc and filter out the fraudulent outpoint-s (of the spent)
        // - i.e. this works hand in hand w/ the below first bad tx
        // the way it should be used:
        // - we set last-good, first-bad, recalc-block when we want to 'clean up', certain segment, point
        // - there should be no other checkpoints in between [last-good, block-recalc]
        // - should be after a while, when we notice first bad-tx-s and we wanna clear those.
        // (I'm guessing this was introduced w/ the feature, IMO automatic or done occasionally to clean)
        nBlockRecalculateAccumulators = std::numeric_limits<int>::max(); //Trigger a recalculation of accumulators

        // first bad-tx (spent) height, when we notice it, nothing before (should be maxed out to start w/)
        // the above (recalcs) won't even start till we set up the first bad-tx height
        nBlockFirstFraudulent = std::numeric_limits<int>::max(); //First block that bad serials emerged

        // this should be set to whatever is our current height (and until we notice any bad tx-s)
        nBlockLastGoodCheckpoint = std::numeric_limits<int>::max(); //Last valid accumulator checkpoint

        // similar to the bad-tx above, we should set this when we notice issues (w/ outputs)
        nBlockEnforceInvalidUTXO = std::numeric_limits<int>::max(); //Start enforcing the invalid UTXO's

        strSporkKey = "026ee678f254a97675a90ebea1e7593fdb53047321f3cb0560966d4202b32c48e2";
        strBootstrapUrl = "https://opcx.io/bootstrap/v1/test";
    }

    CBitcoinAddress GetDevFundAddress() const
    { return CBitcoinAddress("y4XhfKjJPwxi42YRQssbdDytJ74W8V1bVt"); }

    CBitcoinAddress GetTxFeeAddress() const
    { return CBitcoinAddress("yE8w3zvHtbn7mAFxyKk1UJEX92DWrnqzg6"); }

    CBitcoinAddress GetUnallocatedBudgetAddress() const
    { return CBitcoinAddress("yBtxR3o3uvbtkfeWLuFqa7o7yY9N1ha4Yn"); }

    CBitcoinAddress Get108MAddress() const
    { return CBitcoinAddress("xzKVwq9a9fcshaFhQLY1TS7tMbkspRsXjZ"); }

    int GetChainHeight(ChainHeight ch) const
    {
        switch (ch) {
        case ChainHeight::H1:
            return 1;

        case ChainHeight::H2:
        case ChainHeight::H3:
        case ChainHeight::H4:
            return 35500;

        case ChainHeight::H5:
        case ChainHeight::H6:
        case ChainHeight::H7:
            return 53000;

        case ChainHeight::H8:
            return 99000;

        case ChainHeight::H9:
            return 99120;

        default:
            assert(false);
            return -1;
        }
    }

    int64_t GetMinStakeAge(int nTargetHeight) const
    {
        return 60*60*8; //8 hours
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x26;
        pchMessageStart[1] = 0x72;
        pchMessageStart[2] = 0xa6;
        pchMessageStart[3] = 0xbd;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; // OPCX: 1 day
        nTargetSpacing = 1 * 60;        // OPCX: 1 minutes
        nPastBlocksMin = 200;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1525482708;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 12345;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18053;
        
        assert(hashGenesisBlock == uint256("0x647ceccaa95bcc7647fb41ece2b7b6c2e082d0ea94405fef7af88e11e0cdd2a8"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = nullptr;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

bool ParamsSelected()
{
    return pCurrentParams != nullptr;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}

uint64_t GetBlockChainSize()
{
    const uint64_t GB_BYTES = 1000000000LL;
    return 2LL * GB_BYTES;
}

bool VerifyGenesisBlock(const std::string& datadir, const uint256& genesisHash, std::string& err)
{
    const string path = strprintf("%s/blocks/blk00000.dat", datadir);
    FILE *fptr = fopen(path.c_str(), "rb");
    if (!fptr) {
        err = strprintf("Failed to open file: %s", path);
        return false;
    }

    CAutoFile filein(fptr, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull()) {
        err = strprintf("Open block file failed: %s", path);
        return false;
    }

    char buf[MESSAGE_START_SIZE] = {0};
    filein.read(buf, MESSAGE_START_SIZE);
    if (memcmp(buf, Params().MessageStart(), MESSAGE_START_SIZE)) {
        err = strprintf("Invalid magic numer %s in the file: %s", HexStr(buf, buf + MESSAGE_START_SIZE), path);
        return false;
    }

    unsigned int nSize = 0;
    filein >> nSize;
    if (nSize < 80 || nSize > 2000000) {
        err = strprintf("Invalid block size %u in the file: %s", nSize, path);
        return false;
    }

    CBlock block;
    try {
        // Read block
        filein >> block;
    } catch (std::exception& e) {
        err = strprintf("Deserialize or I/O error: %s", e.what());
        return false;
    }

    // Check block hash
    if (block.GetHash() != genesisHash) {
        err = strprintf("Block hash %s does not match genesis block hash %s", block.GetHash().ToString(), genesisHash.ToString());
        return false;
    } else
        return true;
}
