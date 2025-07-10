#!/bin/bash

docker build -t sshs_asg1 -f Dockerfile .
docker run -v .:/workspace -it sshs_asg1
