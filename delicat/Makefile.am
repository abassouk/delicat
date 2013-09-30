bin_PROGRAMS=delicat
delicat_SOURCES=delicat.c xmalloc.c system.h aclocal.m4 delicat-opt.c delicat-opt.h

#  add " -lm" to the following if delicat requires the math library
delicat_LDADD=-lopts

EXTRA_DIST=delicat.lsm.in delicat.spec.in delicat-opt.def delicat.1 manpage.tpl

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

delicat.o: delicat-opt.h
delicat-opt.o: delicat-opt.c
delicat-opt.c delicat-opt.h: $(srcdir)/delicat-opt.def
	autogen -L/usr/share/autogen $(srcdir)/delicat-opt.def

delicat.1: $(srcdir)/delicat-opt.def
	autogen -T manpage.tpl  $(srcdir)/delicat-opt.def
	awk -f updater delicat.1 > tmp$$$$ && mv tmp$$$$ delicat.1
#	rm updater

# info_TEXINFOS = delicat.texinfo
# delicat_TEXINFOS = gpl.texinfo

#  install the man page
man_MANS=delicat.1
