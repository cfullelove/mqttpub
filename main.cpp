#include <iostream>
#include <Poco/Process.h>
#include "MQTTPubApp.h"

int main(int argc, char** argv)
{
	std::string clientId( "ninjapub" );
	MQTTPubApp app( clientId );
	return app.run( argc, argv );
}
