FROM ubuntu:22.04

ENV AFL_SKIP_CPUFREQ=0
ENV AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    git \
    make \
    sudo \
    wget \
    gdb \
    python3 \
    python3-pip \
    && apt-get clean

RUN git clone https://github.com/AFLplusplus/AFLplusplus.git /AFLplusplus
WORKDIR /AFLplusplus
RUN make distrib && make install

WORKDIR /target
COPY json.c /target/json.c
RUN afl-clang-fast -o json_fuzz json.c
RUN mkdir -p /fuzz/in 
COPY in/seed /fuzz/in/
WORKDIR /fuzz

#CMD bash -c "afl-fuzz -i /fuzz/in -o /workspace/out /target/json_fuzz @@; exec /bin/bash"