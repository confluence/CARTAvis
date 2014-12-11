#include "Data/Colormaps.h"
#include "CartaLib/IColormapScalar.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "GrayColormap.h"
#include "Globals.h"
#include "PluginManager.h"

#include <QDebug>

const QString Colormaps::COLOR_LIST = "ColorMaps";
const QString Colormaps::CLASS_NAME = "Colormaps";
const QString Colormaps::COLOR_MAPS = "maps";
const QString Colormaps::COLOR_MAP_COUNT = "colorMapCount";

class Colormaps::Factory : public CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Colormaps (path, id);
        }
    };


bool Colormaps::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Colormaps::Factory());


Colormaps::Colormaps( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

void Colormaps::_initializeDefaultState(){

    // get all colormaps provided by core
    m_colormaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );

    // ask plugins for colormaps
    auto hh = Globals::instance()-> pluginManager()-> prepare < Carta::Lib::Hooks::
                                                              ColormapsScalarHook > ();

    auto lam = [=] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType &cmaps ) {
        m_colormaps.insert( m_colormaps.end(), cmaps.begin(), cmaps.end() );
    };
    hh.forEach( lam );

    int colorMapCount = m_colormaps.size();
    m_state.insertValue<int>( COLOR_MAP_COUNT, colorMapCount );
    m_state.insertArray( COLOR_MAPS, colorMapCount );
    for ( int i = 0; i < colorMapCount; i++ ){
        QString arrayIndexStr = COLOR_MAPS + StateInterface::DELIMITER + QString::number(i);
        m_state.setValue<QString>(arrayIndexStr, m_colormaps[i]->name());
    }
    m_state.flushState();
}

std::shared_ptr<Carta::Lib::IColormapScalar>  Colormaps::getColorMap( int index ) const {
    int mapCount = m_colormaps.size();
    if ( index < 0 || index  >= mapCount ) {
        CARTA_ASSERT( "colormap index out of range!" );
        return NULL;
    }
    return m_colormaps[index];
}



int Colormaps::getIndex( const QString& name ) const {
    int mapIndex = -1;
    int colorMapCount = m_colormaps.size();
    for ( int i = 0; i < colorMapCount; i++ ){
        if ( name == m_colormaps[i]->name()){
            mapIndex = i;
            break;
        }
    }
    return mapIndex;
}

Colormaps::~Colormaps(){

}