FROM python as builder
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /app

RUN apt update -y && apt install -y git cmake g++ libboost-dev \
    libboost-context-dev libboost-stacktrace-dev nlohmann-json3-dev
RUN pip install meson ninja

COPY subprojects/simgrid.wrap subprojects/simgrid.wrap
COPY meson.build meson.build
COPY src src

RUN meson subprojects download
RUN meson setup build
RUN cd build && meson compile

FROM ubuntu:22.04

WORKDIR /app

COPY --from=builder /app/build /app

RUN apt update -y && apt install -y libboost-dev \ 
    libboost-context-dev libboost-stacktrace-dev nlohmann-json3-dev
