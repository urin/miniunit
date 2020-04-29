# miniunit.h

miniunit.h is a unit testing framework for pure-C. It features a simple interface for defining unit tests, putting little in the way of the developer.

- header-file only
- no external dependencies (only uses standard C libraries)

## Usage

Define `MINIUNIT_MAIN` before including `miniunit.h` on the main source file.

```c
#define MINIUNIT_MAIN
#include "miniunit.h"
```

Just include `miniunit.h` without defining `MINIUNIT_MAIN` on other source files.

```c
#include "miniunit.h"
```

### Example

```c
#define MINIUNIT_MAIN
#include "miniunit.h"

int main()
{
    test_case("Addition", {
        expect("1 + 1 = 2", (1 + 1) == 2);
        expect("1 + 1 + 1 = 3", (1 + 1 + 1) == 3);
    });

    test_case("Subtraction", {
        expect("1 - 1 = 0", (1 - 1) == 0);
        expect("1 - 1 - 1 = -1", (1 - 1 - 1) == -1);
    });

    return 0;
}
```

## License

[MIT](/LICENSE)

## Author

[urin](//github.com/urin)

