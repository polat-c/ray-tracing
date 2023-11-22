FROM gcc:13.2.0

WORKDIR /ray-tracing
COPY external/ ./external/

RUN apt-get update
RUN apt-get -y install cmake

## TESTING
RUN git clone -q https://github.com/google/googletest.git /googletest \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make && make install