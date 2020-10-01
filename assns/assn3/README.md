## CPE 442 : Multhreaded Sobel Filter

## Author(s)
* Victor Delaplaine
* Tristan Chutka

## Video link
* [Video demonstrating the sobel program with pthreads](https://youtu.be/g0mPtUYl0p4)

## Timing Table
|           | Single | Multithreaded |
|-----------|--------|---------------|
|average fps|   10   |     29.97     | 


### Methodology
This program is an improvement upon the last assignment, as it now spawns N worker threads, each of which will process a single frame. In this way, N frames are buffered, resulting in significantly smoother playback on multicore machines. 

#### Alternatives solutions
* Divide each image into four and each thread take a differnt quarter of the frame
* you can have 1-2 core(s) perform grayscale on next frame, 1-2 core(s) perform the Sobel on the current frame and 1 more core to coordinate frame grabbing and displaying
* You can even have each core take an entire frame and work on processing four frames at once.
