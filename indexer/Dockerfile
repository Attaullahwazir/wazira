FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake libssl-dev nlohmann-json3-dev libleveldb-dev libcurl4-openssl-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN rm -rf /app/build
RUN cmake -S . -B build && cmake --build build
CMD ["./search_service"] 