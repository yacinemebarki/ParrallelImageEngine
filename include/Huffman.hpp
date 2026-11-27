#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class Huffman {
public:
    struct Code {
        uint16_t symbol;
        uint16_t code;
        uint8_t length;
    };

    virtual ~Huffman() = default;

    virtual void generateLiteral() = 0;
    void generateLiteralCode();

    const std::vector<uint8_t>& getLiteralLength() const;
    const std::vector<uint16_t>& getCount() const;
    const std::vector<Code>& getLiteralCode() const;

protected:
    explicit Huffman(std::size_t literalCount);

    std::vector<uint8_t> literalLength;
    std::vector<uint16_t> count;
    std::vector<Code> literalCode;
};
