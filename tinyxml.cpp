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

#include <iostream>
#include <sstream>
#include <fstream>
#include "tinyxml.h"
using namespace std;


bool TiXmlBase::condenseWhiteSpace = true;


void TiXmlBase::PutString( const std::string& str, std::ostream* stream )
{
	// fixme: is there a better (speed) way to do this?
	unsigned int i, j;

	for( i=0; i<str.length(); ++i )
	{
		for ( j=0; j<NUM_ENTITY; ++j )
		{
			if ( str[i] == entity[j].chr )
			{
				stream->write( entity[j].str, entity[j].strLength );
				break;
			}
		}
		if ( j == NUM_ENTITY )
		{
			stream->put( str[i] );
		}
	}
}


TiXmlNode::TiXmlNode( NodeType _type )
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
}


TiXmlNode::~TiXmlNode()
{
	TiXmlNode* node = firstChild;
	TiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	
}


void TiXmlNode::Clear()
{
	TiXmlNode* node = firstChild;
	TiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	

	firstChild = 0;
	lastChild = 0;
}


TiXmlNode* TiXmlNode::LinkEndChild( TiXmlNode* node )
{
	node->parent = this;
	
	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}
	

TiXmlNode* TiXmlNode::InsertEndChild( const TiXmlNode& addThis )
{
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


TiXmlNode* TiXmlNode::InsertBeforeChild( TiXmlNode* beforeThis, const TiXmlNode& addThis )
{
	if ( beforeThis->parent != this )
		return 0;
	
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;
	
	node->next = beforeThis;
	node->prev = beforeThis->prev;
	beforeThis->prev->next = node;
	beforeThis->prev = node;
	return node;
}


TiXmlNode* TiXmlNode::InsertAfterChild( TiXmlNode* afterThis, const TiXmlNode& addThis )
{
	if ( afterThis->parent != this )
		return 0;
	
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;
	
	node->prev = afterThis;
	node->next = afterThis->next;
	afterThis->next->prev = node;
	afterThis->next = node;
	return node;
}


TiXmlNode* TiXmlNode::ReplaceChild( TiXmlNode* replaceThis, const TiXmlNode& withThis )
{
	if ( replaceThis->parent != this )
		return 0;
	
	TiXmlNode* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete replaceThis;
	return node;
}


bool TiXmlNode::RemoveChild( TiXmlNode* removeThis )
{
	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}
	
	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete removeThis;
	return true;
}


TiXmlNode* TiXmlNode::FirstChild( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = firstChild; node; node = node->next )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::LastChild( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = lastChild; node; node = node->prev )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::IterateChildren( TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}


TiXmlNode* TiXmlNode::IterateChildren( const std::string& val, TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}


TiXmlNode* TiXmlNode::NextSibling( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = next; node; node = node->next )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::PreviousSibling( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = prev; node; node = node->prev )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


void TiXmlElement::RemoveAttribute( const std::string& name )
{
	TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		attributeSet.Remove( node );
		delete node;
	}
}


TiXmlElement* TiXmlNode::FirstChildElement() const
{
	TiXmlNode* node;

	for (	node = FirstChild();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


TiXmlElement* TiXmlNode::FirstChildElement( const std::string& value ) const
{
	TiXmlNode* node;

	for (	node = FirstChild( value );
			node;
			node = node->NextSibling( value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


TiXmlElement* TiXmlNode::NextSiblingElement() const
{
	TiXmlNode* node;

	for (	node = NextSibling();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


TiXmlElement* TiXmlNode::NextSiblingElement( const std::string& value ) const
{
	TiXmlNode* node;

	for (	node = NextSibling( value );
			node;
			node = node->NextSibling( value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}



TiXmlDocument* TiXmlNode::GetDocument() const
{
	const TiXmlNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}


// TiXmlElement::TiXmlElement() 
// 	: TiXmlNode( TiXmlNode::ELEMENT )
// {
// }

TiXmlElement::TiXmlElement( const std::string& _value ) 
	: TiXmlNode( TiXmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}

TiXmlElement::~TiXmlElement()
{
	while( attributeSet.First() )
	{
		TiXmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		delete node;
	}
}

const std::string* TiXmlElement::Attribute( const std::string& name ) const
{
	TiXmlAttribute* node = attributeSet.Find( name );

	if ( node )
		return &(node->Value() );

	return 0;
}


const std::string* TiXmlElement::Attribute( const std::string& name, int* i ) const
{
	const std::string* s = Attribute( name );
	if ( s )
		*i = atoi( s->c_str() );
	else
		*i = 0;
	return s;
}


void TiXmlElement::SetAttribute( const std::string& name, int val )
{	
	char buf[64];
	sprintf( buf, "%d", val );

	std::string v = buf;

	SetAttribute( name, v );
}


void TiXmlElement::SetAttribute( const std::string& name, const std::string& value )
{
	TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		node->SetValue( value );
		return;
	}

	TiXmlAttribute* attrib = new TiXmlAttribute( name, value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		TiXmlDocument* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
	}
}


void TiXmlElement::Print( std::ostream* stream, int depth ) const
{
	int i;
	for ( i=0; i<depth; i++ )
	{
		(*stream) << "    ";
	}

	(*stream) << "<" << value;

	TiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{	
		(*stream) << " ";
		//(*stream) << (*attrib); Not supported for attrib.
		attrib->Print( stream, -1 );
	}

	// If this node has children, give it a closing tag. Else
	// make it an empty tag.
	TiXmlNode* node;
	if ( firstChild )
	{ 		
		(*stream) << ">";

		for ( node = firstChild; node; node=node->NextSibling() )
		{
	 		if (	depth >= 0				// -1 is a special value for unformatted output
				 && !node->ToText() )
			{
				if ( depth >= 0 )
				{
					(*stream) << "\n";
				}
			}
			if ( depth >= 0 )
				node->Print( stream, depth+1 ); 
			else
				node->Print( stream, depth );
		}
		if ( depth >= 0	)			// -1 is a special value for unformatted output
		{
			(*stream) << "\n";
		}
		for( i=0; i<depth; ++i )
			(*stream) << "    ";
		(*stream) << "</" << value << ">";
	}
	else
	{
		(*stream) << " />";
	}
}


TiXmlNode* TiXmlElement::Clone() const
{
	TiXmlElement* clone = new TiXmlElement( Value() );

	if ( !clone )
		return 0;
	
	CopyToClone( clone );

	// Clone the attributes, then clone the children.
	TiXmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First(); 
			attribute; 
			attribute = attribute->Next() )
	{
		clone->SetAttribute( attribute->Name(), attribute->Value() );
	}
	
	TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


TiXmlDocument::TiXmlDocument() : TiXmlNode( TiXmlNode::DOCUMENT )
{
	error = false;
	ignoreWhiteSpace = true;
}


TiXmlDocument::TiXmlDocument( const std::string& documentName ) : TiXmlNode( TiXmlNode::DOCUMENT )
{
	ignoreWhiteSpace = true;
	value = documentName;
	error = false;
}


bool TiXmlDocument::LoadFile()
{
	return LoadFile( value );
}


bool TiXmlDocument::SaveFile() const
{
 	return SaveFile( value );
}


bool TiXmlDocument::LoadFile( const std::string& filename )
{	
	// Delete the existing data:
	Clear();
	value = filename;
	
	FILE* file = fopen( filename.c_str(), "r" );

	if ( file )
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		long length = 0;
		fseek( file, 0, SEEK_END );
		length = ftell( file );
		fseek( file, 0, SEEK_SET );

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.
		std::string data;
		data.reserve( length );

		const int BUF_SIZE = 2048;
		char buf[BUF_SIZE];

		while( fgets( buf, BUF_SIZE, file ) )
		{
			data += buf;
		}
		fclose( file );

		Parse( data.c_str() );
		if (  !Error() )
		{
			return true;
		}
	}
	SetError( TIXML_ERROR_OPENING_FILE );
	return false;
}


bool TiXmlDocument::SaveFile( const std::string& filename ) const
{
	// The old c stuff lives on...
	FILE* fp = fopen( filename.c_str(), "w" );
	if ( fp )
	{
		Print( fp );
		fclose( fp );
		return true;
	}
	return false;
}


TiXmlNode* TiXmlDocument::Clone() const
{
	TiXmlDocument* clone = new TiXmlDocument();
	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->error = error;
	clone->errorDesc = errorDesc;

	TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


void TiXmlDocument::Print( std::ostream* stream, int depth ) const
{
	TiXmlNode* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( stream, depth );
		if ( depth >= 0 )				// -1 is a special value for unformatted output
		{
			(*stream) << "\n";			// Newlines for formatted output.
		}
	}
//	if ( depth < 0 )
//	{
//		(*stream) << " ";			// Minimal white space for stream output.
//	}
}


void TiXmlDocument::Print( FILE* cfile ) const
{
	std::ostringstream out (std::ostringstream::out);
 	Print( &out, 0 );
	fprintf( cfile, "%s", out.str().c_str() );
}


TiXmlElement* TiXmlDocument::RootElement() const
{
	TiXmlNode* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{	
		if ( node->ToElement() )
		{
			return node->ToElement();
		}
	}
	return 0;
}


TiXmlAttribute* TiXmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}


TiXmlAttribute* TiXmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}


void TiXmlAttribute::Print( std::ostream* stream, int depth ) const
{
	if ( value.find( '\"' ) != std::string::npos )
	{
		PutString( name, stream );
		(*stream) << "=" << "'";
		PutString( value, stream );
		(*stream) << "'";
	}
	else
	{
		PutString( name, stream );
		(*stream) << "=" << "\"";
		PutString( value, stream );
		(*stream) << "\"";
	}
}


void TiXmlAttribute::SetIntValue( int value )
{
	std::string s;
	std::ostringstream stream( s );
	stream << value;
	SetValue( stream.str() );
}


void TiXmlAttribute::SetDoubleValue( double value )
{
	std::string s;
	std::ostringstream stream( s );
	stream << value;
	SetValue( stream.str() );
}


const int TiXmlAttribute::IntValue() const
{
	int v;
	std::istringstream string( value );
	string >> v;
	return v;
}


const double  TiXmlAttribute::DoubleValue() const
{
	double v;
	std::istringstream string( value );
	string >> v;
	return v;
}


void TiXmlComment::Print( std::ostream* stream, int depth ) const
{
	for ( int i=0; i<depth; i++ )
	{
		(*stream) << "    ";
	}
	(*stream) << "<!--";
	PutString( value, stream );
	(*stream) << "-->";
}


TiXmlNode* TiXmlComment::Clone() const
{
	TiXmlComment* clone = new TiXmlComment();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


void TiXmlText::Print( std::ostream* stream, int depth ) const
{
//	(*stream) << value;
	PutString( value, stream );
}


TiXmlNode* TiXmlText::Clone() const
{	
	TiXmlText* clone = 0;
	clone = new TiXmlText();
	
	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


TiXmlDeclaration::TiXmlDeclaration( const std::string& _version, 
									const std::string& _encoding,
									const std::string& _standalone )
	: TiXmlNode( TiXmlNode::DECLARATION ) 
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


void TiXmlDeclaration::Print( std::ostream* stream, int depth ) const
{
	(*stream) << "<?xml ";

	if ( !version.empty() )
	{
		(*stream) << "version=\"";
		PutString( version, stream );
//		out += version;
		(*stream) << "\" ";
	}
	if ( !encoding.empty() )
	{
		(*stream) << "encoding=\"";
//		out += encoding;
		PutString( encoding, stream );
		(*stream ) << "\" ";
	}
	if ( !standalone.empty() )
	{
		(*stream) << "standalone=\"";
//		out += standalone;	
		PutString( standalone, stream );
		(*stream) << "\" ";
	}
//	out += "?>";
	(*stream) << "?>";

//	(*stream) << out;
}


TiXmlNode* TiXmlDeclaration::Clone() const
{	
	TiXmlDeclaration* clone = new TiXmlDeclaration();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->version = version;
	clone->encoding = encoding;
	clone->standalone = standalone;
	return clone;
}


void TiXmlUnknown::Print( std::ostream* stream, int depth ) const
{
	for ( int i=0; i<depth; i++ )
		(*stream) << "    ";
	(*stream) << "<" << value << ">";		// Don't use entities hear! It is unknown.
}


TiXmlNode* TiXmlUnknown::Clone() const
{
	TiXmlUnknown* clone = new TiXmlUnknown();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


TiXmlAttributeSet::TiXmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


TiXmlAttributeSet::~TiXmlAttributeSet()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void TiXmlAttributeSet::Add( TiXmlAttribute* addMe )
{
	assert( !Find( addMe->Name() ) );	// Shouldn't be multiply adding to the set.
	
	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void TiXmlAttributeSet::Remove( TiXmlAttribute* removeMe )
{
	TiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}


TiXmlAttribute*	TiXmlAttributeSet::Find( const std::string& name ) const
{
	TiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->Name() == name )
			return node;
	}
	return 0;
}

