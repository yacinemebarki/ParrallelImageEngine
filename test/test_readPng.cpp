#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../include/image.hpp"

TEST(PngReaderTest, ReadsBigEndian32BitInteger) {
    std::vector<uint8_t> data = {
        0x00, 0x00, 0x02, 0x38
    };

    EXPECT_EQ(readBigEndian32(data, 0), 568);
}

TEST(PngReaderTest, ReadsZero) {
    std::vector<uint8_t> data = {
        0x00, 0x00, 0x00, 0x00
    };

    EXPECT_EQ(readBigEndian32(data, 0), 0);
}

TEST(PngReaderTest, RecognizesValidPngSignature) {
    std::vector<uint8_t> data = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
    };

    EXPECT_EQ(checkType(data), ImageType::PNG);
}

TEST(PngReaderTest, RejectsInvalidPngSignature) {
    std::vector<uint8_t> invalidData = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_THROW(checkType(invalidData), std::runtime_error);
}
#include <gtest/gtest.h>
#include "image.hpp"

TEST(ImageTest, CanCreateImageObject) {
    Image image("main/image.png");

    EXPECT_FALSE(image.data.empty());
}

TEST(PngReaderTest, StoresChunk) {
    Image image("main/image.png"); 

    PngChunk chunk;
    chunk.length = 0;
    chunk.type = "IEND";
    chunk.crc = 0;

    image.addChunk(chunk);

    auto chunks = image.getChunks();

    ASSERT_EQ(chunks.size(), 1);
    EXPECT_EQ(chunks[0].type, "IEND");
    EXPECT_EQ(chunks[0].length, 0);
}
TEST(ImageTest, ParsesPngChunks) {
    Image image("main/image.png");

    image.toMatrix();

    auto chunks = image.getChunks();

    ASSERT_FALSE(chunks.empty());
    EXPECT_EQ(chunks.front().type, "IHDR");
    EXPECT_EQ(chunks.back().type, "IEND");
}

TEST(PngReaderTest, ExtractsAndConcatenatesIDAT) {
    Image image("main/image.png");

    PngChunk chunk1;
    chunk1.type = "IDAT";
    chunk1.data = {1, 2, 3};

    PngChunk chunk2;
    chunk2.type = "IDAT";
    chunk2.data = {4, 5, 6};

    image.addChunk(chunk1);
    image.addChunk(chunk2);

    std::vector<uint8_t> expected = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(image.extractIdat(), expected);
}

TEST(ZlibHeaderTest, ValidHeader) {
    std::vector<uint8_t> data = {
        0x78, 0x9C, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_NO_THROW(inspectZlibHeader(data));
}

TEST(ZlibHeaderTest, InvalidDataSize) {
    std::vector<uint8_t> data = {0x78, 0x9C};

    EXPECT_THROW(
        inspectZlibHeader(data),
        std::runtime_error
    );
}

TEST(ZlibHeaderTest, InvalidCompressionMethod) {
    std::vector<uint8_t> data = {
        0x79, 0x9C, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_THROW(
        inspectZlibHeader(data),
        std::runtime_error
    );
}

TEST(ZlibHeaderTest, InvalidFCHECK) {
    std::vector<uint8_t> data = {
        0x78, 0x9D, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_THROW(
        inspectZlibHeader(data),
        std::runtime_error
    );
}

TEST(ZlibHeaderTest, PresetDictionaryNotSupported) {
    std::vector<uint8_t> data = {
        0x78, 0x20, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_THROW(
        inspectZlibHeader(data),
        std::runtime_error
    );
}

TEST(ZlibHeaderTest, InvalidWindowSize) {
    std::vector<uint8_t> data = {
        0x88, 0x1C, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_THROW(
        inspectZlibHeader(data),
        std::runtime_error
    );
}

TEST(DeflateTest, StoredBlock) {
    std::vector<uint8_t> data = {0x01};

    DeflateBlock block = inspectDeflateBlock(data);

    EXPECT_TRUE(block.isFinal);
    EXPECT_EQ(block.type, DeflateBlockType::Stored);
}

TEST(DeflateTest, FixedHuffmanBlock) {
    std::vector<uint8_t> data = {0x03};

    DeflateBlock block = inspectDeflateBlock(data);

    EXPECT_TRUE(block.isFinal);
    EXPECT_EQ(block.type, DeflateBlockType::FixedHuffman);
}

TEST(DeflateTest, DynamicHuffmanBlock) {
    std::vector<uint8_t> data = {0x05};

    DeflateBlock block = inspectDeflateBlock(data);

    EXPECT_TRUE(block.isFinal);
    EXPECT_EQ(block.type, DeflateBlockType::DynamicHuffman);
}

TEST(DeflateTest, ReservedBlock) {
    std::vector<uint8_t> data = {0x07};

    EXPECT_THROW(
        inspectDeflateBlock(data),
        std::runtime_error
    );
}

TEST(DeflateTest, EmptyData) {
    std::vector<uint8_t> data;

    EXPECT_THROW(
        inspectDeflateBlock(data),
        std::runtime_error
    );
}
