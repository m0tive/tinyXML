#include "tinyxml.h"

// This file demonstrates some basic functionality of TinyXml.
//

int main()
{
	const char* demotest = 
		"<?xml version=\"1.0\" standalone='no'>"
		"<!-- Our to do list data -->"
		"<ToDo>"
		"<Item priority=\"1\" distance='close'> Go to the <bold>Toy store!</bold></Item>"
		"<Item priority=\"2\" distance='none'> Do bills</Item>"
		"</ToDo>";

	// The first example parses from the character string (above):
	TiXmlDocument doc( "demotest.xml" );
	doc.Parse( demotest );

	if ( doc.Error() )
	{
		printf( "Error in %s: %s\n", doc.Value().c_str(), doc.ErrorDesc().c_str() );
	}
	else
	{
		// Print should dump the character string back to the screen in
		// properly formatted XML. A good test because the buffer is
		// parsed to intermediate objects that then print themselves.
		// Bugs in the intermediate objects will result in a bad print.
		doc.Print();

		// Print out the top level 'element' objects: 
		TiXmlNode* node;
		for ( node = doc.FirstChild(); node; node = node->NextSibling() )
		{
			if ( node->ToElement() )
			{
				printf( "Top level element '%s' found\n", node->Value().c_str() );
			}
		}


		// Print out the attributes of the 'item' element in the
		// 'todo' element of the document.
		node = doc.FirstChild();		// this should be the ?xml tag
		assert( node );
		node = node->NextSibling();		// this should be the comment
		assert( node );
		node = node->NextSibling();		// this should be the todo.

		if ( node && node->Value() == "ToDo" )
		{
			// The first child node of "ToDo" should be the "Item"
			// element -- but ask for "Item" specifically.
			TiXmlNode* childNode = node->FirstChild( "Item" );
			if ( childNode && childNode->ToElement() )
			{
				// Should indeed trivially cast to an element:
				TiXmlElement*   element = childNode->ToElement();
				printf( "\nChild '%s' of 'ToDo' found. Listing attributes:\n", childNode->Value().c_str()  );
				
				// Walk the attributes of the element and dump
				// them to the screen:
				TiXmlAttribute* attribute;
				for( attribute = element->FirstAttribute(); 
					 attribute;
					 attribute = attribute->Next() )
				{
					printf( "    %s=%s\n", attribute->Name().c_str(), attribute->Value().c_str() );
				}
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
	if ( doc1.Error() ) printf( "Error in doc1: %s\n", doc1.ErrorDesc().c_str() );

	// The test2 file is generally testing bug fixes.
	TiXmlDocument doc2( "test2.xml" );
	doc2.LoadFile();
	doc2.SaveFile( "test2out.xml" );
	if ( doc2.Error() ) printf( "Error in doc2: %s\n", doc2.ErrorDesc().c_str() );
	return 0;
}

