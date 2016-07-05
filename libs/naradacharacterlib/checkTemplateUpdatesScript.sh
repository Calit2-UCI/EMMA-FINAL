#!/bin/bash
#


currentDirectory=$PWD
scriptDirectory=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

#let's get into the scriptDirectory
cd $scriptDirectory
result=$(find ./ -name *.xcodeproj.template)

for x in $result
do
        echo
        echo
        echo "Lets run updateTemplate for: "$x
        echo "-------------------------------------------------------------------------------------------"
        ./routineToCheckTemplateUpdates.sh $x/..
        cd $scriptDirectory
done
echo
echo
echo "all the routines to update templates have been called. Check the output to be sure that everything has gone well"
echo
echo
cd $currentDirectory

exit 0
