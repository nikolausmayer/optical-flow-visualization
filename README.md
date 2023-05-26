# Optical Flow Visualization
Generate animated visualizations for optical flow fields

<img src='https://raw.githubusercontent.com/nikolausmayer/optical-flow-visualization/master/example/0048.png' width='30%' alt=''/>  <img src='https://raw.githubusercontent.com/nikolausmayer/optical-flow-visualization/master/example/0049.png' width='30%' alt=''/>  <img src='https://raw.githubusercontent.com/nikolausmayer/optical-flow-visualization/master/example/flow.gif' width='30%' alt=''/>

# Requirements
Tested with GCC 7.3 on Ubuntu 18.04.2. ImageMagick and Gifsicle required for output conversion.

# Building
Just `make`. This pulls the dependencies [CImg](http://cimg.eu/) and [AGG](https://github.com/nikolausmayer/AntiGrainGeometry-v2.4), so the first build takes a bit longer. Note that CImg and AGG are *not* under this repository's license.

# Running
`$ bash generate-flow-animation.sh example/0048.pfm`. 
This generates flow frames, converts them to GIF (using [ImageMagick](https://www.imagemagick.org/)) and creates an animation (using [Gifsicle](https://www.lcdf.org/gifsicle/)).
The example is from our [scene flow dataset](https://lmb.informatik.uni-freiburg.de/resources/datasets/SceneFlowDatasets.en.html).


Optical flow visualization color code by [Daniel Scharstein](http://vision.middlebury.edu/flow/data/) and others, now lost in time.
