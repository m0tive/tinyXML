#include "tinyxml.h"

// This file demonstrates some basic functionality of TinyXml.
//

int main()
{
	const char* demotest = 
		"<?xml version=\"1.0\" standalone='no'>"
		"<?-- Our to do list data -->"
		"<ToDo>"
		"<Item priority=\"1\"> Go to the <bold>Toy store!</bold></Item>"
		"<Item priority=\"2\"> Do bills</Item>"
		"</ToDo>";

	// The first example parses from a character string:
	TiXmlDocument doc( "demotest.xml" );
	doc.Parse( demotest );

	if ( doc.Error() )
	{
		printf( "Error in %s: %s\n", doc.Value().c_str(), doc.ErrorDesc().c_str() );
	}
	else
	{
		doc.Print();

		// Print out the top level elements: 
		TiXmlNode* node;
		for ( node = doc.FirstChild(); node; node = node->NextSibling() )
		{
			if ( node->ToElement() )
			{
				printf( "Top level element '%s' found\n", node->Value().c_str() );
			}
		}
	}
	doc.SaveFile( "demotestout.xml" );

	// Now we parse from a file, and save again, to check the output.
	// The output should match the input, perhaps somewhat reformatted.
	TiXmlDocument doc0( "test0.xml" );
	doc0.LoadFile();
	doc0.SaveFile( "test0out.xml" );
	if ( doc0.Error() ) printf( "Error in doc0: %s\n", doc0.ErrorDesc().c_str() );

	TiXmlDocument doc1( "test1.xml" );
	doc1.LoadFile();
	doc1.SaveFile( "test1out.xml" );
	if ( doc1.Error() ) printf( "Error in doc0: %s\n", doc1.ErrorDesc().c_str() );

	return 0;
}
