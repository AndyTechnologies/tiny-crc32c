#pragma once
#include <cstdint>

namespace tcrc32{

inline constexpr uint32_t __crc32_impl(
	const uint8_t* data, 
	std::size_t length, 
	uint32_t init = 0
);

// Overload for container with data() and size()
template <typename Container>
concept STLContainer = requires(Container a) { a.data(); a.size(); };

template<STLContainer Container>
inline constexpr uint32_t crc32(const Container& c, uint32_t init = 0) {
	return __crc32_impl(reinterpret_cast<const uint8_t*>(c.data()), c.size(), init);
}

// Incremental CRC32C
class CRC32C {
public:
    constexpr CRC32C(uint32_t init = 0);

    inline constexpr void update(const uint8_t* data, std::size_t length);

    template<STLContainer Container>
    inline constexpr void update(const Container& c) {
        update(reinterpret_cast<const uint8_t*>(c.data()), c.size());
    }

    [[nodiscard]] inline constexpr uint32_t digest() const noexcept;

private:
    uint32_t crc_;
};

}

#ifdef _IMPL_TINY_CRC32_

#include <memory>
#include <array>

namespace tcrc32{
	namespace _inernals{
		static constexpr const std::uint32_t POLY{  0x82f63b78u };

		static consteval std::array<uint32_t, 256> compute() {
			std::array<uint32_t, 256> table{};
			for (uint32_t i = 0; i < 256; ++i) {
				uint32_t crc = i;
				for (int j = 0; j < 8; ++j) {
					if (crc & 1u)
						crc = (crc >> 1) ^ POLY;
					else
						crc >>= 1;
				}
				table[i] = crc;
			}
			return table;
		}
	}

	static constexpr auto CRC_TABLE{ _inernals::compute() };

	inline constexpr uint32_t __crc32_impl(
		const uint8_t* data, 
		std::size_t length, 
		uint32_t init = 0
	) {
		uint32_t crc = ~init;
		for (std::size_t i = 0; i < length; ++i) {
			uint8_t byte = data[i];
			crc = CRC_TABLE[(crc ^ byte) & 0xFFu] ^ (crc >> 8);
		}
		return ~crc;
	}

	CRC32C::CRC32C(uint32_t init = 0): crc_(~init){}
	
	inline constexpr void CRC32C::update(const uint8_t* data, std::size_t length){
		crc_ = __crc32_impl(data, length, crc_);
	}

	inline constexpr uint32_t CRC32C::digest() const noexcept { return ~crc_; }


}
#endif
