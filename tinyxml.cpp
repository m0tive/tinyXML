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


TiXmlNode::TiXmlNode( int _type, TiXmlDocument* doc )
{
	document = doc;
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
}


TiXmlNode::~TiXmlNode()
{
	TiXmlBase* node = firstChild;
	TiXmlBase* temp;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	
}


bool TiXmlNode::InsertEndChild( TiXmlNode* node )
{
	node->parent = this;
	node->document = document;
	
	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;

	lastChild = node;
	return true;
}
	

bool TiXmlNode::InsertEndChild( const TiXmlNode& addThis )
{
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return false;

	return InsertEndChild( node );
}


bool TiXmlNode::InsertBeforeChild( TiXmlNode* beforeThis, const TiXmlNode& addThis )
{
	if ( beforeThis->parent != this )
		return false;
	
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return false;
	node->parent = this;
	node->document = document;
	
	node->next = beforeThis;
	node->prev = beforeThis->prev;
	beforeThis->prev->next = node;
	beforeThis->prev = node;
	return true;
}


bool TiXmlNode::InsertAfterChild( TiXmlNode* afterThis, const TiXmlNode& addThis )
{
	if ( afterThis->parent != this )
		return false;
	
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return false;
	node->parent = this;
	node->document = document;
	
	node->prev = afterThis;
	node->next = afterThis->next;
	afterThis->next->prev = node;
	afterThis->next = node;
	return true;
}


bool TiXmlNode::ReplaceChild( TiXmlNode* replaceThis, const TiXmlNode& withThis )
{
	if ( replaceThis->parent != this )
		return false;
	
	TiXmlNode* node = withThis.Clone();
	if ( !node )
		return false;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;
	replaceThis->next->prev = node;
	replaceThis->prev->next = node;
	delete replaceThis;
	return true;
}


bool TiXmlNode::RemoveChild( TiXmlNode* removeThis )
{
	if ( removeThis->parent != this )
		return false;
	
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
	for ( node = (TiXmlNode*) firstChild; node; node = (TiXmlNode*) node->next )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::LastChild( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = (TiXmlNode*) lastChild; node; node = (TiXmlNode*) node->prev )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::NextSibling( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = (TiXmlNode*) next; node; node = (TiXmlNode*) node->next )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlNode* TiXmlNode::PreviousSibling( const std::string& value ) const
{
	TiXmlNode* node;
	for ( node = (TiXmlNode*) prev; node; node = (TiXmlNode*) node->prev )
	{
		if ( node->Value() == value )
			return node;
	}
	return 0;
}


TiXmlElement::TiXmlElement( TiXmlDocument* doc ) : TiXmlNode( TiXmlNode::ELEMENT, doc )
{
	firstAttrib = lastAttrib = 0;
	firstChild = lastChild = 0;
}

TiXmlElement::~TiXmlElement()
{
	TiXmlBase* node = firstAttrib;
	TiXmlBase* temp;

	while( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}
}

const std::string* TiXmlElement::Attribute( const std::string& name ) const
{
	// OPT
	// We should probably do better than a linear search.

	TiXmlAttribute* node;

	for ( node = firstAttrib; node; node = (TiXmlAttribute*) node->next )
	{	
		if ( node->Name() == name )
			return &(node->Value() );
	}

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
	// OPT
	// We should probably do better than a linear search.

	TiXmlAttribute* node;

	for ( node = firstAttrib; node; node = (TiXmlAttribute*) node->next )
	{	
		if ( node->Name() == name )
		{
			node->SetValue( value );
			return;
		}
	}
	TiXmlAttribute* attrib = new TiXmlAttribute( name, value );
	if ( attrib )
	{
		attrib->prev = lastAttrib;
		attrib->next = 0;
		if ( lastAttrib )
		{
			lastAttrib->next = attrib;
		}
		else
		{
			firstAttrib = attrib;
		}
		lastAttrib = attrib;
	}
	else
	{
		if ( document ) document->SetError( "Memory error." );
	}
}


void TiXmlElement::RemoveAttribute( const std::string& name )
{
	// OPT
	// We should probably do better than a linear search.

	TiXmlAttribute* node;

	for ( node = firstAttrib; node; node = (TiXmlAttribute*) node->next )
	{	
		if ( node->Name() == name )
		{
			if ( node->prev )
				node->prev->next = node->next;
			else
				firstAttrib = (TiXmlAttribute*) node->next;

			if ( node->next )
				node->next->prev = node->prev;
			else
				lastAttrib = (TiXmlAttribute*) node->prev;
			delete node;
		}
	}
}

void TiXmlElement::Print( FILE* fp, int depth )
{
	int i;
	for ( i=0; i<depth; i++ )
		fprintf( fp, "    " );

	fprintf( fp, "<%s", value.c_str() );
	TiXmlBase* node;

	for ( node = firstAttrib; node; node = node->next )
	{	
		fprintf( fp, " " );
		node->Print( fp, 0 );
	}
	// If this node has children, give it a closing tag. Else
	// make it an empty tag.
	if ( firstChild )
	{
 		
		fprintf( fp, ">" );

		for ( node = firstChild; node; node=node->next )
		{
	 		if ( !((TiXmlNode*)node)->ToText() )
				fprintf( fp, "\n" );
			node->Print( fp, depth+1 );
		}
 		fprintf( fp, "\n" );
		for ( i=0; i<depth; i++ )
			fprintf( fp, "    " );
		fprintf( fp, "</%s>", value.c_str() );
	}
	else
	{
		fprintf( fp, " />" );
	}
}

TiXmlNode* TiXmlElement::Clone() const
{
	TiXmlElement* clone;
	if ( document )
	{
		clone = document->Factory()->CreateElement( Parent(), document );
	}
	else	
	{
		clone = new TiXmlElement();
	}

	if ( !clone )
		return 0;
	
	CopyToClone( clone );

	// Clone the attributes, then clone the children.
	TiXmlAttribute* attribute = 0;
	for( attribute = firstAttrib; attribute; attribute = (TiXmlAttribute*) attribute->next )
	{
		clone->SetAttribute( attribute->Name(), attribute->Value() );
	}
	
	TiXmlNode* node = 0;
	for ( node = (TiXmlNode*) firstChild; node; node = (TiXmlNode*) node->next )
	{
		clone->InsertEndChild( node->Clone() );
	}
	return clone;
}


TiXmlDocument::TiXmlDocument() : TiXmlNode( TiXmlNode::DOCUMENT, 0 )
{
	error = false;
	factory = new TiXmlFactory();
}


TiXmlDocument::TiXmlDocument( const std::string& documentName ) : TiXmlNode( TiXmlNode::DOCUMENT, 0 )
{
	factory = new TiXmlFactory();
	value = documentName;
	error = false;
}

void TiXmlDocument::SetFactory( TiXmlFactory* f )
{
	delete factory;
	factory = f;
}


bool TiXmlDocument::LoadFile()
{
	return LoadFile( value );
}


bool TiXmlDocument::SaveFile()
{
 	return SaveFile( value );
}


bool TiXmlDocument::LoadFile( const std::string& filename )
{
	FILE* fp = fopen( filename.c_str(), "r" );
	if ( fp )
	{
		unsigned size;
		fseek( fp, 0, SEEK_END );
		size = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		char* buf = new char[size+1];
		char* p = buf;
		while( fgets( p, size, fp ) )
		{
			p = strchr( p, 0 );
		}
		fclose( fp );
		
		Parse( buf );
		delete buf;

		if ( !Error() )
			return true;
	}
	return false;
}


bool TiXmlDocument::SaveFile( const std::string& filename )
{
	FILE* fp = fopen( filename.c_str(), "w" );
	if ( fp )
	{
		Print( fp, 0 );
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
	for ( node = (TiXmlNode*) firstChild; node; node = (TiXmlNode*) node->next )
	{
		clone->InsertEndChild( node->Clone() );
	}
	return clone;
}


void TiXmlDocument::Print( FILE* fp, int )
{
	TiXmlBase* node;
	for ( node=FirstChild(); node; node=node->next )
	{
		node->Print( fp, 0 );
		fprintf( fp, "\n" );
	}
}


void TiXmlAttribute::Print( FILE* fp, int )
{
	if ( value.find( '\"' ) != std::string::npos )
		fprintf( fp, "%s='%s'", name.c_str(), value.c_str() );
	else
		fprintf( fp, "%s=\"%s\"", name.c_str(), value.c_str() );
}


void TiXmlComment::Print( FILE* fp, int depth )
{
	for ( int i=0; i<depth; i++ )
		fprintf( fp, "    " );
	fprintf( fp, "<?--%s-->", value.c_str() );
}

TiXmlNode* TiXmlComment::Clone() const
{
	TiXmlComment* clone = 0;
	if ( document )
	{
		clone = document->Factory()->CreateComment( Parent(), document );
	}
	else	
	{
		clone = new TiXmlComment();
	}

// 	TiXmlComment* clone = new TiXmlComment();
	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


void TiXmlText::Print( FILE* fp, int )
{
	fprintf( fp, "%s", value.c_str() );
}


TiXmlNode* TiXmlText::Clone() const
{	
	TiXmlText* clone = 0;
	if ( document )
	{
		clone = document->Factory()->CreateText( Parent(), document );
	}
	else	
	{
		clone = new TiXmlText();
	}
// 	TiXmlText* clone = new TiXmlText();
	
	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


void TiXmlUnknown::Print( FILE* fp, int depth )
{
	for ( int i=0; i<depth; i++ )
		fprintf( fp, "    " );
	fprintf( fp, "<%s>", value.c_str() );
}


TiXmlNode* TiXmlUnknown::Clone() const
{
	TiXmlUnknown* clone = 0;	//= new TiXmlUnknown();
	if ( document )
	{
		clone = document->Factory()->CreateUnknown( Parent(), document );
	}
	else	
	{
		clone = new TiXmlUnknown();
	}
	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}
