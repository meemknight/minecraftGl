# minecraftGl 

- So far I have managed to add a working cross platform screen setup, add a camera and a texture atlas to render faces.

![](https://github.com/meemknight/photos/blob/master/mc1.png)
---

-Then i managed to make a chunk to be rendered and a mesh builder to only display the faces that can actually be seen.

![](https://github.com/meemknight/photos/blob/master/mc2.png)
---

-After that it was time to add more chunks and a chunk manager that can load chunks near the player.

![](https://github.com/meemknight/photos/blob/master/mc3.png)
---

-Then it was time to optimize the game so that chunks would only process their mesh once.
Also i had to make shure that i only draw chunks that are visible to the payer

![](https://github.com/meemknight/photos/blob/master/mc4.png)
---

-Before I could start implementing the phisics I had to first make some simple things like being able to draw a border
around blocks or place and remove blocks. For that i made a very gross implementation of a ray cast.
![](https://github.com/meemknight/photos/blob/master/mc5.png)
---

-I started to add some simple AABB type collision detection. It is now working just in one direction and one axe.
![](https://github.com/meemknight/photos/blob/master/mc6.png)
---

-I created a save system that saves 4x4 chunks into a single file and uses lazyness so that it only saves chunks that 
have actually been modified by the player.
![](https://github.com/meemknight/photos/blob/master/mc7.png)
---

-I have improved the world generation and added customizeble biomes that can be seen here.
![](https://github.com/meemknight/photos/blob/master/mc8.jpg)
---

-Now I'll take a small break from the world generator to fix bugs and add things like physics and controller input.
---


-I have made some small optimizations in the chunk code so that I can controll how many chunks are created and updated
per frame and also made sure to update first chunks that are near the player and clear chunks that are far away.
There is still room for improvement.
---

-I have added a skybox and fog.
![](https://github.com/meemknight/photos/blob/master/mc10.png)
---
