A biobjective parametric simplex
================================

This is a C++ implementation of a parametric simplex algorithm aimed at solving linear programmes with two objective functions.
It relies on the [OSI](https://projects.coin-or.org/Osi) library.


Installation
------------

You first need to install CMake, CppUnit, Coin-OR's CLP and OSI:  
`apt-get install cmake libcppunit-dev coinor-libclp1 coinor-libclp-dev coinor-libcoinutils3 coinor-libcoinutils-dev coinor-libosi1 coinor-libosi-dev`

Then:  
`cd your_work_dir`  
`git clone https://github.com/tvincent2/mo-utils.git`  
`git clone https://github.com/tvincent2/biobj-simplex.git`  
`cd biobj-simplex`  
`mkdir build`  
`cd build`  
`cmake ..`  
`make`

You can then lauch tests:  
`./test/biobj-simplex-tests`

How to use it
-------------

Biobj-simplex can be used a callable library. Examples can be found in the `examples` directory.

It is planed to add a mode where the user just calls an executable and passes an instance file as a parameter.


How it works
------------

For more information about this, see _Multicriteria Optimization_ by Matthias Ehrgott, (Springer, 2005, ISBN 978-3-540-27659-3).


Why OSI
-------

Because it's free (as in free speech).


License
-------

LGPL 2.1. See LICENSE.md for more info.
