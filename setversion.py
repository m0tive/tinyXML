# Python program to set the version.
##############################################


def fileProcess( name, lineFunction ):
	filestream = open( name, 'r' )
	if filestream.closed:
		print( "file " + name + " not open." )
		return

	output = ""
	print( "--- Processing " + name + " ---------" )
	while 1:
		line = filestream.readline()
		if not line: break
		output += lineFunction( line )
	filestream.close()
	
	if not output: return			# basic error checking
	
	print( "Writing file " + name )
	filestream = open( name, "w" );
	filestream.write( output );
	filestream.close()
	
	
def echoInput( line ):
	return line
	

import sys
major = input( "Major: " )
minor = input( "Minor: " )
build = input( "Build: " )

print "Setting dox, makedistlinux, tinyxml.h"
print "Version: " + `major` + "." + `minor` + "." + `build`

#### Write the tinyxml.h ####

def engineRule( line ):

	matchMajor = "const int TIXML_MAJOR_VERSION"
	matchMinor = "const int TIXML_MINOR_VERSION"
	matchBuild = "const int TIXML_PATCH_VERSION"

	if line[0:len(matchMajor)] == matchMajor:
		print "1)tinyxml.h Major found"
		return matchMajor + " = " + `major` + ";\n"

	elif line[0:len(matchMinor)] == matchMinor:
		print "2)tinyxml.h Minor found"
		return matchMinor + " = " + `minor` + ";\n"

	elif line[0:len(matchBuild)] == matchBuild:
		print "3)tinyxml.h Build found"
		return matchBuild + " = " + `build` + ";\n"

	else:
		return line;

fileProcess( "tinyxml.h", engineRule )


#### Write the dox ####

def doxRule( line ):

	match = "PROJECT_NUMBER"

	if line[0:len( match )] == match:
		print "dox project found"
		return "PROJECT_NUMBER = " + `major` + "." + `minor` + "." + `build` + "\n"

	else:
		return line;

fileProcess( "dox", doxRule )


#### Write the makedistlinux #####

def buildlinuxRule( line ):

	i = line.rfind( "_" )

	if i >= 4 and line[i] == "_" and line[i-2] == "_" and line[i-4] == "_":
		# This is ghetto. Should really use regular expressions.
		i -= 4
		print "makedistlinux instance found"
		return line[0:i] + "_" + `major` + "_" + `minor` + "_" + `build` + line[i+6:]
	else:
		return line

fileProcess( "makedistlinux", buildlinuxRule )
