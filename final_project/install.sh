cd ~/opencl/VC4CLStdLib
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

cd ~/opencl/VC4C
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

cd ~/opencl/VC4CL
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig