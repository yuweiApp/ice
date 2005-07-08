// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_DATABASE_H
#define ICE_GRID_DATABASE_H

#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <Freeze/ConnectionF.h>
#include <Ice/CommunicatorF.h>
#include <IceGrid/Admin.h>
#include <IceGrid/Internal.h>
#include <IceGrid/StringApplicationDescriptorDict.h>
#include <IceGrid/IdentityObjectInfoDict.h>
#include <IceGrid/StringObjectProxyDict.h>
#include <IceGrid/ServerCache.h>
#include <IceGrid/NodeCache.h>
#include <IceGrid/ObjectCache.h>
#include <IceGrid/AdapterCache.h>

namespace IceGrid
{

class TraceLevels;
typedef IceUtil::Handle<TraceLevels> TraceLevelsPtr;

class NodeSessionI;
typedef IceUtil::Handle<NodeSessionI> NodeSessionIPtr;

class ObserverSessionI;

class ServerEntry;
typedef IceUtil::Handle<ServerEntry> ServerEntryPtr;

class Database : public IceUtil::Shared, public IceUtil::Mutex
{
public:
    
    Database(const Ice::ObjectAdapterPtr&, const std::string&, int, const TraceLevelsPtr&);
    virtual ~Database();

    void setObservers(const RegistryObserverPrx&, const NodeObserverPrx&);

    void lock(int serial, ObserverSessionI*, const std::string&);
    void unlock(ObserverSessionI*);

    void addApplicationDescriptor(ObserverSessionI*, const ApplicationDescriptorPtr&);
    void updateApplicationDescriptor(ObserverSessionI*, const ApplicationUpdateDescriptor&);
    void syncApplicationDescriptor(ObserverSessionI*, const ApplicationDescriptorPtr&);
    void removeApplicationDescriptor(ObserverSessionI*, const std::string&);

    ApplicationDescriptorPtr getApplicationDescriptor(const std::string&);
    Ice::StringSeq getAllApplications(const std::string& = std::string());

    void addNode(const std::string&, const NodeSessionIPtr&);
    NodePrx getNode(const std::string&) const;
    void removeNode(const std::string&);
    Ice::StringSeq getAllNodes(const std::string& = std::string());
    
    ServerInstanceDescriptor getServerDescriptor(const std::string&);
    std::string getServerApplication(const std::string&);
    ServerPrx getServer(const std::string&);
    ServerPrx getServerWithTimeouts(const std::string&, int&, int&);
    Ice::StringSeq getAllServers(const std::string& = std::string());
    Ice::StringSeq getAllNodeServers(const std::string&);

    void setAdapterDirectProxy(const std::string&, const Ice::ObjectPrx&);
    Ice::ObjectPrx getAdapterDirectProxy(const std::string&);
    AdapterPrx getAdapter(const std::string&, const std::string& = std::string());
    Ice::StringSeq getAllAdapters(const std::string& = std::string());

    void addObject(const ObjectInfo&);
    void removeObject(const Ice::Identity&);
    void updateObject(const Ice::ObjectPrx&);
    Ice::ObjectPrx getObjectProxy(const Ice::Identity&, std::string&);
    Ice::ObjectPrx getObjectByType(const std::string&);
    Ice::ObjectProxySeq getObjectsWithType(const std::string&);
    ObjectInfo getObjectInfo(const Ice::Identity&);
    ObjectInfoSeq getAllObjectInfos(const std::string& = std::string());

    const TraceLevelsPtr& getTraceLevels() const;
    int getNodeSessionTimeout() const;

private:

    void syncApplicationDescriptorNoSync(const ApplicationDescriptorPtr&, const ApplicationDescriptorPtr&, 
					 ServerEntrySeq&);

    void addServers(const std::string&, const ServerInstanceDescriptorSeq&, const std::set<std::string>&, 
		    ServerEntrySeq&);
    void updateServers(const ApplicationDescriptorPtr&, const ApplicationDescriptorPtr&,
		       const std::set<std::string>&, ServerEntrySeq&);
    void removeServers(const std::string&, const ServerInstanceDescriptorSeq&, const std::set<std::string>&, 
		       ServerEntrySeq&);

    ServerEntryPtr addServer(const std::string&, const ServerInstanceDescriptor&);
    ServerEntryPtr updateServer(const ServerInstanceDescriptor&);
    ServerEntryPtr removeServer(const std::string&, const ServerInstanceDescriptor&);

    void checkServerForAddition(const std::string&);
    void checkAdapterForAddition(const std::string&);
    void checkObjectForAddition(const Ice::Identity&);

    void checkSessionLock(ObserverSessionI*);

    friend struct AddComponent;

    static const std::string _descriptorDbName;
    static const std::string _objectDbName;
    static const std::string _adapterDbName;

    const Ice::CommunicatorPtr _communicator;
    const Ice::ObjectAdapterPtr _internalAdapter;
    const std::string _envName;
    const int _nodeSessionTimeout;
    const TraceLevelsPtr _traceLevels;

    NodeCache _nodeCache;
    ObjectCache _objectCache;
    AdapterCache _adapterCache;
    ServerCache _serverCache;

    RegistryObserverPrx _registryObserver;
    NodeObserverPrx _nodeObserver;

    std::map<std::string, std::string> _applicationsByServerName;
 
    Freeze::ConnectionPtr _connection;
    StringApplicationDescriptorDict _descriptors;
    IdentityObjectInfoDict _objects;
    StringObjectProxyDict _adapters;
    
    ObserverSessionI* _lock;
    std::string _lockUserId;
    int _serial;
};
typedef IceUtil::Handle<Database> DatabasePtr;

};

#endif
