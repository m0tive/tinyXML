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

#ifndef TIXML_STRING
#define TIXML_STRING

/**
   TiXmlString is an emulation of the std::string template. \n
   Its purpose is to allow compiling TinyXML on compilers with no or poor STL support \n
   Only the member functions relevant to the TinyXML project have been implemented \n
   The buffer allocation is made by a simplistic power of 2 like mechanism : if we increase
   a string and there's no more room, we allocate a buffer twice as big as we need.
*/
class TiXmlString
{
public :
   /// TiXmlString constructor, based on a string
   TiXmlString (const char * instring);

   /// TiXmlString empty constructor
   TiXmlString ()
   {
      allocated = 0;
      cstring = NULL;
   }

   /// TiXmlString copy constructor
   TiXmlString (const TiXmlString & copy);

   /// TiXmlString destructor
   ~ TiXmlString ()
   {
      empty_it ();
   }

   /// Convert a TiXmlString into a classical char *
   const char * c_str () const
   {
      if (allocated)
         return cstring;
      return "";
   }

   /// Return the length of a TiXmlString
   unsigned length () const;

   /// TiXmlString = operator
   void operator = (const char * content);

   /// = operator
   void operator = (const TiXmlString & copy);

   /// += operator. Maps to append
   operator += (const char * suffix)
   {
      append (suffix);
   }

   /// += operator. Maps to append
   operator += (char single)
   {
      append (single);
   }

   /// += operator. Maps to append
   operator += (TiXmlString & suffix)
   {
      append (suffix);
   }
   bool operator == (const TiXmlString & compare) const;
   bool operator < (const TiXmlString & compare) const;
   bool operator > (const TiXmlString & compare) const;

   /// Checks if a TiXmlString is empty
   bool empty () const
   {
      return length () ? false : true;
   }

   /// Checks if a TiXmlString contains only whitespace (same rules as isspace)
   bool isblank () const;

   /// single char extraction
   char at (unsigned index) const
   {
      if (index >= length ())
         return 0;
      return cstring [index];
   }

   /// find a char in a string. Return TiXmlString::notfound if not found
   unsigned find (char lookup) const
   {
      return find (lookup, 0);
   }

   /// find a char in a string from an offset. Return TiXmlString::notfound if not found
   unsigned find (char tofind, unsigned offset) const
   {
      char * lookup;

      if (offset >= (int) length ())
         return notfound;
      for (lookup = cstring + offset; * lookup; lookup++)
         if (* lookup == tofind)
            return lookup - cstring;
      return notfound;
   }

   /// Function to reserve a big amount of data when we know we'll need it. Be aware that this
   /// function clears the content of the TiXmlString if any
   void reserve (unsigned size)
   {
      empty_it ();
      if (size)
      {
         allocated = size;
         cstring = new char [size];
         cstring [0] = 0;
      }
   }
   /// [] operator : Maps to at
   char operator [] (int index) const
   {
      return at (index);
   }

   /// Error value for find primitive 
	enum {notfound = 0xffffffff};

protected :

   /// The base string
   char * cstring;
   /// Number of chars allocated
   unsigned allocated;

   /// New size computation. It is simplistic right now : it returns twice the amount
   /// we need
   unsigned assign_new_size (unsigned minimum_to_allocate)
   {
      return minimum_to_allocate * 2;
   }

   /// Internal function that clears the content of a TiXmlString
   void empty_it ()
   {
      if (cstring)
         delete [] cstring;
      cstring = NULL;
      allocated = 0;
   }

   void append (const char * suffix);

   /// append function for another TiXmlString
   void append (const TiXmlString & suffix)
   {
      append (suffix . c_str ());
   }

   /// append for a single char. This could be improved a lot if needed
   void append (char single)
   {
      char smallstr [2];
      smallstr [0] = single;
      smallstr [1] = 0;
      append (smallstr);
   }

} ;

/** 
   TiXmlOutStream is an emulation of std::ostream. It is based on TiXmlString. \n
   Only the operators that we need for TinyXML has been developped.
*/
class TiXmlOutStream : public TiXmlString
{
public :
   TiXmlOutStream () : TiXmlString () {}

   /// TiXmlOutStream << operator. Maps to TiXmlString::append
   TiXmlOutStream & operator << (const char * in)
   {
      append (in);
      return (* this);
   }

   /// TiXmlOutStream << operator. Maps to TiXmlString::append
   TiXmlOutStream & operator << (const TiXmlString & in)
   {
      append (in . c_str ());
      return (* this);
   }

   void write (const char *, int);

   /// Put a simple char to the output stream
   void put (char outchar)
   {
      append (outchar);
   }
} ;

/**
   TiXmlInStream is an emulation of std::istream. \n
   Only the operators that we need for TinyXML has been developped. \n
   It is virtualized so as to be able to have a custom implementation and one that
   consumes a std::istream, depending on the TIXML_USE_STL status
*/
class TiXmlInStream
{
public :
   TiXmlInStream () {} 
   /// TiXmlInStream destructor
   virtual ~ TiXmlInStream () {}
   /// Check if there are still some chars to consume
   virtual bool good () const = 0;
   /// Consume one char
   virtual char get () = 0;
   /// Check for the next char but don't consume it
   virtual char peek () = 0;
};

/**
   TiXmlInStreamOwn is the custom implementation of an input stream
*/
class TiXmlInStreamOwn : public TiXmlInStream
{
public :
   TiXmlInStreamOwn::TiXmlInStreamOwn (const char * instring);
   /// TiXmlInStream destructor
   virtual ~ TiXmlInStreamOwn ()
   {
      if (valid && cstring)
         delete [] cstring;
   }

   /// Check if there are still some chars to consume
   virtual bool good () const
   {
      if (! valid || ! * consumer)
         return false;
      return true;
   }
   /// Consume one char
   virtual char get ()
   {
      if (! good ())
         return 0;
      consumer++;
      return * (consumer - 1);
   }
   /// Check for the next char but don't consume it
   virtual char peek () 
   {
      if (! good ())
         return 0;
      return * consumer;
   }
protected :
   /// The input stream contents
   char * cstring;
   /// The current consumer pointer
   char * consumer;
   /// true if the string is valid
   bool valid;
} ;

#ifdef TIXML_USE_STL
   /**
      TiXmlInStreamStl is the implementation of an input stream that consumes every input
      from the std::istream
   */
   class TiXmlInStreamStl : public TiXmlInStream 
   {
   public :
      TiXmlInStreamStl (std::istream & _s) : s (_s), TiXmlInStream ()
      {
      }
      virtual ~ TiXmlInStreamStl () {}
      virtual bool good () const {return s . good ();}
      virtual char get () {return s . get ();}
      virtual char peek () {return s . peek ();}
   protected :
      std::istream s;
   } ;
#endif

#endif
