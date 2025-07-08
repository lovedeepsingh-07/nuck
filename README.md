# nuck

> beginner friendly, open-source physics library for 2D games

`nuck` is not a big, production-ready physics library that you can use to make 2D games(you have [Box2D](https://box2d.org/) for that), this was only meant to be the a simple custom physics engine for my platformer game, but while trying to learn how to implement it, I ran into 1 issue, there is a shit ton of stuff online about how the physics behind a physics engine works, but there is little to no resource on how the actual physics engine works. How to ensure performant storage and lookup of objects? How to optimize the physics engine? How to check for collisions for 100s of objects without causing performance issues? These were all the questions that took me(as a beginner) way too much time to find answers for.

But when I finally figured out how it all works, I decided that I would write an open-source physics engine along with a fully-detailed explaination/guide on how it works so that anyone can write it themselves. Ofcourse if you don't want to know all that and just want to use physics-engine itself, it is also super beginner friendly, even if you have little to no knowledge of physics.

I have written this engine in C++ but the idea is same for every single language and the architecture guide is written in a way that you can write your own physics-engine in any language you want.

# Quickstart

- Create a project with `CMakeLists.txt` file at the root.
- Simply clone the this repository into your own project (I prefer placing it in `external/nuck` folder and using "git-submodules")
- Add the following to your cmake file:
  ```cmake
  add_subdirectory(path/to/nuck)
  target_link_libraries(your_project_name PRIVATE nuck)
  ```
- Now you can just include it in your project like:
  ```cpp
  #include "nuck/nuck.hpp"
  ```
