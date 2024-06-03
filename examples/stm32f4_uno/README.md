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



## Flash inspector

### Dump flash memory

```
E100 Cdump A0 [S100]
```



### Check if sector is erased

```
E100 Ccheck [A0]
```



### Erase sector

```
E100 Cerase A0
```



### Erase chip

```
E100 Cchiperase
```



