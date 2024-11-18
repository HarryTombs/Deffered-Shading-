# Programming project
*Harry Ben Brass Tombs*

## Realtime Global illumination
This is the project i've decided on

The one suggested in the sample projects. This is me not trying to reinvent the wheel here. I get lighting systems and i know how they work already ,well enough to know (vaugely) how to approach this project, so doing a path tracer or something along those lines would interested me greatly. **I THINK IT WILL STILL BE VERY HARD** but i think its a more possible goal to achieve. 

The majority of papers i've found and talks i've listened to are from nvidia with pretty useful complete github pages of their RTXGI (global illumination) and RTXDI (dynamic illumination) SDKs. Which i will obviously be a great help for me to look at more direct uses of C++ implementation

The main issue i've found for me as an individual is whether i can acheive this in real time rendering because im not an Nvida engineer

So as a backup for a technique that i know is real time but unfortunatly a vastly different approach is using light field probes
light field probes work by having light "sensors" place across the scene which bake the lighting information and apply it to nearby objects based on proximity
They are exceptionally fast to run but don't provide the most accurate lighting setup, as poor placement can lead to light or shadows leaking into incorrect places

it depends whether or not real time rendering is a priority or im happy enough with an *almost* real time render (if i've done a bunch of work on a ray tracer im not gonna bin it because its not real time)


## How do ray tracing algorithms work?

Real life light works by light coming from a source and bouncing into your eye, a room can be lit without you being there to see it since real life doesn't require processing power
Ray tracing works in the opposite way, A camera functioning as an eye shoots rays into a space and bounces off an object a specified number of times or until it finds a light source
The colour of that pixel is calculated by the informaiton gathered by those bounces including nearby light sources, reflective objects or lack there of.

**To put it in terms of an algorithmic design**


In a 3D lit scene with a camera

Take the resolution of the screen

for each pixel detailed in the camera shoot a ray throught the centre of the pixel

if it hits an object (otherwise render nothing) find object normal and material information and bounce off at X angle (depending on surfaces and normals) for Y number of bounces (defined in the render settings) or until it returns to a light source

based on the information gathered from the secondary bounces (light reflected from nearby object or dedicated light sources or nothing) create an output colour corresponding to the pixel 


Thats the current plan *God help us all*

## Sources

I will cite these correctly later please excuse my poor referincing system for now

### Ray tracing 
https://en.wikipedia.org/wiki/Ray_tracing_(graphics)

https://jcgt.org/published/0008/02/01/

https://www.sci.utah.edu/~wald/PhD/wald_phd.pdf

https://github.com/NVIDIAGameWorks/RTXGI

https://developer.nvidia.com/rtx/ray-tracing/rtxgi

https://github.com/NVIDIAGameWorks/RTXDI

https://resources.nvidia.com/en-us-game-dev-rtx/gtcspring21-s32639

https://alain.xyz/blog/ray-tracing-denoising

https://bth.diva-portal.org/smash/get/diva2:1440210/FULLTEXT02.pdf

https://developer.download.nvidia.com/video/gputechconf/gtc/2019/presentation/s9985-exploring-ray-traced-future-in-metro-exodus.pdf

https://cs.dartmouth.edu/~wjarosz/publications/mara17towards.html

https://gdcvault.com/play/1026722/RTXGI-Scalable-Ray-Traced-Global

https://gdcvault.com/play/1026721/RTX-Ray-Tracing-Best-Practices

https://www.gdcvault.com/play/1024353/

https://in1weekend.blogspot.com/2016/01/ray-tracing-in-one-weekend.html


### Light probes

https://docs.unity3d.com/Manual/LightProbes.html

https://casual-effects.com/research/McGuire2017LightField/index.html

https://casual-effects.com/research/McGuire2017LightField/McGuire2017LightField-GDCSlides.pdf

https://dl.acm.org/doi/abs/10.1145/3023368.3023378

https://github.com/Global-Illuminati/Precomputed-Light-Field-Probes


### Other

https://ieeexplore.ieee.org/abstract/document/9527241

https://www.khronos.org/opengl/wiki/Framebuffer

https://learnopengl.com/

https://graphicscodex.com/app/app.html

https://www.khronos.org/opengl/wiki/





[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/RM1pL2Qm)
