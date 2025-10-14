FROM gcc:14

RUN mkdir /app && mkdir /results


COPY external/ /app/external
COPY tools/ /app/tools
COPY include/ /app/include
COPY CMakeLists.txt /app
COPY src/ /app/src


RUN apt install -y wget gpg

# Add Kitware GPG key
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list

# Update and install
RUN apt update && apt install -y cmake zlib1g-dev
# Copy source code
WORKDIR /app

RUN mkdir build

RUN cmake -S . -B build && cmake --build build


ENTRYPOINT ["./build/qAlgorithms"]
