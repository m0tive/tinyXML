/*
Copyright (c) 2000 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/


#include "tinyxml.h"
#include <ctype.h>
#include <strstream>
using namespace std;


bool TiXmlBase::SkipWhiteSpace( istream* in )
{
	assert( in );
//	while ( p && *p && 
//	        ( isspace( *p ) || *p == '\n' || *p == '\r' ) )
//		p++;
//	return p;

	while ( in->good() )
	{
		int c = in->peek();
		if ( isspace( c ) || c == '\n' || c =='\r' )		// Still using old rules for white space.
			in->get();
		else
			break;
	}

	return ( in->good() );
}

bool TiXmlBase::ReadName( istream* in, string* name )
{
	*name = "";
//	const char* start = p;
	assert( in );

	// Names start with letters or underscores.
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	int c = in->peek();

	if (    in->good() 
		 && ( isalpha( c ) || c == '_' ) )
	{
		while(		in->good()
				&&	(		isalnum( c ) 
						 || c == '_'
						 || c == '-'
						 || c == ':' ) )
		{
			in->get();		// pull c off the stream.
			(*name) += c;

			// Check next time around:
			c = in->peek();
		}
//		name->append( start, p - start );
//		return p;
		return true;
	}
	return false;
//	return 0;
}


TiXmlBase::Entity TiXmlBase::entity[ NUM_ENTITY ] = 
{
	{ "&amp;",  4, '&' },
	{ "&lt;",   3, '<' },
	{ "&gt;",   3, '>' },
	{ "&quot;", 5, '\"' },
	{ "&apos;", 5, '\'' }
};


void TiXmlBase::GetEntity( istream* in, int* value )
{
	assert( in );
	
	// Presume an entity, and pull it out.
	int pulled = 0;
	string ent;
	int c = 0;
	int i;

	while ( pulled < MAX_ENTITY_LENGTH )
	{
		c = in->get();
		ent += ((char) c );
		++pulled;

		if ( c == ';' )
			break;
	}	

	// Now try to match it.
	for( i=0; i<NUM_ENTITY; ++i )
	{
		//if ( strncmp( in, entity[i].str, entity[i].strLength ) == 0 )
		if ( ent.compare( entity[i].str ) == 0 )
		{
			*value = entity[i].chr;
			return;
		}
	}
	
	// So it wasn't an entity, its unrecognized, or something like that.
	// Pull one character, but put the rest back.
	if ( ent.empty() )
	{
		*value = 0;
		return;
	}
	for( i=ent.length()-1; i>0; --i )
	{
		in->putback( ent[i] );
	}
	*value = ent[0];	// Don't put back the last one!
}


bool TiXmlBase::StreamEqual( istream* in, int numEndTag, const char** endTag, bool caseInsensitive )
{
	int i;
	assert( in );
	bool match = false;

	for( i=0; i<numEndTag; ++i )
	{
		int c = in->peek();
		if ( c == *endTag[i] )
		{
			// First letters match.
			string str;
			int len = strlen( endTag[i] );

			int j;
			for( j=0; j<len; ++j )
			{
				c = in->get();
				if ( c < 0 ) break;

				str += (char) c;
				if ( caseInsensitive )
				{
					if ( tolower( c ) != tolower( *( endTag[i] + j ) ) ) 
						break;
				}
				else
				{
					if ( c != *( endTag[i] + j ) ) 
						break;
				}
			}
			if ( j == len )
			{
				// We have a match!
				match = true;
			}

			// Push back the str to the buffer.
			for( j=str.length()-1; j>=0; --j )
			{
				in->putback( str[j] );
				assert( in->good() );
			}

			// If we have a match, return out, else check the next tag.
			if ( match )
				return true;
		}
	}
	return false;
}


bool TiXmlBase::ReadText(	istream* in, 
							string* text, 
							bool ignoreWhiteSpace, 
							int numEndTag, const char* endTag[], bool caseInsensitive,
							bool endOnWhite )
{
	assert( in );
	*text = "";

	if ( !ignoreWhiteSpace )
	{
		// Keep all the white space.
		while ( in->good()
				&& !StreamEqual( in, numEndTag, endTag, caseInsensitive ) )
		{
			char c;
			int  i;
			GetChar( in, &i );
			c = (char) i;
			text->append( &c, 1 );
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		SkipWhiteSpace( in );
		while (	   ( in->peek() > 0 )
				&& !StreamEqual( in, numEndTag, endTag, caseInsensitive ) )
		{
			if ( in->peek() == '\r' || in->peek() == '\n' )
			{
				whitespace = true;
				in->get();
			}
			else if ( isspace( in->peek() ) )
			{
				if ( endOnWhite )
				{
					break;
				}
				else
				{
					whitespace = true;
					in->get();
				}
			}
			else
			{
				// If we've found whitespace, add it before the
				// new character. Any whitespace just becomes a space.
				if ( whitespace )
				{
					text->append( " ", 1 );
					whitespace = false;
				}
				int i;
				char c;
				GetChar( in, &i );
				c = ( char ) i;
				text->append( &c, 1 );
			}
		}
//		// Keep white space before the '<' 
//		if ( whitespace )
//		{
//			text->append( " ", 1 );
//		}
	}
//	return p;
	//assert( in->good() );
	return true;
}


bool TiXmlDocument::Parse( istream* in )
{
	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	//

	// Parsing ends when we hit a declaration (except for the first),
	// or don't hit an opening tag.
	
 	SkipWhiteSpace( in );

	if ( !in->good() )
	{
		// If we are empty, this is an error, else it's just the end.
		if ( NoChildren() )
		{
			SetError( TIXML_ERROR_DOCUMENT_EMPTY );
			return false;
		}
		else
		{
			return true;
		}
	}
	
	while ( in->good() )
	{	
		int c = in->peek();

		if ( c != '<' )
		{
			// If we are empty, this is an error, else it's just the end.
			if ( NoChildren() )
			{
				SetError( TIXML_ERROR_PARSING_ELEMENT );
				return false;
			}
			break;
		}
		else
		{
			TiXmlNode* node = Identify( in );
			if ( node )
			{
				node->Parse( in );
				LinkEndChild( node );
			}		
			else
			{
				break;
			}		
		}
		SkipWhiteSpace( in );
	}
	// All is well.
	return true;
}


TiXmlNode* TiXmlNode::Identify( istream* in )
{
	TiXmlNode* returnNode = 0;

	SkipWhiteSpace( in );
	if( !in->good() || in->peek() != '<' )
	{
		return 0;
	}

	TiXmlDocument* doc = GetDocument();
	SkipWhiteSpace( in );

	if ( !in->good() )
		return 0;

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//

//	if ( 	   tolower( *(q+0) ) == '?'
//			&& tolower( *(q+1) ) == 'x' 
//			&& tolower( *(q+2) ) == 'm'
//			&& tolower( *(q+3) ) == 'l' )
//	{
	const char* xmlHeader[] = { "<?xml" };
	const char* commentHeader[] = { "<!--" };

	int c = in->get();		// the current character,
	int nextC = in->get();	// the next character:
	in->putback( nextC );	// And we put them back. The information is for look-ahead.
	in->putback( c );

	if ( StreamEqual( in, 1, xmlHeader, true ) )
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Declaration\n" );
		#endif
		returnNode = new TiXmlDeclaration();
	}
	else if (    isalpha( nextC ) 
			  || nextC == '_' )
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Element\n" );
		#endif
		returnNode = new TiXmlElement( "" );
	}
	else if ( StreamEqual( in, 1, commentHeader, false ) )
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Comment\n" );
		#endif
		returnNode = new TiXmlComment();
	}
	else
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Unknown\n" );
		#endif
		returnNode = new TiXmlUnknown();
	}

	if ( returnNode )
	{
		// Set the parent, so it can report errors
		returnNode->parent = this;
		//p = returnNode->Parse( p );
	}
	else
	{
		if ( doc )
			doc->SetError( TIXML_ERROR_OUT_OF_MEMORY );
	}
	return returnNode;
}


bool TiXmlElement::Parse( istream* in )
{
	SkipWhiteSpace( in );
	TiXmlDocument* document = GetDocument();

	if ( !in->good() || in->peek() != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT );
		return false;
	}

	in->get();
	SkipWhiteSpace( in );

	// Read the name.
	ReadName( in, &value );
	if ( !in->good() )
	{
		if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME );
		return false;
	}

	string endTag = "</";
	endTag += value;
	endTag += ">";

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( in->good() )
	{
		SkipWhiteSpace( in );
		if ( !in->good() )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
			return false;
		}
		int c = in->peek();

		if ( c == '/' )
		{
			in->get();		// get the '/'
			// Empty tag.
			c = in->peek();
			if ( c  != '>' )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY );		
				return false;;
			}
			in->get();		// get the '>'
			return true;
		}
		else if ( c == '>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			in->get();
			ReadValue( in );		// Note this is an Element method, and will set the error if one happens.
			if ( !in->good() )
				return false;

			// We should find the end tag now
			if ( StreamEqual( in, endTag.c_str(), false ) )
			{
				in->ignore( endTag.length() );		// pull the endtag off the stream.
				return true;
			}
			else
			{
				if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG );
				return false;
			}
		}
		else
		{
			// Try to read an element:
			TiXmlAttribute attrib;
			attrib.SetDocument( document );
			attrib.Parse( in );

			if ( !in->good() )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT );
				return false;
			}
			SetAttribute( attrib.Name(), attrib.Value() );
		}
	}
	return true;
}


bool TiXmlElement::ReadValue( istream* in )
{
	TiXmlDocument* document = GetDocument();

	// Read in text and elements in any order.
	SkipWhiteSpace( in );
	while ( in->good() )
	{
//		const char* start = p;

		string text;
		while ( in->good() && in->peek() != '<' )
		{
			int c = in->get();
			assert( c >= 0 );
			text += c;
		}
//		while ( *p && *p != '<' )
//			p++;

		SkipWhiteSpace( in );

		if ( !text.empty() )
		{
			// Take what we have, make a text element.
			TiXmlText* textNode = new TiXmlText();

			if ( !textNode )
			{
				if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
				return 0;
			}
			istrstream itext( text.c_str(), text.length() );

			textNode->Parse( &itext );

			if ( !textNode->Blank() )
				LinkEndChild( textNode );
			else
				delete textNode;
		} 
		else 
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if ( StreamEqual( in, "</", false ) )
			{
				return true;
			}
			else
			{
				TiXmlNode* node = Identify( in );
				if ( node )
				{
					node->Parse( in );
					LinkEndChild( node );
				}				
				else
				{
					return false;
				}
			}
		}
		SkipWhiteSpace( in );
	}

	if ( !in->good() )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE );
		return false;
	}	
	return true;
}


bool TiXmlUnknown::Parse( istream* in )
{
	TiXmlDocument* document = GetDocument();
	SkipWhiteSpace( in );
	if ( !in->good() || in->peek() != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN );
		return false;
	}
	in->get();		// opening '<'
	int c = in->get();
	value = "";

	while ( c > 0 && c != '>' )
	{
		value += c;
		c = in->get();
	}

	if ( c <= 0 )
	{
		if ( document )	document->SetError( TIXML_ERROR_PARSING_UNKNOWN );
		return false;
	}
	return true;
}


bool TiXmlComment::Parse( istream* in )
{
	TiXmlDocument* document = GetDocument();
	value = "";

	SkipWhiteSpace( in );

	if ( !StreamEqual( in, "<!--", false ) )
	{
		document->SetError( TIXML_ERROR_PARSING_COMMENT );
		return false;
	}
	
	in->ignore( 4 );
	const char* endTag[1];
	endTag[0] = "-->";

	ReadText( in, &value, true, 1, endTag, false, false );

	if ( StreamEqual( in, endTag[0], false ) )
	{
		in->ignore( 3 );
		return true;
	}
	return false;

	// Find the end, copy the parts between to the value of
	// this object, and return.
//	int c;
//	while( in->good() && !StreamEqual( in, "-->", false ) )
//	{
//		GetChar( in, &c );
//		value += c;
//	}
//
//	if ( !in->good() )
//	{
//		if ( document )	document->SetError( TIXML_ERROR_PARSING_COMMENT );
//		return false;
//	}
//
//	in->ignore( 3 );		// the '-->'
//	return in->good();

//	else
//	{
//		TiXmlDocument* doc = GetDocument();
//		bool ignoreWhite = true;
//		if ( doc && !doc->IgnoreWhiteSpace() )
//			ignoreWhite = false;
//
//		const char* end[1] = { "-->" };
//		p = ReadText( start, &value, ignoreWhite, 1, end, false ); 
//
//		if ( !p )
//		{
//			return 0;
//		}
//		return p + 3;			// return just past the '>'
//	}
}


bool TiXmlAttribute::Parse( istream* in )
{
	SkipWhiteSpace( in );
	// Read the name, the '=' and the value.
	ReadName( in, &name );
	if ( !in->good() )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return false;
	}
	SkipWhiteSpace( in );
	if ( !in->good() || in->peek() != '=' )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return false;
	}

	in->ignore( 1 );
	SkipWhiteSpace( in );
	if ( !in->good() )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return false;
	}
	
	const char* end[8];

	if ( in->peek() == '\'' )
	{
		in->ignore( 1 );
		end[0] = "\'";
		ReadText( in, &value, false, 1, end, false, false );
		in->ignore( 1 );	// get past the end '
	}
	else if ( in->peek() == '"' )
	{
		in->ignore( 1 );
		end[0] = "\"";
		ReadText( in, &value, false, 1, end, false, false );
		in->ignore( 1 );	// get past the end "
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		end[0] = "/";
		end[1] = ">";
		in->ignore( 1 );
		ReadText( in, &value, false, 2, end, false, true );
	}
	return ( !value.empty() );
}


bool TiXmlText::Parse( istream* in )
{
	value = "";
//	value.reserve( 256 );	// Avoid a bunch of small allocations in the beginning. Give ourselves some memory.

	TiXmlDocument* doc = GetDocument();
	bool ignoreWhite = true;
	if ( doc && !doc->IgnoreWhiteSpace() ) ignoreWhite = false;

	const char* end[1] = { "<" };
	return ReadText( in, &value, ignoreWhite, 1, end, false, false );
}


bool TiXmlDeclaration::Parse( istream* in )
{
	SkipWhiteSpace( in );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	TiXmlDocument* document = GetDocument();
	if ( !StreamEqual( in, "<?xml", true ) )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION );
		return false;
	}

	in->ignore( 5 );
//	const char* start = p+5;
//	const char* end  = strstr( start, "?>" );

	version = "";
	encoding = "";
	standalone = "";

	while ( in->good() )
	{
		if ( in->peek() == '>' )
		{
			in->ignore( 1 );
			return true;
		}

		SkipWhiteSpace( in );
		if ( StreamEqual( in, "version", true ) )
		{
			in->ignore( 7 );

			TiXmlAttribute attrib;
			attrib.Parse( in );		
			version = attrib.Value();
		}
		else if ( StreamEqual( in, "encoding", true ) )
		{
			in->ignore( 8 );

			TiXmlAttribute attrib;
			attrib.Parse( in );		
			encoding = attrib.Value();
		}
		else if ( StreamEqual( in, "standalone", true ) )
		{
			in->ignore( 10 );

			TiXmlAttribute attrib;
			attrib.Parse( in );		
			standalone = attrib.Value();
		}
		else
		{
			// Read over whatever it is.
			while( in->good() && in->peek() != '>' && !isspace( in->peek() ) )
				in->get();
		}
	}
	return false;		// didn't find end tag
}

bool TiXmlText::Blank() const
{
	for ( unsigned i=0; i<value.size(); i++ )
		if ( !isspace( value[i] ) )
			return false;
	return true;
}

