#****************************************************************************
#
# Makefil for TinyXml test.
# Lee Thomason
# www.grinninglizard.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

DEBUG_CFLAGS     := -Wall -Wno-unknown-pragmas -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O2

#LIBS		 := -L/usr/X11R6/lib -lpthread -lSDL -ldl -lm -lz 
LIBS		 :=  

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg
   CXXFLAGS := ${CXXFLAGS} -pg
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${PROFILE})
  DEFS := -DSDL -DLINUX -DSPEEDTEST
else
  DEFS := -DSDL -DLINUX
endif

#****************************************************************************
# Include paths
#****************************************************************************

INCS := -I/usr/include/g++-2 -I/usr/local/include


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************

OUTPUT := xmltest

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************

SRCS := tinyxml.cpp tinyxmlparser.cpp xmltest.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

clean:
	-rm -f core ${OBJS} ${OUTPUT}

depend:
	makedepend ${INCS} ${SRCS}

# DO NOT DELETE

tinyxml.o: tinyxml.h /usr/include/g++-2/string
tinyxml.o: /usr/include/g++-2/std/bastring.h /usr/include/g++-2/cstddef
tinyxml.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
tinyxml.o: /usr/include/g++-2/std/straits.h /usr/include/g++-2/cctype
tinyxml.o: /usr/include/ctype.h /usr/include/features.h
tinyxml.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
tinyxml.o: /usr/include/bits/types.h /usr/include/endian.h
tinyxml.o: /usr/include/bits/endian.h /usr/include/g++-2/cstring
tinyxml.o: /usr/include/string.h /usr/include/g++-2/alloc.h
tinyxml.o: /usr/include/g++-2/stl_config.h /usr/include/_G_config.h
tinyxml.o: /usr/include/g++-2/stl_alloc.h /usr/include/g++-2/iostream.h
tinyxml.o: /usr/include/g++-2/streambuf.h /usr/include/libio.h
tinyxml.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
tinyxml.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
tinyxml.o: /usr/include/sys/select.h /usr/include/bits/select.h
tinyxml.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
tinyxml.o: /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h
tinyxml.o: /usr/include/sched.h /usr/include/bits/sched.h
tinyxml.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
tinyxml.o: /usr/include/bits/sigthread.h /usr/include/g++-2/iterator
tinyxml.o: /usr/include/g++-2/stl_relops.h /usr/include/g++-2/stl_iterator.h
tinyxml.o: /usr/include/g++-2/cassert /usr/include/g++-2/std/bastring.cc
tinyxml.o: /usr/include/stdio.h /usr/include/bits/stdio_lim.h
tinyxmlparser.o: tinyxml.h /usr/include/g++-2/string
tinyxmlparser.o: /usr/include/g++-2/std/bastring.h /usr/include/g++-2/cstddef
tinyxmlparser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
tinyxmlparser.o: /usr/include/g++-2/std/straits.h /usr/include/g++-2/cctype
tinyxmlparser.o: /usr/include/ctype.h /usr/include/features.h
tinyxmlparser.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
tinyxmlparser.o: /usr/include/bits/types.h /usr/include/endian.h
tinyxmlparser.o: /usr/include/bits/endian.h /usr/include/g++-2/cstring
tinyxmlparser.o: /usr/include/string.h /usr/include/g++-2/alloc.h
tinyxmlparser.o: /usr/include/g++-2/stl_config.h /usr/include/_G_config.h
tinyxmlparser.o: /usr/include/g++-2/stl_alloc.h /usr/include/g++-2/iostream.h
tinyxmlparser.o: /usr/include/g++-2/streambuf.h /usr/include/libio.h
tinyxmlparser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
tinyxmlparser.o: /usr/include/stdlib.h /usr/include/sys/types.h
tinyxmlparser.o: /usr/include/time.h /usr/include/sys/select.h
tinyxmlparser.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
tinyxmlparser.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
tinyxmlparser.o: /usr/include/assert.h /usr/include/pthread.h
tinyxmlparser.o: /usr/include/sched.h /usr/include/bits/sched.h
tinyxmlparser.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
tinyxmlparser.o: /usr/include/bits/sigthread.h /usr/include/g++-2/iterator
tinyxmlparser.o: /usr/include/g++-2/stl_relops.h
tinyxmlparser.o: /usr/include/g++-2/stl_iterator.h /usr/include/g++-2/cassert
tinyxmlparser.o: /usr/include/g++-2/std/bastring.cc /usr/include/stdio.h
tinyxmlparser.o: /usr/include/bits/stdio_lim.h
xmltest.o: tinyxml.h /usr/include/g++-2/string
xmltest.o: /usr/include/g++-2/std/bastring.h /usr/include/g++-2/cstddef
xmltest.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
xmltest.o: /usr/include/g++-2/std/straits.h /usr/include/g++-2/cctype
xmltest.o: /usr/include/ctype.h /usr/include/features.h
xmltest.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
xmltest.o: /usr/include/bits/types.h /usr/include/endian.h
xmltest.o: /usr/include/bits/endian.h /usr/include/g++-2/cstring
xmltest.o: /usr/include/string.h /usr/include/g++-2/alloc.h
xmltest.o: /usr/include/g++-2/stl_config.h /usr/include/_G_config.h
xmltest.o: /usr/include/g++-2/stl_alloc.h /usr/include/g++-2/iostream.h
xmltest.o: /usr/include/g++-2/streambuf.h /usr/include/libio.h
xmltest.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
xmltest.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
xmltest.o: /usr/include/sys/select.h /usr/include/bits/select.h
xmltest.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
xmltest.o: /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h
xmltest.o: /usr/include/sched.h /usr/include/bits/sched.h
xmltest.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
xmltest.o: /usr/include/bits/sigthread.h /usr/include/g++-2/iterator
xmltest.o: /usr/include/g++-2/stl_relops.h /usr/include/g++-2/stl_iterator.h
xmltest.o: /usr/include/g++-2/cassert /usr/include/g++-2/std/bastring.cc
xmltest.o: /usr/include/stdio.h /usr/include/bits/stdio_lim.h
