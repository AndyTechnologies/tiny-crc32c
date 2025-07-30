
#include "../include/tiny_crc32.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <print>
#include <functional>

static constexpr size_t N = 1024 * 1024; // 1mb = 1024kb = 1024 * 1024;
static constexpr size_t iterations = 1024;

void test_this(const std::array<uint8_t, N>& data, std::function<void(const std::array<uint8_t, N>&)> callback, std::string name){
	auto begin = std::chrono::high_resolution_clock::now();
		callback(data);
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - begin;

	std::println("{} API:\n\tTotal: {} ms\n\tPerformance: {} ms/iter", name, elapsed.count(), (elapsed.count() / iterations));
}

void test_raw(const std::array<uint8_t, N>& data){
    uint32_t crc = 0;
    for (size_t i = 0; i < iterations; ++i) {
        crc = tcrc32::_internals::__crc32_impl(data.data(), data.size());
    }
}

void test_STL(const std::array<uint8_t, N>& data){
    uint32_t crc = 0;
    for (size_t i = 0; i < iterations; ++i) {
        crc = tcrc32::crc32(data);
    }
}

void test_Incremental(const std::array<uint8_t, N>& data){
	tcrc32::CRC32C crc{};
    for (size_t i = 0; i < iterations; ++i) {
        crc.update(data);
    }
}

int main() {
	std::println("Benchmark CRC32C (Castagnoli polynomial)");
	std::println("---------------------------------------");

    std::array<uint8_t, N> data{0};

	std::println("Data size: {} MB", (double(N) / 1024) / 1024.0);
	std::println("Iterations: {}", iterations);

    auto begin = std::chrono::high_resolution_clock::now();
	
	test_this(data, test_raw, "Raw");
	test_this(data, test_STL, "STL");
	test_this(data, test_Incremental, "Incremental");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - begin;

	std::println("\n- Total Time: {} ms ({} ms/test)", elapsed.count(), elapsed.count() / 3);
    return 0;
}