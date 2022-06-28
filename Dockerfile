FROM ubuntu:20.04
ENV TZ=Europe/Amsterdam
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
## manual pages and Bash command completion
WORKDIR /home/osboxes/
RUN apt-get clean && apt-get -y update && apt-get -y upgrade && apt-get install -y git
#RUN mkdir osboxes
#WORKDIR home/osboxes/
RUN apt-get install -y --fix-missing \
    net-tools\
    iputils-ping\
    nano\
    g++ make cmake \
    libgmp-dev libssl-dev \
    libboost-all-dev \
    libboost-system1.67.0
RUN git clone -b develop https://github.com/MaxOpperman/sfe-policy-eval.git --recursive
