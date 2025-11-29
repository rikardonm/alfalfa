# alfalfa

[![PyPI - Version](https://img.shields.io/pypi/v/alfalfa.svg)](https://pypi.org/project/alfalfa)
[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/alfalfa.svg)](https://pypi.org/project/alfalfa)


This is a library for embedded systems (and maybe more).

-----

## Table of Contents

- [Installation](#installation)
- [License](#license)

## Installation

```console
pip install alfalfa
```

## Tree Structure


```mermaid
mindmap
    root(Alfalfa)
        Hardware Abstraction Layer (hal)
            Internal Peripherals
        Board Abstraction Layer (bal)
            Devices & Buses
        Operating System Abstraction Layer (oal)
            Implementations
                ESP32 IDF
                Linux
            Timing Control
```



## License

`alfalfa` is distributed under the terms of the [MIT](https://spdx.org/licenses/MIT.html) license.
