#!/bin/bash

rm -f deploy/webserver
docker build -t httpserver.build .
docker run httpserver.build > deploy/binary.tar
cd deploy
tar -xvf binary.tar
rm -f binary.tar
docker build -t httpserver --file Dockerfile.run .

echo "---------------------------------------------------------------------|"
echo "Deleting the old httpserver and httpserver.build image"

exited_container=$(docker ps -q -f status=exited)

if [ -z "$exited_container" ]
then
	echo "no exited_container"
else
	docker rm $exited_container
fi

dead_image=$(docker images -f "dangling=true" -q)

if [ -z "$dead_image" ]
then
	echo "No dead_image"
else
	docker rmi $dead_image
fi
echo "---------------------------------------------------------------------|"


