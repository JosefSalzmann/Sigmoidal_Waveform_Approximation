# Waveform Prediction of Digital Circuits by Sigmoidal Approximation

## Currently work in progress

## Dependencies

Using [CppFlow](https://github.com/serizba/cppflow) to run pretrained TensorFlow models in C++.

Using [CryptoMiniSat](https://github.com/msoos/cryptominisat) as SAT solver.

Using [plog](https://github.com/SergiusTheBest/plog) for logging.

Using [pcl](https://pointclouds.org/) for handling point clouds.

Using [cgal](https://www.cgal.org/) for geometrical operations.

## Usage
After compiling with: ```make all``` simulations can be conducted by specifying an input file with option ```-c``` e.g.,
```
./main -c ./minimal_example/ISCAS85_c17.txt
```