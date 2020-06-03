#!/bin/bash
set -e
set -x

# if a command is given with the run then it goes to else and the command is executed instead of the predifined procedure
if [ "$1" = '-rps' ]; then
	# default configuration, it warms up the server and then starts the load tesing 
	echo "dc-server, 11211" > "/usr/src/memcached/memcached_client/servers.txt"
	/usr/src/memcached/memcached_client/loader \
		-a /usr/src/memcached/twitter_dataset/twitter_dataset_unscaled \
		-o /usr/src/memcached/twitter_dataset/twitter_dataset_5x \
		-s /usr/src/memcached/memcached_client/servers.txt \
		-w 4 -S 5 -D 2048 -j

	# eixe dipli paula prin to loader
	/usr/src/memcached/memcached_client/loader \
		-a /usr/src/memcached/twitter_dataset/twitter_dataset_5x \
		-s /usr/src/memcached/memcached_client/servers.txt \
		-g 0.8 -c 200 -w 4 -e -r "$2" -t 123 -T 120

else
	# custom command
	exec "$@"
fi