
![milestone1](/releases/milestone1/demo_images/header_m1.png)
# pruitt-igoe
A short procedural animation.


## Milestone 1 

Installation:

* Download /releases/milestone1.zip and extract
* Execute pruitt-igoe.exe

Features implemented:

* Basic engine architecture flexible enough for building different effects. 
* FBO architecture to easily manage render textures.
* Proof of concept algorithm that uses temporal coherence to optimize raymarching.

Additional Notes

* The terrain itself is a texture heightmap I built with World Machine. I decided to use this for blocking; however, the final version will generate these textures in runtime.
* The temporal coherence algorithm is not optimal and does not use any kind of reprojection (yet). While the possibilities with this approach are very interesting, I'll reduce the scope of its development -- the idea of this project is the demo, not the algorithm.
* It suffers problems when there are occluders ore very small SDFs; also, because the distance estimation of the heightmap is only its vertical distance, the result is not very good on ridges (because the point is very close to a rock, but very high, for example). These kinds of problems can be probably removed with more information about the terrain, such as its gradient. 

However, the optimization is really good: compare a render with 100 iterations that takes ~7ms:

![milestone1](/releases/milestone1/demo_images/naive_raymarcher.png)

With an optimized raymarching algorithm that only uses AT MOST 10 iterations and renders in ~2ms

![milestone1](/releases/milestone1/demo_images/full_raymarching.png)


## Bugs

The following are interesting bugs that I had while working on the acceleration method. Note that in the first image, the recursive feedback can be seen in the lines that decay.

![milestone1](/releases/milestone1/demo_images/bug1.png)

![milestone1](/releases/milestone1/demo_images/bug2.jpg)
