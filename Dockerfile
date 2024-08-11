FROM ubuntu
COPY . /app
WORKDIR /app
ENV XDG_RUNTIME_DIR=/tmp
RUN apt update && apt upgrade -y && \
    apt install build-essential libsdl2-dev -y && \
    cd examples/sdl2 && ./build.sh && cd ../..
CMD ./dist/goblin3d_example
