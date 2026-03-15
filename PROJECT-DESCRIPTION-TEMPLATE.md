# Police driving simulator

MI 295/2021 - NEMANJA BADRIC -
Police driving simulator

## Controls

'WSAD' → Camera and car movement

'Q' and 'E' → Camera vertical ascending and descending

Right and Left Arrow → Rotate camera around car while driving mode is active.

'F2' key → Activate GUI

'F1' key → Driving mode

'G' key while driving mode is active → Police Emergency lights

'F' key while driving mode is active → Police Head lights

'KEY 1' → Night Vision mode

'KEY 2' → Greyscale Vision mode

UFO LANDING SEQUENCE (Activate in GUI) → after 1 second, UFO starts descending and lands near campfire,
2 seconds after landing, street-lights and police-headlights start flickering.

## Features

### Fundamental:

[X ] Model with lighting
[X ] Two types of lighting with customizable colors and movement through GUI or ACTIONS
[X ] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[X] Frame-buffers with post-processing   
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping
[ ] Bloom with the use of HDR

### Group B:

[ ] Deferred Shading  
[X] Point Shadows  
[ ] SSAO

### Engine improvement:

Framebuffer Post-Processing Architecture: Integrated a modular framebuffer system
to support full-screen effects. Implemented specific shaders for Night Vision and
Grayscale modes using post-processing pipelines.

Dynamic Point Shadow System: Developed a point light shadow mapping component.
Used omnidirectional shadow maps (Cube Maps) to achieve realistic light-shadow interaction,
specifically demonstrated with the animated campfire.

## Models:

Bakery - https://poly.pizza/m/6BGhNQlUzRR,
Bank - https://poly.pizza/m/2gYeniBQ8RH,
Building - https://poly.pizza/m/01lqee-dZAr,
Cactus - https://poly.pizza/m/1_3Ur2-LAs8,
Campfire - https://poly.pizza/m/0vzzmM-t8CP,
Caravan - https://poly.pizza/m/77t77GQMubf,
Cinema - https://poly.pizza/m/c-5balfj4bu,
Convertible - https://poly.pizza/m/dggOiBLYyuR,
Farm house - https://poly.pizza/m/bHyQe5jzdiQ,
House - https://poly.pizza/m/bnZkUs4qEdG,
Lawn mower - https://poly.pizza/m/1pdSPagFCub,
Police car - https://poly.pizza/m/0-j0ksmXXtz,
Road - https://poly.pizza/m/a38EEOJFUlp,
Saloon - https://poly.pizza/m/82JBTSsWag4,
Tennis court - https://poly.pizza/m/7cVH1Xt-LYy,
UFO - https://poly.pizza/m/b0N_HeT_Ttb,
Wood swing - https://poly.pizza/m/3mFM6lFd9P9,
Street light - https://www.turbosquid.com/3d-models/street-light-2386960,
Skybox - https://opengameart.org/content/winter-skyboxes

## Textures

[link1]  
[link2]

{OTHER-RESOURCES}
