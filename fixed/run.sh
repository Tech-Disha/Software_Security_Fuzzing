#!/bin/bash

docker build -t sshs_asg2 -f Dockerfile .
docker run -v .:/workspace -it sshs_asg2

