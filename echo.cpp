#include "tinyxml.h"
#include <time.h>

int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		printf( "Loads a file, then writes it back out. Times both steps.\n" );
		printf( "Usage: echo inputfile\n" );
		return 1;
	}

	TiXmlDocument doc( argv[1] );
	
	time_t start, read, wrote;

	start = clock();
	doc.LoadFile();
	read = clock();
	doc.SaveFile( "echotest.xml" );
	wrote = clock();

	printf( "Read: %fms  Write: %fs\n", double( read-start ) / double( CLOCKS_PER_SEC ),
										 double( wrote-read ) / double( CLOCKS_PER_SEC ) );
	


	return 0;
}