## Some game engine

### Current result():

![output example](assets/example_gif0.gif "output example")
![output example](assets/example_gif1.gif "output example")
![output example](assets/example_gif2.gif "output example")
![output example](assets/example_gif3.gif "output example")

### There is a lot of functional by now:
- [X] 3D/2D modes, some editor
- [X] Models loading
- [X] Some shaders for lighting
- [X] Some particle system
- [X] Sprite Animations
- [X] Entity Component System (we trying not to use virtual functions for it(But it wastes a lot of space, but its better than jumping to virtuall function every frame for every object. Although we didnt benchmark it. Maybe CPU will optimize one of approaches better than another, will see{))
- [X] Our custom rigidbody physics(probably well have to utilise physX or bullet)
- [X] Custom collider detection(GJK and stuff)
- [X] Aility to write scripts in c++, and load scripts as a library into editor project

### Future milestones():
- [ ] Scene serialization(via simdjson) 
- [ ] Skeletal Animations (broken for now)
- [ ] Better editor capabilities
- [ ] Better rendering
- [ ] Better everything

