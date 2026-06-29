# C++ physics-sim
A real-time 2D physics engine written from scratch in C++ with SFML 3, ported from an earlier Python/pygame version. Balls fall under gravity, bounce off walls, and collide with each other using a quadtree for efficient broad-phase detection.

<img width="996" height="622" alt="cppphysicsdemo2" src="https://github.com/user-attachments/assets/58507e7b-f989-4316-a184-dbef64b7f338" />

## Controls
- Left-click + drag: grab and throw ball
- Scroll wheel: spawn a new ball at cursor
- Resize window: resize box
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
- [x] Quad Tree optimization
- [x] Ball collisions
- [x] Mass-weighted elastic collisions response
- [x] Velocity-based color (blue->red)
- [x] Spawn ball w/scroll wheel
- [x] Ceiling check
### In prog.
- [ ] Polish visuals
- [ ] Fix dense collision by implementing a sequential impulse solver
- [ ] Introduce rigid ball sleeping threshold to prevent jitter while resting
## Known limitations
- Resting balls jitter slightly (solver iterations)
- Dense piles causes waves (needs sleeping to fix)
- Query assumes uniform radius (not sure as of this moment)
- Stacked towers causes launches and gaps in between balls
  <img width="550" height="358" alt="cppissue1" src="https://github.com/user-attachments/assets/ef8f323a-07ac-4d06-9b78-4ef692063752" />

## Notes
Translated from python/pygame to C++, uses the essentially the same design. C++ version adds manual memory manipulation/awareness. Built primarily to introduce/learn C++ through context and as a learning project towards embedded/systems work.
