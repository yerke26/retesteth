#include "EthGetBlockBy.h"
#include "../Ethereum/EthereumBlock.h"
#include <retesteth/testStructures/Common.h>

using namespace std;
using namespace dev;

namespace test::teststruct
{
EthGetBlockBy::EthGetBlockBy(spDataObject& _data)
{
    try
    {
        REQUIRE_JSONFIELDS(_data, "EthGetBlockBy " + _data->getKey(),
            {{"logsBloom", {{DataType::String}, jsonField::Required}},
                {"author", {{DataType::String}, jsonField::Optional}},                 //Geth return field
                {"miner", {{DataType::String}, jsonField::Required}},
                {"difficulty", {{DataType::String}, jsonField::Required}},
                {"extraData", {{DataType::String}, jsonField::Required}},
                {"gasLimit", {{DataType::String}, jsonField::Optional}},
                {"baseFeePerGas", {{DataType::String}, jsonField::Optional}},
                {"gasUsed", {{DataType::String}, jsonField::Required}},
                {"hash", {{DataType::String}, jsonField::Required}},
                {"mixHash", {{DataType::String}, jsonField::Optional}},
                {"nonce", {{DataType::String}, jsonField::Optional}},
                {"number", {{DataType::String}, jsonField::Required}},
                {"parentHash", {{DataType::String}, jsonField::Required}},
                {"receiptsRoot", {{DataType::String}, jsonField::Required}},
                {"stateRoot", {{DataType::String}, jsonField::Required}},
                {"timestamp", {{DataType::String}, jsonField::Required}},
                {"transactionsRoot", {{DataType::String}, jsonField::Required}},
                {"withdrawalsRoot", {{DataType::String}, jsonField::Optional}},        //Shanghai field
                {"sha3Uncles", {{DataType::String}, jsonField::Required}},
                {"seedHash", {{DataType::String}, jsonField::Optional}},               //Aleth field
                {"boundary", {{DataType::String}, jsonField::Optional}},               //Aleth field
                {"miner", {{DataType::String}, jsonField::Required}},
                {"size", {{DataType::String}, jsonField::Required}},
                {"totalDifficulty", {{DataType::String}, jsonField::Required}},
                {"uncles", {{DataType::Array}, jsonField::Required}},
                {"withdrawals", {{DataType::Array}, jsonField::Optional}},
                {"transactions", {{DataType::Array}, jsonField::Required}}});


        m_header = readBlockHeader(_data); // BlockHeader verify _data fields

        m_size = spVALUE(new VALUE(_data->atKey("size")));
        m_totalDifficulty = spVALUE(new VALUE(_data->atKey("totalDifficulty")));

        m_lessobjects = false;
        for (auto& el : _data.getContent().atKeyUnsafe("transactions").getSubObjectsUnsafe())
        {
            (*el).renameKey("input", "data");
            (*el).renameKey("gas", "gasLimit");
            m_transactions.push_back(EthGetBlockByTransaction(dataobject::move(el)));
            if (!m_transactions.at(m_transactions.size() - 1).isFullTransaction())
                m_lessobjects = true;
        }

        if (_data->count("withdrawals"))
        {
            for (auto& el : _data.getContent().atKeyUnsafe("withdrawals").getSubObjectsUnsafe())
            {
                m_withdrawals.push_back(EthGetBlockByWithdrawal(dataobject::move(el)));
            }
        }

        // Remote eth_getBlockBy* always return uncles as hashes.
        for (auto const& un : _data->atKey("uncles").getSubObjects())
            m_uncles.push_back(FH32(un));
    }
    catch (std::exception const& _ex)
    {
        ETH_ERROR_MESSAGE(string("EthGetBlockBy unmarshal error: ") + _ex.what());
    }
}

bool EthGetBlockBy::hasTransaction(FH32 const& _hash) const
{
    for (auto const& tr : m_transactions)
    {
        if (tr.hash() == _hash)
            return true;
    }
    return false;
}

// Because EthGetBlockBy does not return uncle headers
BYTES EthGetBlockBy::getRLPHeaderTransactions() const
{
    EthereumBlock block(m_header);
    for (auto const& tr : m_transactions)
        block.addTransaction(tr.transaction());
    for (auto const& wt: m_withdrawals)
        block.addWithdrawal(wt.withdrawal());
    return block.getRLP();
}

}  // namespace teststruct
