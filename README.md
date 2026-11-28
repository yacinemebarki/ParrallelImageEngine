# Parallel Image Engine

A C++ image-processing project focused on PNG parsing, DEFLATE decompression, and parallel image execution experiments.

## Features

- Reads image files and validates PNG signatures
- Parses PNG chunks, including `IHDR`, `IDAT`, and `IEND`
- Extracts and concatenates data from one or more `IDAT` chunks
- Reads PNG metadata from the `IHDR` chunk
- Implements a LSB-first `BitReader`
- Implements fixed and dynamic Huffman tree structures
- Generates canonical Huffman codes
- Decodes individual Huffman symbols
- Decompresses DEFLATE stored and fixed-Huffman blocks
- Supports DEFLATE length/distance back-reference matches
- Includes GoogleTest coverage for PNG, DEFLATE, and Huffman behavior

## Project Structure

```text
include/        Public C++ headers
src/            PNG, DEFLATE, Huffman, and image-processing implementations
main/           Example executable and sample PNG
main/main.cpp   Demo entry point
test/           GoogleTest test suite
Makefile        Build and test commands
.github/        GitHub Actions CI configuration
```

## Requirements

- A C++17-compatible compiler
- `make`
- GoogleTest (`libgtest-dev` on Debian/Ubuntu)
- POSIX/Linux or a compatible environment

### Ubuntu/Debian Setup

```bash
sudo apt-get update
sudo apt-get install -y build-essential libgtest-dev
```

## Build

Build the example executable:

```bash
make
```

This creates `imageengine` in the project root.

Run the example:

```bash
make run
```

The example loads `main/image.png`, parses its PNG chunks, and prints information about the extracted `IDAT` data.

## Tests

Build and run the complete GoogleTest suite:

```bash
make test
```

You can run the test binary directly for filtered or verbose output:

```bash
./test_runner
./test_runner --gtest_filter=HuffmanTest.*
./test_runner --gtest_filter=DeflateTest.*
```

## Continuous Integration

GitHub Actions builds the project and runs the test suite on pushes and pull requests to `main`.

## Implementation Status

The project has working support for:

- PNG signature and chunk parsing
- Zlib header validation
- Stored DEFLATE blocks
- Fixed-Huffman DEFLATE blocks
- Canonical Huffman code generation
- Huffman symbol decoding
- Literal and back-reference decompression

Dynamic-Huffman block decompression is represented in the codebase but is not implemented yet.

## Example

```cpp
FixedHuffman literalTree(288);
FixedHuffman distanceTree(32);

literalTree.generateLiteral();
literalTree.generateLiteralCode();
distanceTree.generateLiteralCode();

BitReader reader(compressedData);
Deflate deflate(reader, literalTree, distanceTree);

std::vector<uint8_t> decompressed = deflate.decompress();
```

## License

No license has been specified for this repository yet.
