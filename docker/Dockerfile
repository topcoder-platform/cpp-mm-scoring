FROM ubuntu:18.04

RUN groupadd --gid 1000 node \
  && useradd --uid 1000 --gid node --shell /bin/bash --create-home node

RUN apt-get update && apt-get install -y build-essential cmake git wget curl unzip libtinfo-dev libz-dev
RUN wget -qO- https://deb.nodesource.com/setup_8.x | bash - && apt-get install -y nodejs

RUN mkdir cpp-mm-scoring
COPY . /cpp-mm-scoring
WORKDIR /cpp-mm-scoring

RUN chown -R node /cpp-mm-scoring

USER node

RUN npm install

CMD ["/bin/bash"]
