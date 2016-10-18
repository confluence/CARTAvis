/**
 *
 **/

#pragma once

#include "CartaLib/IPCache.h"
#include "leveldb/db.h"

namespace Carta {

namespace Data {

QByteArray vd2qb( const std::vector<double> & vd);
std::vector<double> qb2vd( const QByteArray & ba);

class DiskCache : public Carta::Lib::IPCache
{
public:

public:

    DiskCache(QString dbFilePath);
    virtual uint64_t maxStorage() override
    {
        return 1;
    }
    virtual uint64_t usedStorage() override
    {
        return 1;
    }
    virtual uint64_t nEntries() override
    {
        return 1;
    }
    virtual void deleteAll() override
    {
        // not implemented
    }
    virtual bool readEntry(const QByteArray & key, QByteArray & val) override;
    virtual void setEntry(const QByteArray & key, const QByteArray & val, int64_t priority) override;

private:

    std::unique_ptr< leveldb::DB > p_db;
    leveldb::ReadOptions p_readOptions;
    leveldb::WriteOptions p_writeOptions;

};


}

}
