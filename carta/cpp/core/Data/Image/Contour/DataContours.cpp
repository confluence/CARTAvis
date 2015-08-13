#include <math.h>
#include "DataContours.h"
#include "Contour.h"

#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "CartaLib/IContourGeneratorService.h"
#include "DefaultContourGeneratorService.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataContours::CLASS_NAME = "DataContours";
const QString DataContours::CONTOURS = "contours";
const QString DataContours::CONTOUR_DRAW = "contourDraw";
const QString DataContours::SET_NAME = "name";


class DataContours::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new DataContours (path, id);
        }
    };

bool DataContours::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new DataContours::Factory());

DataContours::DataContours( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    // create the contour calculation service and hook it up
    m_contourService.reset( new Carta::Core::DefaultContourGeneratorService( this ) );
    _initializeDefaultState();
}

Contour* DataContours::_getContour(double level) {
    Contour* target = nullptr;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); it++ ){
        if ( qAbs( (*it).getLevel() - level ) < Contour::ERROR_MARGIN ){
            target = const_cast<Contour*> (&( *it ));
            break;
        }
    }
    return target;
}

QString DataContours::getName() const {
    return m_state.getValue<QString>( SET_NAME );
}

std::vector<QPen> DataContours::getPens() const {
    int penCount = m_contours.size();
    std::vector<QPen> pens( penCount );
    int i = 0;
    for ( std::set<Contour>::iterator it = m_contours.begin();
                it != m_contours.end(); it++ ){
        pens[i] = (*it).getPen();
        i++;
    }
    return pens;
}

std::shared_ptr<Carta::Lib::IContourGeneratorService> DataContours::_getRenderer(){
    return m_contourService;
}

Carta::State::StateInterface DataContours::_getState() const {
    return m_state;
}


void DataContours::_initializeCallbacks(){

}

void DataContours::_initializeDefaultState(){
    /*Contour contour1;
    contour1.setColor( 255, 0, 0 );
    contour1.setAlpha( 128 );
    contour1.setLevel( 0.25 );
    contour1.setStyle( "Solid");
    contour1.setWidth( 1 );
    m_contours.insert( contour1 );

    Contour contour2;
    contour2.setColor( 0, 255, 0 );
    contour2.setAlpha( 128 );
    contour2.setLevel( 0.5 );
    contour1.setStyle( "Dashed");
    contour2.setWidth( 2 );
    m_contours.insert( contour2 );

    Contour contour3;
    contour3.setColor( 0, 0, 255 );
    contour3.setAlpha( 128 );
    contour3.setLevel( 0.75 );
    contour3.setStyle( "Solid");
    contour3.setWidth( 3 );
    m_contours.insert( contour3 );*/

    int contourCount = m_contours.size();
    m_state.insertArray( CONTOURS, contourCount );
    m_state.insertValue<bool>( CONTOUR_DRAW, true );
    m_state.insertValue<QString>(SET_NAME, "");

    m_state.flushState();
}

void DataContours::_updateContourState( ){
    int i = 0;
    int contourCount = m_contours.size();
    std::vector<double> levels( contourCount );
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); it++ ){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        m_state.setObject( indexLookup, (*it).getStateString());
        if ( (*it).isVisible() ){
            levels[i] = (*it).getLevel();
        }
        i++;
    }
    m_contourService->setLevels( levels );
    m_state.flushState();
}

bool DataContours::isContourDraw() const {
    return m_state.getValue<bool>( CONTOUR_DRAW );
}

bool DataContours::operator<( const DataContours& other ) const {
    bool lessThan = false;
    int compareResult = QString::compare( getName(), other.getName(), Qt::CaseInsensitive );
    if ( compareResult < 0 ){
        lessThan = true;
    }
    return lessThan;
}

QString DataContours::setAlpha( std::vector<double>& levels, int transparency ){
    QString result;
    int levelCount = levels.size();
    bool alphaSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setAlpha( transparency );
            if ( result.isEmpty() ){
                alphaSet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( alphaSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QStringList DataContours::setColor( std::vector<double>& levels, int red, int green, int blue ){
    QStringList result;
    int levelCount = levels.size();
    bool colorSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setColor( red, green, blue );
            if ( result.isEmpty() ){
                colorSet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( colorSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result.append( "Unrecognized contour levels: "+unrecognizedLevels);
    }
    return result;
}

void DataContours::setContours( std::set<Contour>& contours ){
    int levelCount = contours.size();
    std::vector<double> levels( levelCount );
    m_state.resizeArray( CONTOURS, levelCount );
    m_contours.clear();
    int i = 0;
    for ( std::set<Contour>::iterator it = contours.begin();
                    it != contours.end(); it++ ){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        m_state.setObject( indexLookup, (*it).getStateString());
        levels[i] = (*it).getLevel();
        m_contours.insert( (*it) );
        i++;
    }
    m_contourService->setLevels( levels );
}

bool DataContours::setLevels( std::vector<double>& levels ){
    int levelCount = levels.size();
    bool levelsChanged = false;
    //First go through the levels and make sure there is a contour
    //for each level; add a contour if there is not already one there.
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel == nullptr ){
            Contour contour;
            contour.setLevel( levels[i] );
            m_contours.insert( contour );
            levelsChanged = true;
        }
    }

    //Now iterate through the contour levels and remove any that do not match
    //one of the levels passed in.
    for ( std::set<Contour>::iterator it = m_contours.begin();
                        it != m_contours.end(); ){
        double contourLevel = (*it).getLevel();
        bool levelFound = false;
        for ( int i = 0; i < levelCount; i++ ){
            if ( qAbs( contourLevel - levels[i] ) < Contour::ERROR_MARGIN ){
                levelFound = true;
                break;
            }
        }
        if ( !levelFound ){
            levelsChanged = true;
            m_contours.erase( it++ );
        }
        else {
            it++;
        }
    }
    if ( levelsChanged ){
        m_state.resizeArray( CONTOURS, m_contours.size() );
        _updateContourState();
    }
    return levelsChanged;
}


QString DataContours::setLineStyle( std::vector<double>& levels, const QString& lineStyle ){
    QString result;
    int levelCount = levels.size();
    bool lineStyleSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setStyle( lineStyle );
            if ( result.isEmpty() ){
                lineStyleSet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( lineStyleSet ){
        _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
        result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QString DataContours::setName( const QString& name ){
    QString result;
    if ( !name.isEmpty() && name.length() > 0 ){
        QString oldName = m_state.getValue<QString>(SET_NAME );
        if ( oldName != name ){
            m_state.setValue<QString>( SET_NAME, name );
        }
    }
    else {
        result = "Please choose a nontrivial name for the contour set.";
    }
    return result;
}

QString DataContours::setThickness( std::vector<double>& levels, double thickness ){
    QString result;
    int levelCount = levels.size();
    bool thicknessSet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            result = contourLevel->setWidth( thickness );
            if ( result.isEmpty() ){
                thicknessSet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( thicknessSet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

QString DataContours::setVisibility( std::vector<double>& levels, bool visible ){
    QString result;
    int levelCount = levels.size();
    bool visibilitySet = false;
    QString unrecognizedLevels;
    for ( int i = 0; i < levelCount; i++ ){
        Contour* contourLevel = _getContour( levels[i] );
        if ( contourLevel != nullptr ){
            bool visibilityChanged = contourLevel->setVisible( visible );
            if ( visibilityChanged ){
                visibilitySet = true;
            }
        }
        else {
            if ( unrecognizedLevels.size() > 0 ){
                unrecognizedLevels = unrecognizedLevels + ";";
            }
            unrecognizedLevels = unrecognizedLevels +QString::number(levels[i]);
        }
    }
    if ( visibilitySet ){
       _updateContourState();
    }
    if ( !unrecognizedLevels.isEmpty() ){
       result = "Unrecognized contour levels: "+unrecognizedLevels;
    }
    return result;
}

DataContours::~DataContours(){

}
}
}
