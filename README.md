# Water-Simulation with Realtime Reflections/Refractions from the Surface
This project consists of several smaller projects joined together. Everything is done completely from
scratch. No external libraries or dependencies (except basic OpenGL) are needed to run this simulation.

The final result looks like that:

![Water-Simulation](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished3.png "Water-Simulation with reflections/refractions")

## **Water Simulation**

The water simulation itself is a pressure based height field.

The water's surface is divided into a small grid of points. Each height represents
the pressure at that point. For each time step the pressure is divided and continued on to
the next water point. The calculation itself is similar to image processing calculations,
where a kernel over each pixel calculates the color at that pixel.

A height field water simulation looks very real for smaller waves and distorted water surfaces.
But because of using a height-field (real time efficiency and simplicity to implement) there is no
possibility of calculating single drops or any kind of detachment of waterbodies from the surface.
Also no overlapping waves or such things are possible.

Example with random displacement added to the water's surface, creating very small waves (distortion):

![Water-Distortion](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished6.png "Small distortions")

Example with some small spreading waves:

![Spreading waves](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished7.png "Spreading waves")

Example with waves bouncing back from the shore:

![Bouncing waves](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished1.png "Waves bouncing from the shore")


## **Reflections / Refractions (Screenspace)**

Both the reflection and refraction of the water's surface is calculated in screen space with OpenGL fragment shaders.

The main benefit of using screenspace reflections/refractions is, that it takes a constant time to calculate and doesn't depend
on the complexity of the scene. Another benefit is, that both the reflection and refraction can be calculated using
the exact same calculations.

- For each frame, two textures (color-texture and depth-texture) are created from the point of view and provided to the
  OpenGL fragment shader.

- For each pixel, the position of each reflection/refraction-point is provided in world-coordinates to the fragment shader.

- A step-size is determined (could also be constant).

- For each step, the reflection-vector is scaled with the determinded step-size and transformed into screenspace.
  We now have two vectors. One is the reflection-vector in world coordinates and the same one transformed into screen space.

- Because we look at the same scene which is saved into the texture, we can now sample the depth-texture and get the depth
  of the scene at the point of the scaled reflection-vector.

- We continue the scaling of the reflection-vector until the depth of the sampled depth-texture is smaller than the
  z-coordinate (depth) of the reflection-vector in world coordinates. In this case we know, that we hit something which should
  be displayed as the reflection for the original fragment.

- We can then take the transformed reflection-vector and sample the color texture. The result should be the color of the
  reflection. We take that color and are finished.

Positive features: Works in real time looks good most of the time.

Negative features: Actually takes longer to calculate, if there are no reflections, is not always correct, not defined
for situations where the reflection goes outside the screenspace and can only reflect objects which are on the screen!
(Also there are incorrect reflections if there is an object between the camera and the water's surface.)

Water refraction:

![Refraction](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished2.png "Water-Refraction looking good!")

Reflection and refraction with no waves at all:

![No Wave-Reflection](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished5.png "Water-Reflection, no waves")

Refraction and Reflection of smaller waves from above:

![Small waves](https://github.com/MauriceGit/Water_Simulation/blob/master/Screenshots/finished4.png "Water-Refraction/Reflection with small waves")

## **Terrain Modelling**

The terrain is modelled using a 2D-Perlin-Noise-function (implemented from scratch!).
It overlays different frequences of sinus and cosinus waves.
The perlin-noise height field is then textured based on the current height (Overlapping of different textures).
The mountain like hill is added by adding a gauss function to the perlin-noise output.
The same goes for the valley where the water is.

## **Install && Run**

I only tested and ran this simulation on a debian-based Linux OS (Ubuntu, Mint, ...). It should run on other machines as well but is not
tested.

### **Requirements**

The following system-attributes are required for running this simulation:

- A graphics card supporting OpenGL version 3.3 (For the shaders).

- Unix-Libraries: xorg-dev, freeglut3-dev and mesa-common-dev

### **Running**

Compiling and running is pretty straight forward.

- make

- ./water

While the simulation runs, you can move around (always looking to the center!) with your mouse (left-klick and move).

Initializing a random movement (choppy water) is done with 'r'.

Creating one/several waves can be done by pressing 'n'.















