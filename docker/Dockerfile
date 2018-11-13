FROM ubuntu:18.04

RUN groupadd --gid 1000 node \
  && useradd --uid 1000 --gid node --shell /bin/bash --create-home node
WORKDIR /home/node

RUN apt-get update
RUN apt-get install -y build-essential cmake git wget curl unzip libtinfo-dev libz-dev
RUN wget -qO- https://deb.nodesource.com/setup_8.x | bash - && apt-get install -y nodejs

RUN mkdir node-addon
COPY . /home/node/node-addon
WORKDIR /home/node/node-addon

RUN chown -R node /home/node

USER node

RUN npm install

# avoid issue later
CMD ["/bin/bash"]
