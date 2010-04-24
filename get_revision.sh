#!/bin/bash

echo -n "Finding revision "
pwd
revnum=`hg log -l 1 --template '{node}'`

if [ -f src/svn_revision.h ] ; then
    oldrevnum=`cut -f 2 -d '"' "src/scm_revision.h"`
    if [ "${revnum}" = "${oldrevnum}" ] ; then
	    echo "Same revsion as last time: not updating."
	    exit 0
    fi
fi

# Now write the constant declaration to the file:
echo "#define SCM_REVISION \"$revnum\"" > src/scm_revision.h
echo "Wrote revision $revnum to scm_revision.h"
