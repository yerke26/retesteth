#pragma once
#include "../../basetypes.h"
#include <retesteth/dataObject/DataObject.h>
#include <retesteth/testStructures/Common.h>

using namespace dataobject;

namespace test
{
namespace teststruct
{

struct MineBlocksResult
{
    MineBlocksResult(DataObject const& _data)
    {
        m_isRejectData = false;
        if (_data.type() == DataType::Bool)
            m_result = _data.asBool();
        else
        {
            if (_data.atKey("result").type() == DataType::Bool)
                m_result = _data.atKey("result").asBool();
            else
                m_result = _data.atKey("result").asInt();
            if (_data.count("rejectedTransactions"))
            {
                m_isRejectData = true;
                for (auto const& el : _data.atKey("rejectedTransactions").getSubObjects())
                    m_rejectedTransactions.emplace(
                                FH32(el.atKey("hash").asString()), el.atKey("error").asString());
            }
            requireJsonFields(_data, "test_mineBlocks::MineBlocksResult",
                {{"result", {{DataType::Bool, DataType::Integer}, jsonField::Required}},
                 {"rejectedTransactions", {{DataType::Array}, jsonField::Optional}}
                });
        }
    }

    bool isRejectData() const { return m_isRejectData; }

    std::string const& getTrException(FH32 const& _hash) const
    {
        static std::string empty;
        if (m_rejectedTransactions.count(_hash))
            return m_rejectedTransactions.at(_hash);
        return empty;
    }

private:
    bool m_result;
    bool m_isRejectData;
    std::map<FH32, string> m_rejectedTransactions;
};


}  // namespace teststruct
}  // namespace test
