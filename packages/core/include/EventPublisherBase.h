/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisherBase.h
 */


#ifndef RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007
#define RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007

// Library Includes
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/thread/recursive_mutex.hpp>

// Project Includes
#include "core/include/ReadWriteMutex.h"
#include "core/include/EventConnection.h"
#include "core/include/EventHub.h"
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class EventPublisherBase
{
public:
    virtual ~EventPublisherBase() {}
};

template<typename T>
class EventPublisherBaseTemplate :
    public EventPublisherBase
{
public:
    EventPublisherBaseTemplate(EventHubPtr hub = EventHubPtr(),
                               std::string name = "UNNAMED");
    
    virtual ~EventPublisherBaseTemplate() {};

    virtual EventConnectionPtr subscribe(
        T type,
        boost::function<void (EventPtr)> handler);
    

    virtual void publish(T subscribeType, Event::EventType etype,
                         EventPublisher* sender, EventPtr event);

    std::string getPublisherName();
    
protected:
    /// Implements the abstract connection class
    class Connection : public EventConnection
    {
    public:
        Connection(T type,
                   EventPublisherBaseTemplate<T>* publisher,
                   boost::signals::connection connection);
    
        virtual T getType();
        
        virtual void disconnect();

        virtual bool connected();
    private:
        /** Whether or not disconnected has been called */
        bool m_connected;
        
        /** Type of the event */
        T m_type;
        
        /** Publisher to which the event is connection */
        EventPublisherBaseTemplate* m_publisher;

        /** The underlying boost connection */
        boost::signals::connection m_connection;
    };

    typedef boost::shared_ptr<Connection> ConnectionPtr;
    
private:
    /** Remove handler from recieving particular event types */
    void unSubscribe(T type,
                     boost::signals::connection connection);
    
    // So it can call unSubscribe
    friend class Connection;

    /// Can be used to identify publishers
    std::string m_name;

    /// The hub to which all messages are puslished
    EventHubPtr m_hub;
    
    /// Protects access to map of types->signals
    ReadWriteMutex m_signalMapMutex;

    /// Protects access to each signal (they are not thread safe)
    boost::ptr_map<T, boost::recursive_mutex> m_signalMutexes;

    typedef boost::signal<void (EventPtr)>  EventSignal;
    /// Maps types -> signals
    boost::ptr_map<T, EventSignal>  m_signals;
};

// ------------------------------------------------------------------------- //
//             T E M P L A T E   I M P L E M E N T A T I O N                 //
// ------------------------------------------------------------------------- // 
    
template<typename T>
EventPublisherBaseTemplate<T>::EventPublisherBaseTemplate(EventHubPtr hub,
                                                          std::string name) :
    m_name(name),
    m_hub(hub)    
{
}
    
template<typename T>
EventConnectionPtr EventPublisherBaseTemplate<T>::subscribe(
    T type,
    boost::function<void (EventPtr)> handler)
{
    ReadWriteMutex::ScopedWriteLock lock(m_signalMapMutex);
    return EventConnectionPtr(
        new typename EventPublisherBaseTemplate<T>::Connection(type, this,
            m_signals[type].connect(handler)));
}

template<typename T>
void EventPublisherBaseTemplate<T>::publish(T subscribeType,
                                            Event::EventType etype,
                                            EventPublisher* sender,
                                            EventPtr event)
{
    // Set event property
    event->type = etype;
    event->sender = sender;

    boost::recursive_mutex* signalMutex;
    {
        ReadWriteMutex::ScopedReadLock mapLock(m_signalMapMutex);
        signalMutex = &m_signalMutexes[subscribeType];
    }
    {
        boost::recursive_mutex::scoped_lock lock(*signalMutex);
        
        // Call subscribers
        m_signals[subscribeType](event);
    }

    if (m_hub)
        m_hub->publish(event);
}

template<typename T>
std::string EventPublisherBaseTemplate<T>::getPublisherName()
{
    return m_name;
}
    
template<typename T>
void EventPublisherBaseTemplate<T>::unSubscribe(T type,
    boost::signals::connection connection)
{
    boost::recursive_mutex* signalMutex;
    {
        ReadWriteMutex::ScopedReadLock mapLock(m_signalMapMutex);
        signalMutex = &m_signalMutexes[type];
    }
    
    boost::recursive_mutex::scoped_lock lock(*signalMutex);
    connection.disconnect();
}

template<typename T>
EventPublisherBaseTemplate<T>::Connection::Connection(T type,
                   EventPublisherBaseTemplate<T>* publisher,
                   boost::signals::connection connection) :
    m_connected(true),
    m_type(type),
    m_publisher(publisher),
    m_connection(connection)
{
}

template<typename T>
T EventPublisherBaseTemplate<T>::Connection::getType()
{
    return m_type;
}

template<typename T>
void EventPublisherBaseTemplate<T>::Connection::disconnect()
{
    m_publisher->unSubscribe(m_type, m_connection);
    m_connected = false;
}

template<typename T>
bool EventPublisherBaseTemplate<T>::Connection::connected()
{
    return m_connected;
}
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHERBASE_H_11_30_2007
