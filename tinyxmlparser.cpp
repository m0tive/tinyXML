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

const char* TiXmlBase::SkipWhiteSpace( const char* p )
{
	while ( p && *p && 
	        ( isspace( *p ) || *p == '\n' || *p == '\r' ) )
		p++;
	return p;
}

const char* TiXmlBase::ReadName( const char* p, std::string* name )
{
	*name = "";
	const char* start = p;

	// Names start with letters or underscores.
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if ( p && ( isalpha( (unsigned char) *p ) || *p == '_' ) )
	{
		p++;
		while( p && *p && 
			   (   isalnum( (unsigned char) *p ) 
			     || *p == '_'
				 || *p == '-'
				 || *p == ':' ) )
		{
			p++;
		}
		name->append( start, p - start );
		return p;
	}
	return 0;
}


TiXmlBase::Entity TiXmlBase::entity[ NUM_ENTITY ] = 
{
	{ "&amp",  4, '&' },
	{ "&lt",   3, '<' },
	{ "&gt",   3, '>' },
	{ "&quot", 5, '\"' },
	{ "&apos", 5, '\'' }
};


const char* TiXmlBase::GetEntity( const char* in, char* value )
{
	assert( in );
	for( int i=0; i<NUM_ENTITY; ++i )
	{
		if ( strncmp( in, entity[i].str, entity[i].strLength ) == 0 )
		{
			*value = entity[i].chr;
			return ( in + entity[i].strLength );
		}
	}
	// Technically, this should be an error, but be forgiving.
	*value = *in;
	return ( in + 1 );
}


const char* TiXmlBase::ReadText(	const char* p, std::string* text, 
									bool ignoreWhiteSpace, 
									int numEndTag, const char* endTag[], bool endOnWhite )
{
	const int MAXENDTAG = 16;
	assert( numEndTag < MAXENDTAG );
	*text = "";
	int i;

	int endTagLen[MAXENDTAG];
	for ( i = 0; i<numEndTag; ++i )
	{
		endTagLen[i] = strlen( endTag[i] );
	}

	// If there can be multiple ends, find the nearest.
	char* end = 0;
	char* q;
	for( i=0; i<numEndTag; ++i )
	{
		if ( q = strstr( p, endTag[i] ) )
		{
			if ( end )
			{
				if ( q < end ) 
					end = q;
			}
			else
			{
				end = q;
			}
		}	
	}

	if ( !end )
	{
		return 0;
	}

	if ( !ignoreWhiteSpace )
	{
		// Keep all the white space, if we have a document, and it is set to
		// keep the white space.

		while ( *p && p < end )
		{
			char c;
			p = GetChar( p, &c );
			text->append( &c, 1 );
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		p = SkipWhiteSpace( p );
		while (	*p && p < end ) 
		{
			if ( *p == '\r' || *p == '\n' )
			{
				whitespace = true;
				++p;
			}
			else if ( isspace( *p ) )
			{
				if ( endOnWhite )
				{
					break;
				}
				else
				{
					whitespace = true;
					++p;
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
				char c;
				p = GetChar( p, &c );
				text->append( &c, 1 );
			}
		}
//		// Keep white space before the '<' 
//		if ( whitespace )
//		{
//			text->append( " ", 1 );
//		}
	}
	return p;
}


const char* TiXmlDocument::Parse( const char* start )
{
	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	//

// Not yet implemented:
//	// Parsing ends when we hit a second declaration,
//	// or don't hit an opening tag.
	
	const char* p = start;
//	bool declarationFound = false;

 	p = SkipWhiteSpace( p );
	if ( !p || !*p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY );
	}
	
	while ( p && *p )
	{	
		if ( *p != '<' )
		{
			SetError( TIXML_ERROR_PARSING_ELEMENT );
			break;
		}
		else
		{
			TiXmlNode* node = Identify( p );
			if ( node )
			{
				p = node->Parse( p );
				LinkEndChild( node );
			}		
			else
			{
				p = 0;
				break;
			}		
		}
		p = SkipWhiteSpace( p );
	}
	return 0;	// Return null is fine for a document: once it is read, the parsing is over.
}


TiXmlNode* TiXmlNode::Identify( const char* p )
{
	p = SkipWhiteSpace( p );
	TiXmlNode* returnNode = 0;

	if( !p || *p != '<' )
	{
		return 0;
	}

	TiXmlDocument* doc = GetDocument();
	const char* q = SkipWhiteSpace( p+1 );

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//
	if ( 	   tolower( *(q+0) ) == '?'
			&& tolower( *(q+1) ) == 'x' 
			&& tolower( *(q+2) ) == 'm'
			&& tolower( *(q+3) ) == 'l' )
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Declaration\n" );
		#endif
		returnNode = new TiXmlDeclaration();
	}
	else if ( isalpha( (unsigned char) *q ) || *q == '_' )
	{
		#ifdef DEBUG_PARSER
			printf( "XML parsing Element\n" );
		#endif
		returnNode = new TiXmlElement( "" );
	}
	else if (    *(q+0) == '!'
			  && *(q+1) == '-'
			  && *(q+2) == '-' )
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
		p = 0;
	}
	return returnNode;
}


const char* TiXmlElement::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	TiXmlDocument* document = GetDocument();
	//p = SkipWhiteSpace( p );
	if ( !p || !*p || *p != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT );
		return 0;
	}

	p = SkipWhiteSpace( p+1 );

	// Read the name.
	p = ReadName( p, &value );
	if ( !p )
	{
		if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME );
		return 0;
	}

	std::string endTag = "</";
	endTag += value;
	endTag += ">";

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( p && *p )
	{
		p = SkipWhiteSpace( p );
		if ( !p || !*p )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
			return 0;
		}
		if ( *p == '/' )
		{
			// Empty tag.
			if ( *(p+1) != '>' )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY );		
				return 0;
			}
			return p+2;
		}
		else if ( *p == '>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			p = ReadValue( p+1 );		// Note this is an Element method, and will set the error if one happens.
			if ( !p )
				return 0;

			// We should find the end tag now
			std::string buf( p, endTag.size() );
			if ( endTag == buf )
			{
				return p+endTag.size();
			}
			else
			{
				if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG );
				return 0;
			}
		}
		else
		{
			// Try to read an element:
			TiXmlAttribute attrib;
			attrib.SetDocument( document );
			p = attrib.Parse( p );

			if ( p )
			{
				SetAttribute( attrib.Name(), attrib.Value() );
			}
		}
	}
	return 0;
}


const char* TiXmlElement::ReadValue( const char* p )
{
	TiXmlDocument* document = GetDocument();

	// Read in text and elements in any order.
	p = SkipWhiteSpace( p );
	while ( p && *p )
	{
		const char* start = p;
		while ( *p && *p != '<' )
			p++;

		if ( !*p )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE );
			return 0;
		}
		if ( p != start )
		{
			// Take what we have, make a text element.
			TiXmlText* text = new TiXmlText();

			if ( !text )
			{
				if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
				return 0;
			}
			text->Parse( start );
			if ( !text->Blank() )
				LinkEndChild( text );
			else
				delete text;
		} 
		else 
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if ( *(p+1) == '/' )
			{
				return p;	// end tag
			}
			else
			{
				TiXmlNode* node = Identify( p );
				if ( node )
				{
					p = node->Parse( p );
					LinkEndChild( node );
				}				
				else
				{
					return 0;
				}
			}
		}
	}
	return 0;
}


const char* TiXmlUnknown::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	const char* end = strchr( p, '>' );
	if ( !end )
	{
		TiXmlDocument* document = GetDocument();
		if ( document )
			document->SetError( TIXML_ERROR_PARSING_UNKNOWN );
		return 0;
	}
	else
	{
		value = std::string( p, end-p );
		return end + 1;			// return just past the '>'
	}
}


const char* TiXmlComment::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	assert(    *(p+0) == '<' 
	        && *(p+1) == '!' 
			&& *(p+2) == '-' 
			&& *(p+3) == '-' );

	// Find the end, copy the parts between to the value of
	// this object, and return.
	const char* start = p+4;
	const char* end = strstr( p, "-->" );
	if ( !end )
	{
		TiXmlDocument* document = GetDocument();
		if ( document )
			document->SetError( TIXML_ERROR_PARSING_COMMENT );
		return 0;
	}
	else
	{
		TiXmlDocument* doc = GetDocument();
		bool ignoreWhite = true;
		if ( doc && !doc->IgnoreWhiteSpace() )
			ignoreWhite = false;

		const char* end[1] = { "-->" };
		p = ReadText( start, &value, ignoreWhite, 1, end, false ); 

		if ( !p )
		{
			return 0;
		}
		return p + 3;			// return just past the '>'
	}
}


const char* TiXmlAttribute::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	// Read the name, the '=' and the value.
	p = ReadName( p, &name );
	if ( !p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}
	p = SkipWhiteSpace( p );
	if ( !p || *p != '=' )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}

	p = SkipWhiteSpace( p+1 );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}
	
//	const char* end = 0;
//	const char* start = p+1;
//	const char* past = 0;
//
	const char* end[8];

	if ( *p == '\'' )
	{
//		end = strchr( start, '\'' );
//		past = end+1;
		end[0] = "\'";
		p = ReadText( p+1, &value, false, 1, end, false );
		if ( p ) p = p + 1;	// get past the end '
	}
	else if ( *p == '"' )
	{
//		end = strchr( start, '"' );
//		past = end+1;
		end[0] = "\"";
		p = ReadText( p+1, &value, false, 1, end, false );
		if ( p ) p = p + 1;	// get past the end "
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
//		start--;
//		for ( end = start; *end; end++ )
//		{
//			if ( isspace( *end ) || *end == '/' || *end == '>' )
//				break;
//		}
//		past = end;
		end[0] = "/";
		end[1] = ">";
		p = ReadText( p, &value, false, 2, end, true );
	}
//	value = std::string( start, end-start );
	return p;
}


const char* TiXmlText::Parse( const char* p )
{
	value = "";
//	value.reserve( 256 );	// Avoid a bunch of small allocations in the beginning. Give ourselves some memory.

	TiXmlDocument* doc = GetDocument();
	bool ignoreWhite = true;
	if ( doc && !doc->IgnoreWhiteSpace() )
		ignoreWhite = false;

	const char* end[1] = { "<" };
	p = ReadText( p, &value, ignoreWhite, 1, end, false );

	return p;
}


const char* TiXmlDeclaration::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	const char* start = p+5;
	const char* end  = strstr( start, "?>" );

	version = "";
	encoding = "";
	standalone = "";

	// Be nice to the user:
	if ( !end )
	{
		end = strstr( start, ">" );
		end++;
	}
	else
	{
		end += 2;
	}

	if ( !end )
	{
		TiXmlDocument* document = GetDocument();
		if ( document )
			document->SetError( TIXML_ERROR_PARSING_DECLARATION );
		return 0;
	}
	else
	{
		const char* p;
		
		p = strstr( start, "version" );
		if ( p && p < end )
		{
			TiXmlAttribute attrib;
			attrib.Parse( p );		
			version = attrib.Value();
		}

		p = strstr( start, "encoding" );
		if ( p && p < end )
		{
			TiXmlAttribute attrib;
			attrib.Parse( p );		
			encoding = attrib.Value();
		}

		p = strstr( start, "standalone" );
		if ( p && p < end )
		{
			TiXmlAttribute attrib;
			attrib.Parse( p );		
			standalone = attrib.Value();
		}
	}
	return end;
}

bool TiXmlText::Blank() const
{
	for ( unsigned i=0; i<value.size(); i++ )
		if ( !isspace( value[i] ) )
			return false;
	return true;
}

