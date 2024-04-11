FROM ubuntu:22.04
WORKDIR /src
COPY ./src /src
RUN apt-get update && \
    apt-get -y install gcc && \
    apt-get -y install make && \
    apt-get -y install freeglut3-dev 
CMD ["make"]
