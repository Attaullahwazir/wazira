FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake libleveldb-dev libssl-dev libcurl4-openssl-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY crawler/ /app/
COPY indexer/ /app/indexer/
RUN rm -rf /app/build /app/crawler/build /app/crawler/crawler/build /app/crawler/cli/build
RUN rm -rf /app/CMakeFiles /app/crawler/CMakeFiles /app/crawler/crawler/CMakeFiles /app/crawler/cli/CMakeFiles
RUN rm -f /app/Makefile /app/crawler/Makefile /app/crawler/crawler/Makefile /app/crawler/cli/Makefile
RUN cmake -S . -B build && cmake --build build
CMD ["./cli/main"] 