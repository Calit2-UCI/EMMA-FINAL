
#include "Logger.h"

// C++
#include <cstdarg>

namespace VisageSDK
{

Logger::Logger()
{
	logfile = fopen( "vsdk.log", "w" );
	tmr.start();
}

Logger::~Logger()
{
	if( logfile != NULL )
		fclose(logfile);
}

void Logger::write( const char* msg, ... )
{
	if( logfile == NULL )
		return;

	fputs( tmr.getTimeStr().c_str(), logfile );
	fputs( "\t", logfile );

	va_list args;
	va_start( args, msg );
	vfprintf( logfile, msg, args );
	va_end(args);

	fputs( "\n", logfile );
}

}
