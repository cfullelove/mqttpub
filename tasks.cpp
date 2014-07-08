#include <iostream>
#include <fstream>
#include <Poco/NumberFormatter.h>
#include <Poco/Delegate.h>
#include "MQTTClient.h"
#include "tasks.h"

using Poco::Delegate;

void PubTask::runTask()
{

	std::istream* ttyRead;

	if ( *(app.getTtyFilename()) == "-" )
	{
		logger.debug( "Publishing from stdin" );

		ttyRead = &std::cin;
	}
	else
	{
		logger.debug( "Publishing from : " + (*(app.getTtyFilename())) );
		ttyRead = new std::ifstream( app.getTtyFilename()->c_str(), std::fstream::in );
	}

	std::string line;
	std::string *topic = app.getTopic();

	MQTTClient *mosq = app.getClient();

	while ( ! isCancelled() )
	{
		if ( ! ttyRead->good() )
			break;

		std::getline( *ttyRead, line );

		logger.debug( line );

		if ( line != "" )
			mosq->publish( *topic, line );
	}

	logger.debug( "Finished Publish task" );
}

void ConnectTask::runTask()
{
	MQTTClient *mosq = app.getClient();		
	

	mosq->clientMessage += Delegate<ConnectTask, MQTTMessageEventArgs>( this, &ConnectTask::handleMessage );
	mosq->clientConnect += Delegate<ConnectTask, MQTTConnectEventArgs>( this, &ConnectTask::handleConnect );
	mosq->clientSubscribe += Delegate<ConnectTask, MQTTSubscribeEventArgs>( this, &ConnectTask::handleSubscribe );

	logger.information( "Starting MQTT Client");

	int rv = mosq->connect();
	if ( rv > 0 )
	{
		logger.error( "Failed to connect " + Poco::NumberFormatter::format( rv ) );
	}
	else
	{
		while ( ! isCancelled() )
		{
			rv = mosq->loop( 10 );
			if ( rv != MOSQ_ERR_SUCCESS )
			{
				logger.information( "Reconnecting..." );
				logger.debug( "loop() returned: " + Poco::NumberFormatter::formatHex( rv ) );
				rv = mosq->reconnect();
			}
		}

		logger.debug( "Loop finished" );
	}
	logger.debug( "Finished Subscribe task" );

}


void ConnectTask::handleMessage( const void* pSender, MQTTMessageEventArgs& eventArgs )
{
}

void ConnectTask::handleConnect( const void* pSender, MQTTConnectEventArgs& eventArgs )
{
	if ( eventArgs.rv == MOSQ_ERR_SUCCESS )
		logger.information( "Connection successful" );
}

void ConnectTask::handleSubscribe( const void* pSender, MQTTSubscribeEventArgs& eventArgs )
{
}
