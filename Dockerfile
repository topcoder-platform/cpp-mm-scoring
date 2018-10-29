FROM ubuntu:16.04

RUN groupadd --gid 1000 node \
  && useradd --uid 1000 --gid node --shell /bin/bash --create-home node
WORKDIR /home/node

RUN apt-get update && apt-get install -y build-essential cmake git wget unzip libtinfo-dev libz-dev
RUN wget -qO- https://deb.nodesource.com/setup_8.x | bash - && apt-get install -y nodejs
# cling download link may expire and use current date
RUN wget https://root.cern.ch/download/cling/cling_2018-10-19_ubuntu16.tar.bz2 && tar xvjf cling_2018-10-19_ubuntu16.tar.bz2 && rm cling_2018-10-19_ubuntu16.tar.bz2
RUN wget https://github.com/nlohmann/json/archive/v3.3.0.zip -O json-3.3.0.zip && unzip json-3.3.0.zip && rm json-3.3.0.zip
RUN cp -r /home/node/cling_2018-10-19_ubuntu16/include/* /usr/local/include
RUN cp -r /home/node/cling_2018-10-19_ubuntu16/lib /home/node/cling_2018-10-19_ubuntu16/libexec
RUN echo "/home/node/cling_2018-10-19_ubuntu16/libexec/lib" > /etc/ld.so.conf.d/cling.conf && ldconfig
USER node
RUN mkdir node-addon
COPY . /home/node/node-addon
WORKDIR /home/node/node-addon
# you may run RUN /bin/bash -c ". env.sh ; npm install" but environements will lose later
ENV CLING_DIR /home/node/cling_2018-10-19_ubuntu16/lib/cmake/cling
ENV LLVM_INSTALL_PREFIX /home/node/cling_2018-10-19_ubuntu16/libexec
ENV CLING_LIB_DIR /home/node/cling_2018-10-19_ubuntu16/libexec/lib
ENV NLOHMANN_JSON_INCLUDE_DIR /home/node/json-3.3.0/include
RUN npm install && npm run lint && npm test && npm run rebuild-demo
# avoid issue later
ENV LD_LIBRARY_PATH /home/node/cling_2018-10-19_ubuntu16/libexec/lib
USER node
CMD ["/bin/bash"]

