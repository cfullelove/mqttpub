#include <Poco/Task.h>
#include "MQTTPubApp.h"

#ifndef NTPS_tasks_included
#define NTPS_tasks_included

using Poco::Task;

class PubTask : public Task
{

public:
	PubTask( MQTTPubApp& app ) : Task( "PubTask" ), logger( app.logger() ), app( app )
	{
	}

	void runTask();

private:
	Poco::Logger& logger;
	MQTTPubApp& app;

};

class ConnectTask : public Task
{

public:
	ConnectTask( MQTTPubApp& app ) : Task( "ConnectTask" ), logger( app.logger() ), app( app )
	{
	}

	void runTask();

	void handleMessage( const void* pSender, MQTTMessageEventArgs& eventArgs );
	void handleConnect( const void* pSender, MQTTConnectEventArgs& eventArgs );
	void handleSubscribe( const void* pSender, MQTTSubscribeEventArgs& eventArgs );

private:
	Poco::Logger& logger;
	MQTTPubApp& app;
};

#endif