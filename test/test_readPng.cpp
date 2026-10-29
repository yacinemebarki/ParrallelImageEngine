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
