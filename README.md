A biobjective parametric simplex
================================

This is a C++ implementation of a parametric simplex algorithm aimed at solving linear programmes with two objective functions.
It relies on the [OSI](https://projects.coin-or.org/Osi) library.


Installation
------------

You first need to install Coin-OR's CLP and OSI:  
`apt-get install …`

Then:  
`git clone`  
`cd build`  
`cmake ..`  
`make`

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

To be decided.
