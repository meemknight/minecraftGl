# minecraftGl 

- So far I have managed to add a working cross platform screen setup, add a camera and a texture atlas to render faces.

![](https://github.com/meemknight/photos/blob/master/mc1.png)
---

-Then i managed to make a chunk to be rendered and a mesh builder to only display the faces that can actually be seen.

![](https://github.com/meemknight/photos/blob/master/mc2.png)
---

-After that it was time to add more chunks and a chumk manager that can load chunks near the player.

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