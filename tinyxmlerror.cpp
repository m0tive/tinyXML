#include "tinyxml.h"

const char* TiXmlBase::errorString[ ERROR_STRING_COUNT ] =
{
	"No error",
	"Memory allocation failed.",
	"Error parsing Element.",
	"Failed to read Element name",
	"Error reading Element value.",
	"Error reading Attributes.",
	"Error: empty tag.",
	"Error reading end tag.",
	"Error parsing Unknown.",
	"Error parsing Comment.",
	"Error parsing Declaration.",
};
