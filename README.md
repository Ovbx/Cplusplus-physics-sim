# C++ physics-sim
A real-time 2D physics engine written from scratch in C++ with SFML 3, ported from an earlier Python/pygame version. Balls fall under gravity, bounce off walls, and (in progress) collide with each other using a quadtree for efficient broad-phase detection.

## Built with
- C++ (C++20)
- SFML (window, graphics, system)
- MSYS2 / MinGW toolchain

## Building
Requires SFML 3 installed via MSYS2 (pacman -S mingw-w64-ucrt-x86_64-sfml).
```
g++ -std=c++20 main.cpp -o output/main.exe -lsfml-graphics -lsfml-window -lsfml-system
./output/main.exe
```

## Roadmap

### Done
- [x] SFML 3 window rendering with a working build (MSYS2 + linking)
- [x] Ball class
- [x] Circle drawing
- [x] Rescaling border
- [x] Position updating w/substepping
- [x] Fix energy generation bug
- [x] fix alternating energy while "resting"
- [x] Mouse click to select
### In prog.
- [ ] Ball collisions
- [ ] Mass-weighted elastic collisions response
- [ ] Quadtree optimization
- [ ] Add ball button/key
- [ ] Customize colors
- [ ] Polish visuals

## Notes
Translated from python/pygame to C++, uses the essentially the same design. C++ version adds manual memory manipulation/awareness. Built primarily to learn C++ through context and as a learning project towards embedded/systems work.
