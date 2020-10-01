
## CPE 442 : Sobel Filter

## Author(s)
* Victor Delaplaine
* Tristin Chutka

## Video link
* [Video demonstrating the sobel program with pthreads](https://youtu.be/g0mPtUYl0p4)


diffence in time between tut2 and tut3 : https://i.imgur.com/Xl6p64J.png
### Alternatives for using threads
* Divide each image into four and each thread take a differnt quarter of the frame
* you can have 1-2 core(s) perform grayscale on next frame, 1-2 core(s) perform the Sobel on the current frame and 1 more core to coordinate frame grabbing and displaying
* You can even have each core take an entire frame and work on processing four frames at once.
