/**
 *
 **/

#include "DiskCache.h"
#include <QDebug>

namespace Carta {

namespace Data {

QByteArray vd2qb( const std::vector<double> & vd) {
    QByteArray ba;
    for( const double & d : vd) {
        ba.append( (const char *)( & d), sizeof( double));
    }
    return ba;
}

std::vector<double> qb2vd( const QByteArray & ba) {
    std::vector<double> vd;
    if( ba.size() % sizeof(double) != 0) {
        return vd;
    }
    const char * cptr = ba.constData();
    for( int i = 0 ; i < ba.size() ; i += sizeof(double)) {
        vd.push_back( * ((const double *) (cptr + i)));
    }
    return vd;
}

QByteArray id2qb( const std::pair<int, double> & id) {
    QByteArray ba;
    ba.append( (const char *)( & id[0]), sizeof( int));
    ba.append( (const char *)( & id[1]), sizeof( double));
    return ba;
}

std::pair<int, double> qb2id( const QByteArray & ba) {
    std::pair<int, double> id;
    if( ba.size() != (sizeof(double) + sizeof(int))) {
        return id;
    }
    const char * cptr = ba.constData();
    int int_val( * ((const int *) (cptr)));
    double double_val( * ((const double *) (cptr + sizeof(int))));
    id = std::make_pair(int_val, double_val);
    return id;
}

DiskCache::DiskCache(QString dbFilePath)
{
    p_readOptions = leveldb::ReadOptions();
    p_writeOptions = leveldb::WriteOptions();
    p_writeOptions.sync = false;


    // Set up database connection information and open database
    leveldb::DB * db;
    leveldb::Options options;
//    qDebug() << "write buffer size=" << options.write_buffer_size;
//    options.write_buffer_size *= 256;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open( options, dbFilePath.toStdString(), & db );

    if ( false == status.ok() ) {
        qDebug() << "Unable to open/create database '" << dbFilePath << "'" << endl;
        qDebug() << status.ToString().c_str() << endl;
        return;
    }

    p_db.reset( db );
}

bool
DiskCache::readEntry( const QByteArray & key, QByteArray & val )
{
    if ( ! p_db ) {
        return false;
    }
    std::string tmpVal;
    auto status = p_db-> Get( p_readOptions, key.constData(), & tmpVal );
    if ( ! status.ok() ) {
        return false;
    }
    val = QByteArray( tmpVal.data(), tmpVal.size());
//    val.setRawData( tmpVal.data(), tmpVal.size());
    return true;
}

void
DiskCache::setEntry( const QByteArray & key, const QByteArray & val, int64_t priority )
{
    if( ! p_db) return;
    p_db-> Put( p_writeOptions,
                leveldb::Slice( key.constData(), key.size()),
                leveldb::Slice( val.constData(), val.size()));

    Q_UNUSED( priority);
    /// \todo we'll have to embed  priority into the value, e.g. first 8 bytes?
}

}
}
