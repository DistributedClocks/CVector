#!/bin/bash
FILE=shiviz.txt
echo '(?<host>\S*) (?<clock>{.*})\n(?<event>.*)' > $FILE
echo -e "\n" >> $FILE
cat *.log >> $FILE

