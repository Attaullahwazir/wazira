# Indexer Module

This module implements the high-performance, privacy-preserving indexer for the Wazira search engine.

## Features
- Tokenization, stemming, and inverted index (LevelDB)
- Merkle diffing for incremental updates
- Microsoft SEAL integration for encrypted queries (CKKS)
- pybind11 bindings for Python integration

## Build Requirements
- **C++17**
- **LevelDB**
- **Microsoft SEAL** (https://github.com/microsoft/SEAL)
- **pybind11** (https://github.com/pybind/pybind11)

## Building the SEAL Wrapper and Python Bindings

1. **Install Microsoft SEAL and pybind11:**
   - Follow instructions on their GitHub repos to build/install system-wide or as submodules.

2. **Build the C++ wrapper and Python module:**
   - Add the following to your `CMakeLists.txt`:
     ```cmake
     find_package(SEAL REQUIRED)
     find_package(pybind11 REQUIRED)
     add_library(seal_wrapper SHARED seal_wrapper.cpp)
     target_link_libraries(seal_wrapper SEAL::seal)
     pybind11_add_module(seal_wrapper bindings.cpp)
     target_link_libraries(seal_wrapper PRIVATE SEAL::seal)
     ```
   - Build with CMake:
     ```sh
     mkdir build && cd build
     cmake ..
     make
     ```
   - This will produce a `seal_wrapper.*.so` Python module.

3. **Test the Python bindings:**
   ```python
   import seal_wrapper
   seal_wrapper.seal_init()
   ct = seal_wrapper.encrypt_vector([1.0, 2.0, 3.0])
   pt = seal_wrapper.decrypt_vector(ct)
   print("Decrypted:", pt)
   dot = seal_wrapper.encrypted_dot(ct, ct)
   print("Encrypted dot product:", dot)
   ```

## TODO
- Implement real SEAL logic in `seal_wrapper.cpp`
- Integrate with FastAPI service for encrypted query processing
- Add end-to-end tests

## Purpose
The indexer processes crawled web content to build a fast, searchable inverted index, enabling efficient query processing.

## Key Responsibilities
- Tokenizing and normalizing web page content
- Stemming and removing stop words
- Building and updating the inverted index
- Supporting efficient word-to-document lookups
- Handling incremental updates via Merkle diffs

## Planned Technologies
- **Language:** C++
- **Libraries:** LevelDB (index storage), Snowball (stemming)

## Planned Code Structure
- `tokenizer/`: Text normalization and tokenization
- `stemmer/`: Word stemming
- `inverted_index/`: Index data structures and storage
- `merkle_diff/`: Incremental update logic
- `tests/`: Unit and integration tests

## Logic Overview
1. Receive and parse crawled content
2. Tokenize and normalize text
3. Stem words and remove stop words
4. Update the inverted index in LevelDB
5. Apply incremental updates using Merkle diffs

## Next Steps
- Implement module skeletons and interfaces
- Add unit and integration tests

## Real-Time Indexing Pipeline
- Integrate Kafka or RabbitMQ to receive crawl events and trigger indexing in real time.
- Update the indexer to consume messages and index new documents as they arrive.
- Monitor indexing freshness and throughput.
- (Planned) Add a dashboard for real-time monitoring and event tracing. 