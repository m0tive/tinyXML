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


#ifndef TINYXML_INCLUDED
#define TINYXML_INCLUDED

#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )

#include <string>
#include <stdio.h>
#include <assert.h>

class TiXmlDocument;
class TiXmlElement;
class TiXmlComment;
class TiXmlUnknown;
class TiXmlAttribute;
class TiXmlText;


/** TiXmlBase is a base class for every class in TinyXml.
	It does little except to establist that TinyXml classes
	can be printed and are all part of doubly linked lists.

	Classes in the TinyXML DOM are either containment nodes 
	or leaf nodes. No node (in a normal document) is both.

	@verbatim
	A Document contains:	Element	(container)
							Comment (leaf)
							Unknown (leaf)
	An Element contains:	Element (container)
							Text	(leaf)
							Attributes (not on tree)
							Comment (leaf)
							Unknown (leaf)
	@endverbatim
*/
class TiXmlBase
{
	friend class TiXmlNode;
	friend class TiXmlElement;
	friend class TiXmlDocument;
 
  public:
	TiXmlBase() : prev( 0 ), next( 0 )		{}	
	virtual ~TiXmlBase()					{}
	
	/*	All TinyXml classes can print themselves to a filestream.
	*/
	virtual void Print( FILE* fp, int depth )	= 0;
  
  protected:
	/*	General parsing helper method. Takes a pointer in,
		skips all the white space it finds, and returns a pointer
		to the first non-whitespace data.
	*/
	static const char* SkipWhiteSpace( const char* p );

	/*	Reads an XML name into the string provided. Returns
		a pointer just past the last character of the name, 
		or 0 if the function has an error.
	*/
	static const char* ReadName( const char* p, std::string* name );

	TiXmlBase* prev;
	TiXmlBase* next;
};


/** The parent class for everything in the Document Object Model.
	(Except for attributes, which are contained in elements.)
	Nodes have siblings, a parent, and children. A node can be
	in a document, or stand on its own. The type of a TyXmlNode
	can be queried, and it can be cast to its more defined type.
*/
class TiXmlNode : public TiXmlBase
{
  public:
	/** The types of XML nodes supported by TinyXml. (All the
		unsupported types are picked up by UNKNOWN.)
	*/
	enum {
		DOCUMENT, ELEMENT, COMMENT, UNKNOWN, TEXT, TYPECOUNT
	};

	virtual ~TiXmlNode();

	/** The meaning of 'value' changes for the specific type of
		TiXmlNode.
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
		@endverbatim

		The subclasses will wrap this function.
	*/
	const std::string& Value()	const			{ return value; }

	/** Changes the value of the node. Defined as:
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
		@endverbatim
	*/
	void SetValue( const std::string& _value )		{ value = _value; }

	/// One step up the DOM.
	TiXmlNode* Parent() const					{ return parent; }

	TiXmlNode* FirstChild()	const	{ return (TiXmlNode*) firstChild; }  ///< The first child of this node. Will be null if there are no children.
	TiXmlNode* FirstChild( const std::string& value ) const;					 ///< The first child of this node with the matching 'value'. Will be null if none found.
	
	TiXmlNode* LastChild() const	{ return (TiXmlNode*) lastChild; }		/// The last child of this node. Will be null if there are no children.
	TiXmlNode* LastChild( const std::string& value ) const;			/// The last child of this node matching 'value'. Will be null if there are no children.
		
	/// Add a new node related to this. Adds a child past the LastChild.
	bool InsertEndChild( const TiXmlNode& addThis );					

	/// Add a new node related to this. Adds a child before the specified child.
	bool InsertBeforeChild( TiXmlNode* beforeThis, const TiXmlNode& addThis );

	/// Add a new node related to this. Adds a child after the specified child.
	bool InsertAfterChild(  TiXmlNode* afterThis, const TiXmlNode& addThis );
	
	/// Replace a child of this node.
	bool ReplaceChild( TiXmlNode* replaceThis, const TiXmlNode& withThis );
	
	/// Delete a child of this node.
	bool RemoveChild( TiXmlNode* removeThis );

	/// Navigate to a sibling node.
	TiXmlNode* PreviousSibling() const			{ return (TiXmlNode*) prev; }
	/// Navigate to a sibling node.
	TiXmlNode* PreviousSibling( const std::string& ) const;
	
	/// Navigate to a sibling node.
	TiXmlNode* NextSibling() const				{ return (TiXmlNode*) next; }
	/// Navigate to a sibling node with the given 'value'.
	TiXmlNode* NextSibling( const std::string& ) const;

	/// Query the type (as an enumerated value, above) of this node.
	virtual int Type()	{ return type; }

	TiXmlDocument* ToDocument()	const	{ return ( type == DOCUMENT ) ? (TiXmlDocument*) this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlElement*  ToElement() const	{ return ( type == ELEMENT  ) ? (TiXmlElement*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlComment*  ToComment() const	{ return ( type == COMMENT  ) ? (TiXmlComment*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlUnknown*  ToUnknown() const	{ return ( type == UNKNOWN  ) ? (TiXmlUnknown*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlText*	   ToText()    const	{ return ( type == TEXT     ) ? (TiXmlText*)     this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.

	virtual TiXmlNode* Clone() const = 0;

  protected:
	TiXmlNode( int type, TiXmlDocument* doc );
	virtual const char* Parse( const char* ) = 0;

	// The node is passed in by ownership. This object will delete it.
	bool InsertEndChild( TiXmlNode* addThis );
	// Figure out what is at *p, and parse it. Return a node if
	// successful, and update p.
	TiXmlNode* IdentifyAndParse( const char** p, TiXmlDocument* doc );

	void CopyToClone( TiXmlNode* target ) const	{ target->value = value; }

	TiXmlDocument*	document;
	TiXmlNode*		parent;		
	int				type;
	TiXmlBase*		firstChild;
	TiXmlBase*		lastChild;
	std::string		value;
};


/** An attribute is a name-value pair. Elements have an arbitrary
	number of attributes, each with a unique name.

	@note The attributes are not TiXmlNodes. There is a possibility
		  this will change in the future.
*/
class TiXmlAttribute : public TiXmlBase
{
  public:
	/// Construct an empty attribute.
	TiXmlAttribute( TiXmlDocument* doc = 0) : document( doc )	{}
	/// Construct an attribute with a name and value.
	TiXmlAttribute( const std::string& _name, const std::string& _value )	: name( _name ), value( _value ) {}

	const std::string& Name()  const { return name; }		///< Return the name of this attribute.
	const std::string& Value() const { return value; }		///< Return the value of this attribute.

	void SetName( const std::string& _name )	 { name = _name; }		///< Set the name of this attribute.
	void SetValue( const std::string& _value ) { value = _value; }		///< Set the value.

	/// Get the next sibling attribute in the DOM. Returns null at end.
	TiXmlAttribute* Next()			{ return (TiXmlAttribute*) next; }	// The backcast relies on their being only attributes in an attribute linked list.
	/// Get the previous sibling attribute in the DOM. Returns null at beginning.
	TiXmlAttribute* Previous()		{ return (TiXmlAttribute*) prev; }

	bool operator==( const TiXmlAttribute& rhs ) const { return rhs.name == name; }
	bool operator<( const TiXmlAttribute& rhs )	 const { return name < rhs.name; }
	bool operator>( const TiXmlAttribute& rhs )  const { return name > rhs.name; }

	/*	[internal use] 
		Attribtue parsing starts: first letter of the name
						 returns: the next char after the value end quote
	*/	
	const char* Parse( const char* );
	// [internal use] 
	virtual void Print( FILE* fp, int depth );

  private:
	TiXmlDocument*	document;
	std::string		name;
	std::string		value;
};


/** The element is a container class. It has a value, the element name,
	and can contain other elements, text, comments, and unknowns.
	Elements also contain an arbitrary number of attributes.
*/
class TiXmlElement : public TiXmlNode
{
  public:
	/// Construct an empty element.
	TiXmlElement( TiXmlDocument* doc = 0);
	virtual ~TiXmlElement();

	/** Given an attribute name, attribute returns the value
		for the attribute of that name, or null if none exists.
	*/
	const std::string* Attribute( const std::string& name ) const;

	/** Given an attribute name, attribute returns the value
		for the attribute of that name, or null if none exists.
	*/
	const std::string* Attribute( const std::string& name, int* i ) const;

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const std::string& name, 
					   const std::string& value );

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const std::string& name, 
					   int value );

	/** Deletes an attribute with the given name.
	*/
	void RemoveAttribute( const std::string& name );

	TiXmlAttribute* FirstAttribute()	{ return firstAttrib; }		///< Access the first attribute in this element.
	TiXmlAttribute* LastAttribute()		{ return lastAttrib; }		///< Access the last attribute in this element.

	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNode* Clone() const;
	// [internal use] 
	virtual void Print( FILE* fp, int depth );

  protected:
	/*	[internal use] 
		Attribtue parsing starts: next char past '<'
						 returns: next char past '>'
	*/	
	virtual const char* Parse( const char* );
	const char* ReadValue( const char* p );

  private:
	TiXmlAttribute* firstAttrib;
	TiXmlAttribute* lastAttrib;
};


/**	An XML comment.
*/
class TiXmlComment : public TiXmlNode
{
  public:
	/// Constructs an empty comment.
	TiXmlComment( TiXmlDocument* doc = 0) : TiXmlNode( TiXmlNode::COMMENT, doc ) {}
	virtual ~TiXmlComment()	{}

	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNode* Clone() const;
	// [internal use] 
	virtual void Print( FILE* fp, int depth );

  protected:
	/*	[internal use] 
		Attribtue parsing starts: at the ! of the !--
						 returns: next char past '>'
	*/	
	virtual const char* Parse( const char* );
};


/** XML text. Contained in an element.
*/
class TiXmlText : public TiXmlNode
{
  public:
	TiXmlText( TiXmlDocument* doc = 0)  : TiXmlNode( TiXmlNode::TEXT, doc ) {}
	virtual ~TiXmlText() {}


	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNode* Clone() const;
	// [internal use] 
	virtual void Print( FILE* fp, int depth );
	// [internal use] 	
	bool Blank();	// returns true if all white space and new lines

	/*	[internal use] 
		Attribtue parsing starts: First char of the text
						 returns: next char past '>'
	*/	
	virtual const char* Parse( const char* );
};


/** Any tag that tinyXml doesn't recognize is save as an 
	unknown. It is a tag of text, but should not be modified.
	It will be written back to the XML, unchanged, when the file 
	is saved.
*/
class TiXmlUnknown : public TiXmlNode
{
  public:
	TiXmlUnknown( TiXmlDocument* doc = 0) : TiXmlNode( TiXmlNode::UNKNOWN, doc ) {}
	virtual ~TiXmlUnknown() {}

	// [internal use] 	
	virtual TiXmlNode* Clone() const;
	// [internal use] 	
	virtual void Print( FILE* fp, int depth );

  protected:
	/*	[internal use] 
		Attribute parsing starts: First char of the text
						 returns: next char past '>'
	*/	
	virtual const char* Parse( const char* );
};


/** Advanced feature.
	If you want to use your own subclasses of Element, Text, Comment,
	and Unknown, you need to override the methods of factory.
	The first method call made on TiXmlDocument should be 
	SetFactory.
*/
class TiXmlFactory
{
  public:
	virtual TiXmlElement*	CreateElement( const TiXmlNode* parent, TiXmlDocument* doc )		{ return new TiXmlElement( doc ); }
	virtual TiXmlText*		CreateText( const TiXmlNode* parent, TiXmlDocument* doc )			{ return new TiXmlText( doc ); }
	virtual TiXmlComment*	CreateComment( const TiXmlNode* parent, TiXmlDocument* doc )		{ return new TiXmlComment( doc ); }
	virtual TiXmlUnknown*	CreateUnknown( const TiXmlNode* parent, TiXmlDocument* doc )		{ return new TiXmlUnknown( doc ); }
};


/** Always the top level node. A document binds together all the
	XML pieces. It can be saved, loaded, and printed to the screen.
	The 'value' of a document node is the xml file name.
*/
class TiXmlDocument : public TiXmlNode
{
  public:
	/// Create an empty document, that has no name.
	TiXmlDocument();
	/// Create a document with a name. The name of the document is also the filename of the xml.
	TiXmlDocument( const std::string& documentName );
	
	virtual ~TiXmlDocument() { delete factory; }

	/** An advanced feature, Set Factory allows a 
		user to subclass the TinyXml objects.
		The factory class is passed in by ownership and will
		be deleted by the document. This should be the first call
		after the document is constructed.
	*/
	void SetFactory( TiXmlFactory* factory );

	/// Load a file using the current document value. Returns true if successful.
	bool LoadFile();
	/// Save a file using the current document value. Returns true if successful.
	bool SaveFile();
	/// Load a file using the given filename. Returns true if successful.
	bool LoadFile( const std::string& filename );
	/// Save a file using the given filename. Returns true if successful.
	bool SaveFile( const std::string& filename );

	/// Parse the given null terminated block of xml data.
	const char* Parse( const char* );
	
	/// If, during parsing, a error occurs, Error will be set to true.
	bool Error()						{ return error; }
	/// Contains a textual (english) description of the error if one occurs.
	const std::string& ErrorDesc()		{ return errorDesc; }

	/// Write the document to a file -- usually invoked by SaveFile.
	virtual void Print( FILE* fp, int depth = 0 );
	/// Dump the document to standard out.
	void Print()										{ Print( stdout, 0 ); }
  
	// [internal use] 	
	virtual TiXmlNode* Clone() const;
	// [internal use] 	
	void SetError( const std::string& err ) {	error = true; 
												errorDesc = err; }
	// [internal use]
	TiXmlFactory* Factory() const	{ return factory; }

  private:
	TiXmlFactory* factory;
	bool error;
	std::string errorDesc;	
};

#endif

