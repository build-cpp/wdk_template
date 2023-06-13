# wdk_template

Simple [`cmkr`](https://cmkr.build) template for a WDK kernel driver for Windows. The `WDK` package is based on [FindWDK](https://github.com/SergiusTheBest/FindWDK).

## Requirements

- Visual Studio 2022
- [WDK 10](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)
- CMake 3.15+

## Building

```
cmake -B build
cmake --build build --config Release
```

You can open `build\MyDriver.sln` in Visual Studio and work there.
