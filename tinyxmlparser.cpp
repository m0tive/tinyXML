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
	if ( p && ( isalpha( *p ) || *p == '_' ) )
	{
		p++;
		while( p && *p && 
			   (   isalnum( *p ) 
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


const char* TiXmlDocument::Parse( const char* start )
{
	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	
	const char* p = start;

 	p = SkipWhiteSpace( p );
	if ( !p || !*p )
	{
		error = true;
		errorDesc = "Document empty.";
	}
	
	while ( p && *p )
	{	
		if ( *p != '<' )
		{
			error = true;
			errorDesc = "The '<' symbol that starts a tag was not found.";
			break;
		}
		else
		{
			TiXmlNode* node = IdentifyAndParse( &p, this );
			if ( node )
			{
				InsertEndChild( node );
			}				
		}
		p = SkipWhiteSpace( p );
	}
	return 0;	// Return null is fine for a document: once it is read, the parsing is over.
}


TiXmlNode* TiXmlNode::IdentifyAndParse( const char** where, TiXmlDocument* doc )
{
	const char* p = *where;
	TiXmlNode* returnNode = 0;

	assert( *p == '<' );

	p = SkipWhiteSpace( p+1 );

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Everthing else is unknown to tinyxml.
	//
	if ( 	   tolower( *(p+0) ) == 'x' 
			&& tolower( *(p+1) ) == 'm'
			&& tolower( *(p+2) ) == 'l' )		// stricmp is not portable.
	{
		if ( doc )
			returnNode = doc->Factory()->CreateUnknown( this, doc );
		else			
			returnNode = new TiXmlUnknown( doc );
	}
	else if ( isalpha( *p ) || *p == '_' )
	{
		if ( doc )
			returnNode = doc->Factory()->CreateElement( this, doc );
		else			
			returnNode = new TiXmlElement( doc );
// 		returnNode = new TiXmlElement( doc );
	}
	else if (    *(p+0) == '!'
			  && *(p+1) == '-'
			  && *(p+2) == '-' )
	{
		if ( doc )
			returnNode = doc->Factory()->CreateComment( this, doc );
		else			
			returnNode = new TiXmlComment( doc );
// 		returnNode = new TiXmlComment( doc );
	}
	else
	{
		if ( doc )
			returnNode = doc->Factory()->CreateUnknown( this, doc );
		else			
			returnNode = new TiXmlUnknown( doc );
// 		returnNode = new TiXmlUnknown( doc );
	}

	if ( returnNode )
	{
		p = returnNode->Parse( p );
	}
	else
	{
		if ( doc )
			doc->SetError( "Memory Error." );
		p = 0;
	}
	*where = p;
	return returnNode;
}


const char* TiXmlElement::Parse( const char* p )
{
	p = SkipWhiteSpace( p );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( "Error parsing element." );
		return 0;
	}

	// Read the name.
	p = ReadName( p, &value );
	if ( !p )
	{
		if ( document )	document->SetError( "Failed to read element name." );
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
			if ( document ) document->SetError( "Error trying to read attributes." );
			return 0;
		}
		if ( *p == '/' )
		{
			// Empty tag.
			if ( *(p+1) != '>' )
			{
				if ( document ) document->SetError( "Error parsing empty tag." );		
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
				if ( document ) document->SetError( "Error reading end tag." );
				return 0;
			}
		}
		else
		{
			// Try to read an element:
			TiXmlAttribute attrib( document );
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
	// Read in text and elements in any order.
	while ( p && *p )
	{
		const char* start = p;
		while ( *p && *p != '<' )
			p++;

		if ( !*p )
		{
			if ( document ) document->SetError( "Error reading element value." );
			return 0;
		}
		if ( p != start )
		{
			// Take what we have, make a text element.
			TiXmlText* text = 0;
			if ( document )
				text = document->Factory()->CreateText( this, document );
			else
				text = new TiXmlText( document );
// 			text->document = document;

			if ( !text )
			{
				if ( document ) document->SetError( "Memory error." );
				return 0;
			}
			text->Parse( start );
			if ( !text->Blank() )
				InsertEndChild( text );
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
				TiXmlElement* element = 0;
				if ( document )
					element = document->Factory()->CreateElement( this, document );
				else
					element = new TiXmlElement( document );

				if ( element )
				{
					p = element->Parse( p+1 );
					if ( p )
						InsertEndChild( element );
				}
				else
				{
					if ( document ) document->SetError( "Memory error." );
					return 0;
				}
			}
		}
	}
	return 0;
}


const char* TiXmlUnknown::Parse( const char* p )
{
	const char* end = strchr( p, '>' );
	if ( !end )
	{
		if ( document )
			document->SetError( "End tag '>' not found in TiXmlUnknown." );
		return 0;
	}
	else
	{
		value = std::string( p, end-p );
// 		value.resize( end - p );
		return end + 1;			// return just past the '>'
	}
}


const char* TiXmlComment::Parse( const char* p )
{
	assert( *p == '!' && *(p+1) == '-' && *(p+2) == '-' );

	// Find the end, copy the parts between to the value of
	// this object, and return.
	const char* start = p+3;
	const char* end = strstr( p, "-->" );
	if ( !end )
	{
		if ( document )
			document->SetError( "End tag '-->' not found." );
		return 0;
	}
	else
	{
		value = std::string( start, end-start );
// 		value.resize( end - start );
		return end + 3;			// return just past the '>'
	}
}


const char* TiXmlAttribute::Parse( const char* p )
{
	// Read the name, the '=' and the value.
	p = ReadName( p, &name );
	if ( !p )
	{
		if ( document ) document->SetError( "Error reading attribute name." );
		return 0;
	}
	p = SkipWhiteSpace( p );
	if ( !p || *p != '=' )
	{
		if ( document ) document->SetError( "Error finding attribute '='. Elements must consist of name and value." );
		return 0;
	}

	p = SkipWhiteSpace( p+1 );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( "Error skipping white space after attribute '='" );
		return 0;
	}
	
	const char* end = 0;
	const char* start = p+1;
	const char* past = 0;

	if ( *p == '\'' )
	{
		end = strchr( start, '\'' );
		past = end+1;
	}
	else if ( *p == '"' )
	{
		end = strchr( start, '"' );
		past = end+1;
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		start--;
		for ( end = start; *end; end++ )
		{
			if ( isspace( *end ) || *end == '/' || *end == '>' )
				break;
		}
		past = end;
	}
	value = std::string( start, end-start );
	return past;
}


const char* TiXmlText::Parse( const char* p )
{
	value = "";
	while ( *p && *p != '<' )
	{
		if ( *p == '\r' || *p == '\n' )
		{
			// Make sure we don't go out of range:
			if ( value.size() > 0 )
			{
				// only add one white space
				if ( !isspace( value[ value.size() - 1 ] ) )
					value += ' ';
			}
		}
		else
		{
			value += *p;
		}
		p++;
	}
	return p;
}

bool TiXmlText::Blank()
{
	for ( unsigned i=0; i<value.size(); i++ )
		if ( !isspace( value[i] ) )
			return false;
	return true;
}

