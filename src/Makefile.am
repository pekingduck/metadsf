AUTOMAKE_OPTIONS = foreign
#ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS=-Wall -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDADD=-ltag -lz
bin_PROGRAMS = metadsf
metadsf_SOURCES = dsffile.cpp dsfheader.cpp dsfproperties.cpp main.cpp metadsf.cpp options.cpp utils.cpp
