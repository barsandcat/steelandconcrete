#! /bin/bash

echo -n "Finding revision in "
pwd
revnum=`svnversion`

# Now write the constant declaration to the file:
echo "#define SVN_REVISION \"$revnum\"" > src/svn_revision.h
echo "Wrote revision $revnum to svn_version.h"
