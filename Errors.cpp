/**
	Here is where all the errors are defined, it makes for an easier =
translation
	of the errors from one language to another, or simply change their =
parsing.
*/

#ifndef XML_Error_List
#define XML_Error_List

#include <string>

const int NumOfErrors =3D 18;
/**
@verbatim
Error #00:	No error occured
Error #01:	Memory error
Error #02:	<?xml tag appeared twice in the document
Error #03:	General Invalid XML syntax error
Error #04:	Couldn't read attribute name correctly
Error #05:	There wasn't a =3D char after the attribute's name
Error #06:	Attribute's value that is not contained in qoutes or double =
qoutes.
Error #07:	Unterminated string as attribute value
Error #08:	The array ended before finishing the parsing
Error #09:	Couldn't parse element name correctly
Error #11:	Couldn't read the ending tag of an open tag.
Error #12:	A comment was not closed successfully.
Error #13:	Can't find the char < where it should be
Error #14:	Can't find the char > where it should be
Error #15:	Can't find the <?xml at the beginning of the text
Error #16:	Can't find the closing ?> tag
Error #last:Tried to get an error whose number was too high.
@endverbatim
*/

std::string errorDesc[NumOfErrors] =3D {
	"There wasn't any error, well, yet :-)", //0
	"Error in memory allocation, it's likely that the OS can't find enough =
RAM.\nBuy more!" // 1
	"The \"<?xml\" tag appeared in an unexpected location",//2
	"Invalid XML syntax, sould enter more details about it",//3
	"Error reading attribute's name",//4
	"Error, no \"=3D\" character was found after reading attribute's name", =
//5
	"Error, attribute values must be contained within \" \" or \' \'",//6
	"Attribute value is an unterminated string", //7
	"The XML data stream ended unexpectedly.", //8
	"Error parsing Element name.",//9
	"Error parsing empty tag.",//10
	"Error reading end tag.", //11
	"Unterminated comment."//12
	"Unable to find open char <.",//13
	"Unable to find closing char >.",//14
	"Unable to find <?xml at the start of the header",//15
	"Unable to find closing ?> at the end of the header", //16
	"Try to get an error descrition whose errorID is too high. \nIn other =
words, this error either does not exist, or is so new it didn't got its =
error descrition yet." // NumOfErrors -1
};


#endif