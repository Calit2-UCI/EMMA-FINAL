#!/bin/bash
#

echo
echo "Template project updates checker v1.0"
echo

update=false
while true
do

directory=$1
echo "entering "$directory
cd $directory


	if [ -f currentTemplateRevision.log ] 
	then
		currentTemplate=$(cat currentTemplateRevision.log)
		echo "Your current template revision is: "$currentTemplate
		
		if [ -f template.log ] 
		then
			newTemplate=$( cat template.log)
			echo "last template revision is: "$newTemplate

			if [ $currentTemplate -lt $newTemplate ]
			then
				echo "You need to update your .xcodeproj"
				update=true;	

			else
				echo "ABORT: You are already updated to the most recent .xcodeproj template"
				break
			fi
		else
			echo "ABORT: Missing file template.log"
			break
		fi
	else
		echo "You don't have the currentTemplateRevision.log file, we are going to create it and update your .xcodeproj to the template"
		update=true;
	fi


if [ update ]
then
                                echo "Do you want to update it now? A backup of your current .xcodeproj will be created, as .xcodeproj.orig"
                                echo "enter (y)es or (n)o"
                                read -n 1 response
                                echo
                                case $response in
                                        y) echo "your reply is yes, so, let's do it"
                                        ;;

                                        n) echo "your reply is no. Maybe later"
                                        break
                                        ;;

                                        *) echo "ABORT: invalid response"
                                        break
                                        ;;
                                esac

                                        #Check if project exists, and in that case, copy template on it

                                        lsList=$(ls | grep xcodeproj.template)
                                        counter=0
                                        for x in $lsList
                                        do
                                                counter=$(expr $counter + 1)
                                        done

                                        if [ $counter -gt 1 ]
                                        then
                                                echo "ABORT: too much projects in this folder: "$counter
                                                echo

                                                for x in $lsList
                                                do
                                                        echo $x
                                                done
                                                break

                                        elif [ $counter -lt 1 ]
					then
						echo "ABORT, project not found"
						break
					else
						lsList=${lsList%".template"}
						echo "let's update "$lsList
					fi

					if [ -e $lsList".orig" ]
					then
						echo "removing old .orig"
						rm -r $lsList".orig"
					fi

                                        command="cp -r "$lsList" "$lsList".orig"
					eval $command
					if [ $? == 0 ]
                                        then
                                                echo "Created backup of .xcodeproj"
						rm -r $lsList
                                        else
                                                echo "WARNING: cannot create a backup of .xcodeproj, directory not found."
				                echo "Do you want to continue anyway?"
                                		echo "enter (y)es or (n)o"
                                		read -n 1 response
                                		echo
                                		case $response in
                                        		y) echo "your reply is yes, so, let's continue"
                                        		;;

                                        		n) echo "your reply is no. Maybe later. let's abort"
                                        		break
                                        		;;

                                       			*) echo "ABORT: invalid response."
                                       			break
                                       			;;
                                		esac
                                        fi
					
					if [ ! -e $lsList ]
					then
						mkdir $lsList
					fi
                                        command="cp -r "$lsList".template/* "$lsList"/"
					eval $command
                                        if [ $? == 0 ]
                                        then
                                                echo "Updating .xcodeproj"
                                        else
                                                echo "ABORT: ERROR updating .xcodeproj, restoring old .xcodeproj"
						mv $lsList".orig" $lsList
						break;
					fi
					
                                        cp template.log currentTemplateRevision.log
                                        if [ $? == 0 ]
                                        then
                                                echo "Updating currentTemplateRevision.log"
                                        else
                                                echo "ERROR updating currentTemplateRevision.log"
                                                break
                                        fi
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

