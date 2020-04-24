To build this project you need to have Qt (it was originally written for Qt 5.6.2) and, I think, Qt Creator as well.

To be able to run this game folders mat/ and obj/ should be in the working directory of the resulting binary.

Thrillide was written with some attempts at OpenMP parallelizing, it was needed due to the fact that I was drawing to Qt surfaces which is slow as fuck (it was forbidden to use OpenGL or any other graphics library so I did it all in software) so a few extra threads do help here. As a result of those attempts the game is playable with a Macbook Pro 2010 (4-threaded i5 CPU). 