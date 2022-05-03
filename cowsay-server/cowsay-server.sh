#!/bin/bash
#
# Network Service to provide random cow files saying random aphorisms 
# from the fortune command
#
# F18 CSCI466 Networks
#
ncat -l -k -n -p ${NETPORT} -c "cowsay -f \$(bash -c 'files=(/usr/share/cowsay/cows/*) && echo \${files[\$RANDOM%\${#files[@]}]##*/}') \$(fortune)"
#
# Explain how the syntax above implements the desired behavior
# it is helpful to draw a box model of the execute of the command expansion
# the shell is performing - remember command expansions are replaced with
# the result of the expansion
 
