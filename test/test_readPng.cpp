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
