#!/bin/bash
FILE=shiviz.txt
echo '(?<host>\S*) (?<clock>{.*})\n(?<event>.shiviz)' > $FILE
echo -e "\n" >> $FILE
cat *.log >> $FILE

