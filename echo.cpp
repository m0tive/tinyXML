#include "tinyxml.h"
#include <time.h>
#include <fstream>

using namespace std;

int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		printf( "Loads a file, then writes it back out. Times both steps.\n" );
		printf( "Usage: echo inputfile\n" );
		return 1;
	}

	TiXmlDocument doc( argv[1] );
	
	time_t prev, now;

	prev = clock();
	
	doc.LoadFile();
	now = clock();
	printf( "Read from c-file: %fs\n", double( now-prev ) / double( CLOCKS_PER_SEC ) );
	prev = now;

#ifdef TIXML_USE_STL	
	doc.SaveFile( "echoteststl.xml" );
#else
	doc.SaveFile( "echotest.xml" );
#endif	

	now = clock();
	printf( "Write to c-file: %fs\n", double( now-prev ) / double( CLOCKS_PER_SEC ) );
	prev = now;

	doc.Clear();
	prev = clock();

#ifdef TIXML_USE_STL		
	ifstream inputStream( argv[1] );
	inputStream >> doc;
	now = clock();
	printf( "Read from stream: %fs\n", double( now-prev ) / double( CLOCKS_PER_SEC ) );
	prev = now;

	ofstream outputStream( "echoteststream.xml" );
	outputStream << doc;
	now = clock();
	printf( "Write to stream: %fs\n", double( now-prev ) / double( CLOCKS_PER_SEC ) );
	prev = now;
#endif

	return 0;
}