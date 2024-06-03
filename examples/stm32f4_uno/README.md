# Create Application Skeleton



## Use board config

You can find some examples in `Board Config Files`



## Add git_rev_macro.py

Add the following at the end of the file `CMakeLists_template.txt`:

```cmake
include(git_rev_macro.cmake)
```



## Add `Application` directory

### In `CMakeLists_template.txt`

Add `Application` to `include_directories`

Add `"Application/*.*"` to `file`



### In `Core/Src/main.c`

Add the following to `USER CODE BEGIN Includes`:

```c
#include "defines.h"
#include "globals.h"
#include "main.hpp"
```

Add the following to `USER CODE BEGIN 2`:

```c
  // Jump to our C++ setup function
  setup();
```

Add the following to `USER CODE BEGIN Error_Handler_Debug`:

```c
  // Jump to our C++ setup function
  errorHandler();
```



### In `Core/Src/app_threadx.c`

Add the following to `USER CODE BEGIN Includes`:

```c
#include "main.h"
#include "setupMainThread.hpp"
```

Add the following to `USER CODE BEGIN App_ThreadX_Init`:

```c
  // Jump to our C++ thread setup function
  ret = setupMainThread(byte_pool);
  assert_param(ret == TX_SUCCESS);
```



## E100 Flash inspector

E100 operates on the NOR flash device directly. A sector is what the manufacturer of the flash device defines as a sector (4096 bytes).

### Dump flash memory

```<
E100 Cdump A<address> [S<size>]
```

Print  `size` Bytes the contents starting at `address`.  If `size` is not given, 512 bytes are printed.

### Check if sector is erased

```
E100 Ccheck A<address>
```

Checks if the sector at `address` is empty. `address` is automatically adjusted to a sector start. 0=sector is not empty, 1=sector is empty

### Erase sector

```
E100 Cerase A<address>
```

Erase the sector at `address`. `address` must point exactly to a sector start.

### Erase chip

```
E100 Cchiperase
```

Erase the whole NOR flash chip. No questions asked.



## E101 LevelX

https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md

### Initialize

```
E101 Cinit
```

Initialize NOR flash support

### Open

```
E101 Copen
```

Open NOR flash instance

### Close

```
E101 Cclose
```

Close NOR flash instance

### Read Sector

```
E101 Cread A<logical_sector>
```

Read and print `logical_sector`.

### Release Sector

```
E101 Crel A<logical_sector>
```

Release `logical_sector`.

### Write Sector

```
E101 Cwrite A<logical_sector> W<text>
```

Write `text` into `logical_sector`.



