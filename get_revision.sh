#!/bin/bash

echo -n "Finding revision "
pwd
revnum=`svnversion`

if [ -f src/svn_revision.h ] ; then
    oldrevnum=`cut -f 2 -d '"' "src/svn_revision.h"`
    if [ "${revnum}" = "${oldrevnum}" ] ; then
	    echo "Same revsion as last time: not updating."
	    exit 0
    fi
fi

# Now write the constant declaration to the file:
echo "#define SVN_REVISION \"$revnum\"" > src/svn_revision.h
echo "Wrote revision $revnum to svn_version.h"
