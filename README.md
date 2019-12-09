# Microsila_LL - a simple C library for programming STM32 Blue Pill board

## Notice
This is a draft. There is no guarantee of the project development. It can be refactored or deleted at any time.

## Goal
The goal is to simplify programming of one of the most popular STM32 boards - the STM32F103C8T6 Blue Pill board (and possibly other popular MCU boards in the future).
STM32 devices are feature-rich, but that abundance brings a problem: complex setup and programming. Because each feaure has plenty of settings, there is almost infinite number of combinations of those settings. STM developed tools like `STM CubeMX` to deal with the complexity. But `microsila_ll` approach is different: use common and well tested setting sets whenever possible; only if impossible, use the heavy artillery.
For example, using a widely spread crystal resonator with commonly used frequency (e.g. 8 MHz) in your project will simplify clock setup.
Similarly, preferring 8 data bits and 1 stop bit with no parity control for UART will reduce the number of combinations for UART setting.
Of course, you can go custom at any time and adjust almost everything to you need. But that will (significantly) increase  the programming and testing time.

## Structure
`microsila_ll` is a middleware based on STM Low-Level Drivers. That results in higher performance and fewer bugs in comparison with STM HAL library.

## Usage
You need to add two files into your project: `microsila_ll.c` and `mi_ll_setup.h`. The first one can be added directly from your `microsila_ll` folder, the latter must be copied into your project as it must be customized. Place it into a directory listed in the include path, e.g. `Inc` folder of Keil project.
Add path to `microsila_ll` directory to your include path.
Edit `mi_ll_setup.h` according to your needs. See examples and tests for more.
