
FROM ubuntu:22.04


RUN apt-get update && apt-get install -y \
    build-essential \
    patch \
    gcc

WORKDIR /app

COPY json.c /app/json.c
COPY patch.patch /app/json.patch
COPY in/crash /app/crash

RUN patch /app/json.c < /app/json.patch

RUN gcc -o json_fixed /app/json.c

CMD ["./json_fixed", "/app/crash"]