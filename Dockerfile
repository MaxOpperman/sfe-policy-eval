FROM ubuntu:20.04
ENV TZ=Europe/Amsterdam
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
## manual pages and Bash command completion
WORKDIR /home/osboxes/
RUN apt-get clean && apt-get -y update && apt-get install -y git
#RUN mkdir osboxes
#WORKDIR home/osboxes/
RUN apt-get install -y \
     g++ make cmake \
      libgmp-dev libssl-dev \
      libboost-all-dev \
      libboost-system1.67.0
RUN git clone https://github.com/MaxOpperman/sfe-policy-eval.git --recursive
