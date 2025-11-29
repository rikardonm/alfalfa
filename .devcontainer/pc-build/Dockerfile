FROM gcc:15-trixie

ARG DEBIAN_FRONTEND=nointeractive

# Sadly, we are running Debian
#RUN apt-get update \
#   && apt install -y -q \
#   software-properties -common \
#   && rm -rf /var/lib/apt/lists/*
# RUN add-apt-repository ppa:deadsnakes/ppa

RUN apt-get update \
   && apt install -y -q \
   cmake \
   python3.11-full \
   gdb \
   cppcheck \
   && rm -rf /var/lib/apt/lists/*

#RUN apt install -y clang
#RUN apt install -y clang cppcheck

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

ENTRYPOINT [ "/usr/bin/bash" ]

CMD [""]
