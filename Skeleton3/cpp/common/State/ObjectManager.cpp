
 /* CartaObjectManager.cc
 *
 *  Created on: Oct 3, 2014
 *      Author: jjacobs
 */

#include "ObjectManager.h"
#include "Globals.h"
#include "Data/Layout.h"
#include <QDebug>
#include <cassert>

using namespace std;

QList<QString> CartaObjectFactory::globalIds = {"DataLoader","Layout", "ViewManager"};

QString CartaObject::addIdToCommand (const QString & command) const {
    QString fullCommand = m_path;
    if ( command.size() > 0 ){
        fullCommand = fullCommand + m_Delimiter + command;
    }
    return fullCommand;
}

QString
CartaObject::getClassName () const
{
    return m_className;
}

QString
CartaObject::getId () const
{
    return m_id;
}

QString
CartaObject::getPath () const
{
    return m_path;
}

CartaObject::CartaObject (const QString & className,
                          const QString & path,
                          const QString & id)
: m_className (className),
  m_id (id),
  m_path (path)
{}

void
CartaObject::addCommandCallback (const QString & rawCommand, IConnector::CommandCallback callback)
{
    IConnector * connector = Globals::instance()->connector();
    QString actualCmd = addIdToCommand( rawCommand );
    //qDebug() << "AddCommandCallback actual="+actualCmd;
    connector->addCommandCallback ( actualCmd, callback);
}


int64_t CartaObject::addStateCallback( const QString& statePath, const IConnector::StateChangedCallback & cb){
    IConnector * connector = Globals::instance()->connector();
    return connector->addStateCallback( QString( addIdToCommand( statePath)), cb );
}

void CartaObject::registerView( IView * view){
    IConnector * connector = Globals::instance()->connector();
    connector->registerView( view );
}

void CartaObject::refreshView( IView* view ){
    IConnector * connector = Globals::instance()->connector();
    connector->refreshView( view );
}

void CartaObject::unregisterView(){
    IConnector * connector = Globals::instance()->connector();
    connector->unregisterView( m_id );
}

QString
CartaObject::removeId (const QString & commandAndId)
{
    // Command should have the ID as a prefix.  Strip off
    // that part plus the delimiter and return the rest.

    assert (commandAndId.count (m_Delimiter) == 1);

    return commandAndId.section (m_Delimiter, 1);
}

const QString ObjectManager::CreateObject = "CreateObject";
const QString ObjectManager::ClassName = "ClassName";
const QString ObjectManager::DestroyObject = "DestroyObject";

ObjectManager::ObjectManager ()
:m_RootPath ("/CartaObjects"),
    m_nextId (0){

}

QString ObjectManager::getRootPath() const {
    return m_RootPath;
}

QString
ObjectManager::createObject (const QString & className)
{
    // This shouldn't be called until the PW State has been initialized.

    ClassRegistry::iterator i = m_classes.find (className);

    QString result; // empty string on failure

    if (i != m_classes.end()){

        // Generate the object's id and path
        // Create the object
        CartaObjectFactory* factory = i->second.getFactory();
        QString id = factory->getGlobalId();
        if ( id.length() == 0 ){
            m_nextId ++;
            id = QString::number( m_nextId);
        }
        QString path (m_RootPath + "/" + id);


        CartaObject* object = factory->create( path, id );
        //CartaObject * object = i->second.getFactory() (path, id);

        assert (object != 0);

        // Install the newly created object in the object registry.

        assert (m_objects.find (id) == m_objects.end());

        m_objects [id] = ObjectRegistryEntry (className, id, path, object);

        result = id;

    }

    return result;
}

QString
ObjectManager::destroyObject (const QString & id)
{
    CartaObject * object = getObject (id);

    assert (object != 0);

    m_objects.erase (id);

    delete object;

    return "";
}

///Initialize the state variables that were persisted.
bool ObjectManager::readState( const QString& fileName ){
    IConnector * connector = Globals::instance()->connector();
    return connector->readState( fileName );
}

bool ObjectManager::saveState( const QString& fileName ){
    IConnector * connector = Globals::instance()->connector();
    return connector->saveState( fileName );
}

CartaObject *
ObjectManager::getObject (const QString & id)
{
    ObjectRegistry::iterator i = m_objects.find (id);

    CartaObject * result = 0;

    if (i != m_objects.end()){
        result = i->second.getObject();
    }

    return result;
}

void
ObjectManager::initialize ()
{

    // Register command handler(s) for create, etc.

    IConnector * connector = Globals::instance()->connector();
    connector->addCommandCallback (QString (CreateObject), OnCreateObject (this));
    connector->addCommandCallback (QString (DestroyObject), OnDestroyObject (this));

}

ObjectManager *
ObjectManager::objectManager ()
{
    // Implements a singleton pattern

    static ObjectManager * om = new ObjectManager ();

    return om;
}


//QString ObjectManager::onCreateObject (const QString& /*command*/,
//                               const QString& parameters,
//                               const QString& /*sessionId*/)
//{
//    QString className = parameters;
//
//    QString id = objectManager()->createObject(className);
//
//    return id;
//}
//
//QString
//ObjectManager::onDestroyObject (QString sessionid,
//                                Typeless const & command,
//                                Typeless & /* responses */)
//{
//    string className = command.Child (ClassName).ValueOr ("");
//
//    string id = objectManager()->destroyObject(className);
//
//    return "";
//}

bool
ObjectManager::registerClass (const QString & className, CartaObjectFactory * factory)
{
    assert (m_classes.find (className) == m_classes.end());

    m_classes [className] = ClassRegistryEntry (className, factory);

    return true; // failure gets "handled" by the assert for now
}

bool ExampleCartaObject::m_registered =
    ObjectManager::objectManager()->registerClass ("edu.nrao.carta.ExampleCartaObject",
                                                   new ExampleCartaObject::Factory());

const QString ExampleCartaObject::DoSomething ("DoSomething");

ExampleCartaObject::ExampleCartaObject (const QString & path, const QString & id)
: CartaObject (path, id, "edu.nrao.carta.ExampleCartaObject")
{
    //addCommandCallback (DoSomething, OnCommand (this, & ExampleCartaObject::doSomething));
    //addCommandCallback (DoSomething, wrapCommandHandler (this, & ExampleCartaObject::doSomething));
}
