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


TiXmlNode::TiXmlNode( NodeType _type, TiXmlDocument* doc )
{
	document = doc;
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


TiXmlNode* TiXmlNode::InsertEndChild( TiXmlNode* node )
{
	node->parent = this;
	node->document = document;
	
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

	return InsertEndChild( node );
}


TiXmlNode* TiXmlNode::InsertBeforeChild( TiXmlNode* beforeThis, const TiXmlNode& addThis )
{
	if ( beforeThis->parent != this )
		return 0;
	
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;
	node->document = document;
	
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
	node->document = document;
	
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
	replaceThis->next->prev = node;
	replaceThis->prev->next = node;
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


TiXmlNode* TiXmlNode::IterateChildren( TiXmlNode* previous )
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


TiXmlNode* TiXmlNode::IterateChildren( const std::string& val, TiXmlNode* previous )
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


TiXmlElement::TiXmlElement( TiXmlDocument* doc ) 
	: TiXmlNode( TiXmlNode::ELEMENT, doc )
{
}

TiXmlElement::TiXmlElement( const std::string& _value, TiXmlDocument* doc ) 
	: TiXmlNode( TiXmlNode::ELEMENT, doc )
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
		if ( document ) document->SetError( ERROR_OUT_OF_MEMORY );
	}
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


void TiXmlElement::Print( FILE* fp, int depth )
{
	int i;
	for ( i=0; i<depth; i++ )
		fprintf( fp, "    " );

	fprintf( fp, "<%s", value.c_str() );

	TiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{	
		fprintf( fp, " " );
		attrib->Print( fp, 0 );
	}
	// If this node has children, give it a closing tag. Else
	// make it an empty tag.
	TiXmlNode* node;
	if ( firstChild )
	{ 		
		fprintf( fp, ">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
	 		if ( !node->ToText() )
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
	for(	attribute = attributeSet.First(); 
			attribute; 
			attribute = attribute->Next() )
	{
		clone->SetAttribute( attribute->Name(), attribute->Value() );
	}
	
	TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
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
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->InsertEndChild( node->Clone() );
	}
	return clone;
}


void TiXmlDocument::Print( FILE* fp, int )
{
	TiXmlNode* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( fp, 0 );
		fprintf( fp, "\n" );
	}
}


// TiXmlAttribute* TiXmlAttribute::Successor()
// {
// 	// The leftmost node of the right tree.
// 	TiXmlAttribute* s = 0;
// 
// 	if ( right )
// 	{
// 		s = right;
// 		while( s->left )
// 			s = s->left;
// 	}
// 	return s;
// }
// 
// 
// TiXmlAttribute* TiXmlAttribute::Predecessor();
// {
// 	// The rightmost node of the left tree.
// 	TiXmlAttribute* p = 0;
// 
// 	if ( left )
// 	{
// 		p = left;
// 		while( p->right )
// 			p = p->right;
// 	}
// 	return p;
// }


TiXmlAttribute* TiXmlAttribute::Next()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}


TiXmlAttribute* TiXmlAttribute::Previous()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return next;
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
	fprintf( fp, "<!--%s-->", value.c_str() );
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


TiXmlDeclaration::TiXmlDeclaration( const std::string& _version, 
									const std::string& _encoding,
									const std::string& _standalone,
									TiXmlDocument* _doc )
	: TiXmlNode( TiXmlNode::DECLARATION, _doc ) 
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


void TiXmlDeclaration::Print( FILE* fp, int )
{
	std::string out = "<?xml ";

	if ( !version.empty() )
	{
		out += "version=\"";
		out += version;
		out += "\" ";
	}
	if ( !encoding.empty() )
	{
		out += "encoding=\"";
		out += encoding;
		out += "\" ";
	}
	if ( !standalone.empty() )
	{
		out += "standalone=\"";
		out += standalone;
		out += "\" ";
	}
	out += "?>";

	fprintf( fp, "%s", out.c_str() );
}


TiXmlNode* TiXmlDeclaration::Clone() const
{	
	TiXmlDeclaration* clone = 0;
	if ( document )
	{
		clone = document->Factory()->CreateDeclaration( Parent(), document );
	}
	else	
	{
		clone = new TiXmlDeclaration();
	}
	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->version = version;
	clone->encoding = encoding;
	clone->standalone = standalone;
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
	
	addMe->prev = &sentinel;
	addMe->next = sentinel.next;

	sentinel.next->prev = addMe;
	sentinel.next       = addMe;
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

