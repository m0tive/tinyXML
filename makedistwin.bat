del .\dist\*.*
del .\docs\*.*

doxygen dox

copy readme.txt dist
copy *.cpp dist
copy *.h dist
copy *.dsp dist
copy test0.xml dist
copy test1.xml dist
copy test2.xml dist

mkdir .\dist\docs
copy docs .\dist\docs

