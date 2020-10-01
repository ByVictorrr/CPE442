
## CPE 442 : Sobel Filter

## Author(s)
* Victor Delaplaine
* Tristin Chutka

## Video link
* [Video demonstrating the sobel program](https://youtu.be/jWrUpy8nI4M)

## Installing/Setting up OpenCV
### Installing
* Depending on what operating system you are working on for this part.
* On a terminal type the following:
    * For MacOS: `sudo brew update && sudo brew install pkg-config opencv`
    * For Debian distros: `sudo apt update && sudo apt install -y pkg-config libopencv-dev`
    * For Arch distros: `sudo pacman -Syu pkg-config opencv`
* Above assumes that opencv4 was installed

### Setting up OpenCV
* This section is about linking opencv libraries and putting in headers in the c/c++ files

#### Linking OpenCv libraries to your program
    * In your Makefile for the `C_FLAGS` add a link to all the opencv libraries:

        `C_FLAGS+=pkg-config --cflags --libs opencv4` 

    * If you dont want to add all the libraries example:

        `C_FLAGS+= -I<path to opencv4 folder> -lopencv_<name(s)>`

#### Putting headers in your c/c++ files
    * Find the full path to your opencv folder 
        * Extract part of the path excluding the path to the opencv4 folder
            * For an example `full_path=/usr/include/opencv4/opencv2/imgcodecs.hpp`
            * In your c/c++ you would include the opencv2/imgcodecs.hpp part
    
## Grabbing a frame in OpenCV
Assuming that you are using VideoCatpure(vc) to grab a frame. The easiest way to grab a frame from vc is to use the >> operator and make sure that the vc object is on the left of that operator and the Matrix(Mat) object is on the right of it. The question now is - how do you know when there are no frames left in vc? This is simple just use the empty() function on that frame.

## Displaying a frame in OpenCV
To display a frame is easy in opencv. There is a function called imshow(char *, Mat), the char * parameter is the name of the GUI window that will pop up and the Mat is your frame object. Its good to add a delay after you call this function if your trying to show a video(set of frames). This can be acheived by the waitKey(ms) function, the only parameter is the time till the next frame (depending on your setup).