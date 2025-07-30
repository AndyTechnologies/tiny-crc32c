#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#define _IMPL_TINY_CRC32_
#include "../include/tiny_crc32.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <random>
#include <cstdint>
#include <cstring>

// Vectores de prueba conocidos
struct TestVector {
    std::string_view input;
    uint32_t expected;
};

static constexpr TestVector vectors[] = {
    {"",            0x00000000u}, // Empty
    {"123456789",   0xE3069283u}, // RFC CRC32C test
    {"hola",        0x688FD52Fu}, // ASCII lower
    {"¡ñáéíóú!",    0xFF3F8800u}, // UTF-8 multibyte
    {"The quick brown fox jumps over the lazy dog", 0x22620404u},
    {std::string_view("Lorem ipsum dolor sit amet, consectetur adipiscing elit."), 0x6C7D6ADF},
};


TEST_CASE("CRC32C raw interface vs test vectors") {
    for (auto const& tv : vectors) {
        uint32_t crc = tcrc32::crc32(
            reinterpret_cast<const uint8_t*>(tv.input.data()), tv.input.size());
        INFO("input='" << tv.input << "', got=0x" << std::hex << crc
             << ", expected=0x" << tv.expected);
        CHECK_EQ(crc, tv.expected);
    }
}

TEST_CASE("CRC32C STL interface") {
    // std::vector
    std::vector<uint8_t> vec(vectors[1].input.begin(), vectors[1].input.end());
    CHECK_EQ(tcrc32::crc32(vec), vectors[1].expected);

    // std::string
    std::string str(vectors[2].input);
    CHECK_EQ(tcrc32::crc32(str), vectors[2].expected);

    // std::array
    std::array<uint8_t, 9> arr = {{'1','2','3','4','5','6','7','8','9'}};
    CHECK_EQ(tcrc32::crc32(arr, 0), 0xE3069283u);
}

TEST_CASE("CRC32C incremental == raw") {
    for (auto const& tv : vectors) {
        tcrc32::CRC32C ctx;
        size_t pos = 0;
        while (pos < tv.input.size()) {
            size_t chunk = 1 + (pos % 5);
            ctx.update(
                reinterpret_cast<const uint8_t*>(tv.input.data() + pos),
                std::min(chunk, tv.input.size() - pos)
            );
            pos += chunk;
        }
        uint32_t crc_inc = ctx.digest();
        uint32_t crc_raw = tcrc32::crc32(
            reinterpret_cast<const uint8_t*>(tv.input.data()), tv.input.size());
        INFO("input='" << tv.input << "', incremental=0x" << std::hex << crc_inc
             << ", raw=0x" << crc_raw);
        CHECK_EQ(crc_inc, crc_raw);
    }
}

TEST_CASE("CRC32C random buffers consistency") {
    std::mt19937_64 rng(0xDEADBEEFull);
    std::uniform_int_distribution<int> dist(0, 255);
    for (int trial = 0; trial < 100; ++trial) {
        size_t len = 1 + (rng() % 1024);
        std::vector<uint8_t> buf(len);
        for (size_t i = 0; i < len; ++i) buf[i] = static_cast<uint8_t>(dist(rng));

        uint32_t raw = tcrc32::crc32(buf);
        tcrc32::CRC32C ctx;
        size_t pos = 0;
        while (pos < len) {
            size_t chunk = 1 + (rng() % 64);
            ctx.update(buf.data() + pos, std::min(chunk, len - pos));
            pos += chunk;
        }
        uint32_t inc = ctx.digest();
        INFO("trial=" << trial << ", len=" << len << ", raw=0x" << std::hex << raw
             << ", inc=0x" << inc);
        CHECK_EQ(inc, raw);
    }
}

TEST_CASE("CRC32C large buffer sanity") {
    const size_t large_size = 1 << 20; // 1 MiB
    std::vector<uint8_t> buf(large_size, 0xA5);
    uint32_t raw = tcrc32::crc32(buf);
    tcrc32::CRC32C ctx;
    ctx.update(buf);
    uint32_t inc = ctx.digest();
    CHECK_EQ(inc, raw);
}

TEST_CASE("CRC32C zero buffer lengths") {
    // Empty buffer via pointer
    CHECK_EQ(tcrc32::crc32((const uint8_t*)nullptr, 0), 0x00000000u);
    // Zero-length container
    std::vector<uint8_t> empty;
    CHECK_EQ(tcrc32::crc32(empty), 0x00000000u);
}

TEST_CASE("CRC32C non-zero initial value") {
    std::string data = "abcdef";
    uint32_t init = 0x12345678u;
    uint32_t crc1 = tcrc32::crc32(
        reinterpret_cast<const uint8_t*>(data.data()), data.size(), init);
    // Compute manually: first pass then as incremental with init
    tcrc32::CRC32C ctx(init);
    ctx.update(data);
    uint32_t crc2 = ctx.digest();
    CHECK_EQ(crc2, crc1);
    INFO("init=0x" << std::hex << init << ", crc=0x" << crc1);
}

TEST_CASE("CRC32C constexpr table sample values") {
    // Check first and last entries of CRC table
    constexpr uint32_t first = tcrc32::_inernals::CRC_TABLE[0];
    constexpr uint32_t last = tcrc32::_inernals::CRC_TABLE[255];
    static_assert(first == 0x00000000u, "CRC_TABLE[0] must be 0");
    static_assert(last  == 0xAD7D5351, "CRC_TABLE[255] mismatch");
    CHECK(first == 0x00000000u);
    CHECK(last  == 0xAD7D5351);
}

// Test constexpr at compile time
constexpr auto make_ct() {
    constexpr char s[] = "compile-time";
    std::array<uint8_t, sizeof(s)-1> a{};
    for (size_t i = 0; i < a.size(); ++i)
        a[i] = static_cast<uint8_t>(s[i]);
    return a;
}
static constexpr auto ct = make_ct();
static_assert(
    tcrc32::crc32(ct.data(), ct.size()) == 0x428A4F9Du,
    "CRC32C constexpr failure");

