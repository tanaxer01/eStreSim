from ubuntu:22.04

workdir /app

RUN apt update -y && apt install -y build-essential pkg-config meson cmake g++

COPY . .

#RUN meson subproyects download
#RUN meson setup build
#RUN cd build && meson compile
