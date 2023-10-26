FROM gcc:13.2.0

WORKDIR /ray-tracing
VOLUME $WORKDIR/outputs
COPY src/ ./src/

RUN g++ src/main.cc -o main

CMD ["/bin/sh", "-c", "./main > outputs/image.ppm"]