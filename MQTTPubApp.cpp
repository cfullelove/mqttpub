#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/TaskManager.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Format.h>
#include <Poco/AutoPtr.h>
#include <Poco/SyslogChannel.h>
#include <Poco/Logger.h>
#include "MQTTPubApp.h"
#include "tasks.h"


using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::SyslogChannel;
using Poco::AutoPtr;
using Poco::Task;
using Poco::TaskManager;
using Poco::DateTimeFormatter;



void MQTTPubApp::initialize(Application& self)
{

	loadConfiguration();  // load default configuration files, if present

	ServerApplication::initialize(self);

}

void MQTTPubApp::uninitialize()
{
	Application::uninitialize();
}

void MQTTPubApp::handleHelp(const std::string& name, const std::string& value)
{
	_helpRequested = true;
	displayHelp();
	stopOptionsProcessing();
}

void MQTTPubApp::defineOptions(OptionSet& options)
{
	ServerApplication::defineOptions(options);

	options.addOption(
		Option("help", "", "display help information on command line arguments")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<MQTTPubApp>(this, &MQTTPubApp::handleHelp)));

	options.addOption(
		Option( "host", "h", "hostname" )
			.required( true )
			.repeatable( false )
			.argument( "host" )
			.callback( OptionCallback<MQTTPubApp>(this, &MQTTPubApp::set_host ) ) );

	options.addOption(
		Option( "topic", "t", "topic" )
			.required( true )
			.repeatable( false )
			.argument( "topic" )
			.callback( OptionCallback<MQTTPubApp>(this, &MQTTPubApp::set_topic_base ) ) );

	options.addOption(
		Option( "file", "f", "of file to publish, or - for stdin" )
			.required( true )
			.repeatable( false )
			.argument( "file" )
			.callback( OptionCallback<MQTTPubApp>(this, &MQTTPubApp::set_filename ) ) );

	options.addOption(
		Option( "name", "n", "client name" )
			.required( false )
			.repeatable( false )
			.argument( "name" )
			.callback( OptionCallback<MQTTPubApp>(this, &MQTTPubApp::set_client_name ) ) );

	options.addOption(
		Option( "debug", "d", "turn on debug logging" )
			.required( false )
			.repeatable( false )
			.noArgument()
			.callback( OptionCallback<MQTTPubApp>( this, &MQTTPubApp::set_debug ) ) );
}

void MQTTPubApp::set_filename(const std::string& name, const std::string& value)
{
	ttyFilename = new std::string( value );
}

void MQTTPubApp::set_topic_base(const std::string& name, const std::string& value)
{
	topicBase = new std::string( value );
}

void MQTTPubApp::set_host(const std::string& name, const std::string& value)
{
	host = new std::string( value );
}

void MQTTPubApp::set_client_name(const std::string& name, const std::string& value)
{
	delete clientName;
	clientName = new std::string( value );
}

void MQTTPubApp::set_debug(const std::string& name, const std::string& value)
{
	logger().debug( "Setting Debug" );
	Poco::Logger::setLevel( "", 7 );
}

void MQTTPubApp::displayHelp()
{
	HelpFormatter helpFormatter(options());
	helpFormatter.setCommand(commandName());
	helpFormatter.setUsage("OPTIONS");
	helpFormatter.format(std::cout);
}

int MQTTPubApp::main(const std::vector<std::string>& args)
{
	if (!_helpRequested)
	{
		mosq = new MQTTClient( clientName->c_str() );
		mosq->set_host( *host );

		logger().debug("Starting up");

		TaskManager tm;
		tm.start(new ConnectTask( *this ) );
		tm.start(new PubTask( *this ) );
		waitForTerminationRequest();
		tm.cancelAll();
		tm.joinAll();
		logger().debug("Shutting Down");
	}
	return Application::EXIT_OK;
}

