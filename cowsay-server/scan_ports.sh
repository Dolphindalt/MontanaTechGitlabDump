#!/bin/bash
for i in `seq 30000 30129`; do
	if [[ $(echo -ne '\n' | nc localhost $i) ]]; then
		echo Found $i
	fi
done	
