FROM gcc:13.2.0

WORKDIR /ray-tracing
VOLUME $WORKDIR/outputs
COPY src/ ./src/
COPY test/ ./test/

RUN apt-get update
RUN apt-get -y install cmake

## IMAGE OUTPUT
# RUN g++ src/main.cc -o main
# CMD ["/bin/sh", "-c", "./main > outputs/image.ppm"]

## TESTING
RUN git clone -q https://github.com/google/googletest.git /googletest \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make && make install
RUN g++ test/vec3_test.cc /googletest/build/lib/libgtest.a -o vec3_test
CMD ["/bin/sh", "-c", "./vec3_test"]