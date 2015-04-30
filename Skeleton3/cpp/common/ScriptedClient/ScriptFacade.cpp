#include "ScriptFacade.h"
#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/AnimatorType.h"
#include "Data/Controller.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Util.h"
#include "Data/Animator.h"
#include "Data/Histogram.h"
#include "Data/Statistics.h"

#include <QDebug>

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    QString vmLookUp = Carta::Data::ViewManager::CLASS_NAME;
    CartaObject* obj = Carta::Data::Util::findSingletonObject( vmLookUp );
    m_viewManager = dynamic_cast<Carta::Data::ViewManager*>(obj);
}

QString ScriptFacade::getColorMapId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Colormap::CLASS_NAME, index);
}

QStringList ScriptFacade::getColorMaps() const {
    QString cmLookUp = Carta::Data::Colormaps::CLASS_NAME;
    CartaObject* obj = Carta::Data::Util::findSingletonObject( cmLookUp );
    Carta::Data::Colormaps* maps = dynamic_cast<Carta::Data::Colormaps*>(obj);
    return maps->getColorMaps();
}

QString ScriptFacade::getImageViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, index );
}

QString ScriptFacade::getAnimatorViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, index );
}

QString ScriptFacade::getHistogramViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Histogram::CLASS_NAME, index );
}

QString ScriptFacade::getStatisticsViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Statistics::CLASS_NAME, index );
}

QStringList ScriptFacade::getImageViews() {
    QStringList imageViewList;
    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        imageViewList << imageView;
    }
    return imageViewList;
}

QStringList ScriptFacade::getColorMapViews() {
    QStringList colorMapViewList;
    int numColorMaps = m_viewManager->getColormapCount();
    for (int i = 0; i < numColorMaps; i++) {
        QString colorMapView = getColorMapId( i );
        colorMapViewList << colorMapView;
    }
    return colorMapViewList;
}

QStringList ScriptFacade::getAnimatorViews() {
    QStringList animatorViewList;
    int numAnimators = m_viewManager->getAnimatorCount();
    for (int i = 0; i < numAnimators; i++) {
        QString animatorView = getAnimatorViewId( i );
        animatorViewList << animatorView;
    }
    return animatorViewList;
}

QStringList ScriptFacade::getHistogramViews() {
    QStringList histogramViewList;
    int numHistograms = m_viewManager->getHistogramCount();
    for (int i = 0; i < numHistograms; i++) {
        QString histogramView = getHistogramViewId( i );
        histogramViewList << histogramView;
    }
    return histogramViewList;
}

QStringList ScriptFacade::getStatisticsViews() {
    QStringList statisticsViewList;
    int numStatistics = m_viewManager->getStatisticsCount();
    for (int i = 0; i < numStatistics; i++) {
        QString statisticsView = getStatisticsViewId( i );
        statisticsViewList << statisticsView;
    }
    return statisticsViewList;
}

QStringList ScriptFacade::addLink( const QString& sourceId, const QString& destId ){
    QString la = m_viewManager->linkAdd( sourceId, destId );
    QStringList result(la);
    result.append(sourceId);
    result.append(destId);
    return result;
}

QStringList ScriptFacade::removeLink( const QString& sourceId, const QString& destId ){
    QString lr = m_viewManager->linkRemove( sourceId, destId );
    QStringList result(lr);
    result.append(sourceId);
    result.append(destId);
    return result;
}

QStringList ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadFile( objectId, fileName );
    QStringList result("loadFile");
    result.append(fileName);
    return result;
}

QStringList ScriptFacade::loadLocalFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadLocalFile( objectId, fileName );
    QStringList result("loadLocalFile");
    result.append(fileName);
    return result;
}

QStringList ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
    QStringList result("setAnalysisLayout");
    return result;
}

QStringList ScriptFacade::setCustomLayout( int rows, int cols ){
    QString resultStr = m_viewManager->setCustomView( rows, cols );
    QStringList result( resultStr );
    return result;
}

Carta::Data::Animator* ScriptFacade::getAnimator( const QString& index ){
    Carta::Data::Animator* animator;
    QString animId;
    int numAnimators = m_viewManager->getAnimatorCount();
    for ( int i = 0; i < numAnimators; i++ ) {
        QString animatorView = getAnimatorViewId( i );
        if ( animatorView == index ) {
            animId = m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, i );
            break;
        }
    }
    if ( animId != NULL ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( animId );
        CartaObject* obj = objMan->getObject( id );
        animator = dynamic_cast<Carta::Data::Animator*>(obj);
    }
    return animator;
}

QStringList ScriptFacade::showImageAnimator( const QString& index ){
    QString result = "failure";
    Carta::Data::Animator* animator = getAnimator( index );
    if ( animator ){
        result = "success";
        QString animId;
        animator->addAnimator( "Image", animId );
    }
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMap( colormapName );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->reverseColormap( reverseStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setCacheColormap( const QString& colormapId, const QString& cacheStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setCacheColormap( cacheStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setCacheSize( const QString& colormapId, const QString& cacheSize ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setCacheSize( cacheSize );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setInterpolatedColorMap( interpolateStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::invertColorMap( const QString& colormapId, const QString& invertStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->invertColorMap( invertStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setColorMix( const QString& colormapId, const QString& percentString ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMix( percentString );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setGamma( gamma );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setDataTransform( const QString& colormapId, const QString& transformString ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setDataTransform( transformString );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
    QStringList result("setImageLayout");
    return result;
}

QStringList ScriptFacade::setPlugins( const QStringList& names ) {
    m_viewManager->setPlugins( names );
    QStringList result("setPlugins");
    result.append(names);
    return result;
}

QStringList ScriptFacade::setChannel( const QString& animatorId, int index ) {
    Carta::Data::Animator* animator = getAnimator( animatorId );
    if ( animator ){
        Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
        if ( animType != nullptr ){
            animType->setFrame( index );
        }
        else {
            qDebug()<<"Could not get channel animator";
        }
    }
    else {
        qDebug() << "Could not find animator";
    }
    QStringList result("setChannel");
    return result;
}

QStringList ScriptFacade::setImage( const QString& animatorId, int index ) {
    Carta::Data::Animator* animator = getAnimator( animatorId );
    if ( animator ){
        animator->changeImageIndex( index );
    }
    else {
        qDebug() << "Could not find animator";
    }
    QStringList result("setImage");
    return result;
}

QStringList ScriptFacade::setClipValue( const QString& controlId, const QString& clipValue ) {
    const QString& param = "clipValue:" + clipValue;
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->setClipValue( param );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::saveImage( const QString& controlId, const QString& filename ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            bool result = controller->saveImage( filename );
            if ( !result ) {
                resultList = QStringList( "Could not save image to " + filename );
            }
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::saveFullImage( const QString& controlId, const QString& filename, double scale ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            bool result = controller->saveFullImage( filename, scale );
            if ( !result ) {
                resultList = QStringList( "Could not save full image to " + filename );
            }
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

/*
QStringList ScriptFacade::saveState( const QString& saveName ) {
    QString result = m_viewManager->saveState( "fakeSessionId", saveName, true, true, true );
    QStringList resultList(result);
    return resultList;
}
*/

QStringList ScriptFacade::getLinkedColorMaps( const QString& controlId ) {
    QStringList linkedColorMaps;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getColormapCount(); i++ ){
        QString colormapId = getColorMapId( i );
        QString id = objMan->parseId( colormapId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
            QList<QString> oldLinks = colormap->getLinks();
            if (oldLinks.contains( controlId )) {
                linkedColorMaps.append( colormapId );
            }
        }
    }
    return linkedColorMaps;
}

QStringList ScriptFacade::getLinkedAnimators( const QString& controlId ) {
    QStringList linkedAnimators;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getAnimatorCount(); i++ ){
        QString animatorId = getAnimatorViewId( i );
        QString id = objMan->parseId( animatorId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
            QList<QString> oldLinks = animator->getLinks();
            if (oldLinks.contains( controlId )) {
                linkedAnimators.append( animatorId );
            }
        }
    }
    return linkedAnimators;
}

QStringList ScriptFacade::getLinkedHistograms( const QString& controlId ) {
    QStringList linkedHistograms;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getHistogramCount(); i++ ){
        QString histogramId = getHistogramViewId( i );
        QString id = objMan->parseId( histogramId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            QList<QString> oldLinks = histogram->getLinks();
            if (oldLinks.contains( controlId )) {
                linkedHistograms.append( histogramId );
            }
        }
    }
    return linkedHistograms;
}

QStringList ScriptFacade::getLinkedStatistics( const QString& controlId ) {
    QStringList linkedStatistics;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getStatisticsCount(); i++ ){
        QString statisticsId = getStatisticsViewId( i );
        QString id = objMan->parseId( statisticsId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Statistics* statistics = dynamic_cast<Carta::Data::Statistics*>(obj);
            QList<QString> oldLinks = statistics->getLinks();
            if (oldLinks.contains( controlId )) {
                linkedStatistics.append( statisticsId );
            }
        }
    }
    return linkedStatistics;
}

QStringList ScriptFacade::centerOnPixel( const QString& controlId, double x, double y ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->centerOnPixel( x, y );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setZoomLevel( const QString& controlId, double zoomLevel ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->setZoomLevel( zoomLevel );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::getZoomLevel( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            double zoomLevel = controller->getZoomLevel( );
            resultList = QStringList( QString::number( zoomLevel ) );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::getImageDimensions( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getImageDimensions( );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::getOutputSize( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getOutputSize( );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setClipBuffer( const QString& histogramId, int bufferAmount ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipBuffer( bufferAmount );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setUseClipBuffer( const QString& histogramId, const QString& useBufferStr ) {
    QStringList resultList;
    bool validBool = false;
    bool useBuffer = Carta::Data::Util::toBool( useBufferStr, &validBool );
    if ( validBool || useBufferStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( useBufferStr.toLower() == "toggle" ) {
                    bool currentUseBuffer = histogram->getUseClipBuffer();
                    useBuffer = !currentUseBuffer;
                }
                QString result = histogram->setUseClipBuffer( useBuffer );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "this shouldn't happen." );
            }
        }
        else {
            resultList = QStringList( "Object has been removed and no longer exists?" );
        }
    }
    else {
        resultList = QStringList( "Set clip buffer parameter must be true/false or 'toggle': " + useBufferStr );
    }
    return resultList;
}

QStringList ScriptFacade::setClipRange( const QString& histogramId, double minRange, double maxRange ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipRange( minRange, maxRange );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::applyClips( const QString& histogramId, double clipMinValue, double clipMaxValue, QString mode ) {
    QStringList resultList;
    QString result = "";
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            if ( mode == "percent" ) {
                result += histogram->setClipMinPercent( clipMinValue );
                result += histogram->setClipMaxPercent( clipMaxValue );
            }
            else if ( mode == "intensity" ) {
                result += histogram->setClipMin( clipMinValue );
                result += histogram->setClipMax( clipMaxValue );
            }
            else {
                result = "invalid mode: " + mode;
            }
            histogram->applyClips();
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ) {
    QStringList resultList;
    double intensity;
    bool valid;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            valid = controller->getIntensity( frameLow, frameHigh, percentile, &intensity );
            if ( valid ) {
                resultList = QStringList( QString::number( intensity ) );
            }
            else {
                resultList = QStringList( Carta::Data::Util::toString( valid ) );
            }
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setBinCount( const QString& histogramId, int binCount ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinCount( binCount );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setBinWidth( const QString& histogramId, double binWidth ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinWidth( binWidth );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneMode( const QString& histogramId, const QString& planeMode ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneMode( planeMode );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneRange( const QString& histogramId, double minPlane, double maxPlane) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneRange( minPlane, maxPlane );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setChannelUnit( const QString& histogramId, const QString& units ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setChannelUnit( units );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setGraphStyle( const QString& histogramId, const QString& style ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setGraphStyle( style );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}

QStringList ScriptFacade::setLogCount( const QString& histogramId, const QString& logCountStr ) {
    QStringList resultList;
    bool validBool = false;
    bool logCount = Carta::Data::Util::toBool( logCountStr, &validBool );
    if ( validBool || logCountStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( logCountStr.toLower() == "toggle" ) {
                    bool currentLogCount = histogram->getLogCount();
                    logCount = !currentLogCount;
                }
                QString result = histogram->setLogCount( logCount );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "this shouldn't happen." );
            }
        }
        else {
            resultList = QStringList( "Object has been removed and no longer exists?" );
        }
    }
    else {
        resultList = QStringList( "Set log count parameter must be true/false or 'toggle': " + logCountStr );
    }
    return resultList;
}

QStringList ScriptFacade::setColored( const QString& histogramId, const QString& coloredStr ) {
    QStringList resultList;
    bool validBool = false;
    bool colored = Carta::Data::Util::toBool( coloredStr, &validBool );
    if ( validBool || coloredStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( coloredStr.toLower() == "toggle" ) {
                    bool currentColored = histogram->getColored();
                    colored = !currentColored;
                }
                QString result = histogram->setColored( colored );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "this shouldn't happen." );
            }
        }
        else {
            resultList = QStringList( "Object has been removed and no longer exists?" );
        }
    }
    else {
        resultList = QStringList( "Set colored parameter must be true/false or 'toggle': " + coloredStr );
    }
    return resultList;
}

QStringList ScriptFacade::saveHistogram( const QString& histogramId, const QString& filename, int width, int height ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->saveHistogram( filename, width, height );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "this shouldn't happen." );
        }
    }
    else {
        resultList = QStringList( "Object has been removed and no longer exists?" );
    }
    return resultList;
}
