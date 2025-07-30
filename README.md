
# tiny-crc32c

**tiny-crc32c** es una librería header-only en C++23 para el cálculo de CRC32C (polinomio Castagnoli 0x1EDC6F41 reflejado: 0x82F63B78), sin dependencias externas.

## Características

* **Tabla en tiempo de compilación**: `consteval` genera la tabla de 256 entradas en compilación.
* **Interfaz Raw**: función `tcrc32::crc32(const uint8_t* data, size_t length, uint32_t init = 0)` para uso de bajo nivel.
* **STL-friendly**: overload para contenedores con `data()` y `size()`, como `std::vector`, `std::string`, `std::array`.
* **Modo incremental**: clase `tcrc32::CRC32C` con métodos `update()` y `digest()`.
* **`constexpr`**: permite cálculo en tiempo de compilación.
* **C++23**: concepts para validaciones internas.

## Instalación

```bash
git clone https://github.com/AndyTechnologies/tiny-crc32c.git
cd tiny-crc32c
mkdir build && cd build
cmake ..
cmake --build .
```

Tu proyecto solo necesita incluir:

```cpp
#define _IMPL_TINY_CRC32_
#include "tiny_crc32c.hpp"
```

> **Nota**: Debes definir el macro `_IMPL_TINY_CRC32_` en un único archivo fuente antes de incluir el header para generar las definiciones de las funciones en tiempo de compilación. En otros archivos solo incluye el header sin el define.

## Uso

### Cálculo en una sola pasada

```cpp
#include "tiny_crc32c.hpp"
#include <iostream>

int main() {
    const uint8_t data[] = {'H','o','l','a'};
    uint32_t crc = tcrc32::crc32(data, sizeof(data));
    std::cout << std::hex << crc;
}
```

### Sobrecarga STL

```cpp
std::vector<uint8_t> v = {...};
uint32_t crc = tcrc32::crc32(v);
```

### Cálculo incremental

```cpp
#include "tiny_crc32c.hpp"

tcrc32::CRC32C ctx;
ctx.update(data, len1);
ctx.update(data2, len2);
uint32_t crc = ctx.digest();
```

### `constexpr` en tiempo de compilación

```cpp
constexpr char s[] = "compile-time";
constexpr auto a = []{
    std::array<uint8_t, sizeof(s)-1> arr{};
    for (size_t i = 0; i < arr.size(); ++i) arr[i] = static_cast<uint8_t>(s[i]);
    return arr;
}();
static_assert(tcrc32::crc32(a.data(), a.size()) == 0x428A4F9Du);
```

## Tests

Se incluye suite de tests con **doctest**:

```bash
cd build
tests --output-on-failure
```

Cobertura de tests:

* Vectores conocidos (RFC, ASCII, UTF-8, frases comunes).
* Consistencia raw vs incremental.
* Buffers aleatorios y grandes.
* Casos límite (longitudes cero, valor inicial no nulo).
* Validaciones internas de la tabla.

## Contribuciones

PRs bienvenidas. Mantener estilo header-only, sin deps externas.

## Licencia

MIT
