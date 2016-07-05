#!/bin/bash
#

echo
echo "Template project updater v1.0"
echo

while true
do

if [$1 == ""]
then
        echo "ABORT: missing path to Xcode project"
        break
fi
directory=$1

echo "entering "$directory
cd $directory


lsList=$(ls | grep -v .template | grep -v .orig | grep .xcodeproj)
#lsList=$(ls)

counter=0
addToMercurial=true

for x in $lsList
do
	counter=$(expr $counter + 1)
done

if [ $counter -gt 1 ]
then
	echo "too much projects in this folder: "$counter
	echo 

	for x in $lsList
	do
		echo $x
	done

	break;

fi


hgTip=$(hg tip | grep "changeset:")

#modify the internal field separator to extract revision number from mercurial changeset
OIFS=$IFS
IFS=' :'

counter=0
for x in $hgTip
do
	counter=$(expr $counter + 1)
                        
	#store the number of the revision in $hgTip
	if [ $counter == 2 ]
	then 
	echo "tip revision: "$x
 		hgTip=$x
	fi
done

#get back to the default internal field separator
IFS=$OIFS

	
if [ -f "template.log" ]
then
	#read the number of the revision in template.log
	lastTemplateRevision=$(cat template.log)
	echo "last template revision: "$lastTemplateRevision

	
	#if the revision in template.log is the same as ht tip, exits
	if [ "$lastTemplateRevision" == "$hgTip" ]
	then
		echo "ABORT: Your template has been already updated, let's exit"
		break;
	fi
else
	echo "WARNING!! template.log does not exist, so it's supposed to be the first time creating the template"
	echo
	addToMercurial=true

fi

lsListTemplate=$lsList".template"
if [ ! -e "$lsListTemplate" ]
then
	mkdir $lsListTemplate
fi

#cp -r $lsList"/*" $lsList".template"
command="cp -r "$lsList"/* "$lsList".template/"
eval $command
if [ $? -eq 0 ]
then
	echo "Updating last template revision"
else
	echo
	echo "error Updating last template revision"
	break
fi

echo $hgTip > template.log
if [ $? -eq 0 ]
then
	echo "Updating template.log"
else
	echo
	echo "error Updating template.log"
	break;
fi

if $addToMercurial
then
	echo "adding .xcodeproj.template to mercurial"
	hg add $lsList".template"
	echo "adding template.log to mercurial"
	hg add template.log
fi
echo
echo "Update successful"
echo
echo "----------//----------"
echo
exit 0


done

echo
echo "----------//----------"
echo
exit -1
