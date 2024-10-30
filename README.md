# &#x1F41C; Ant-Colony-Salesman &#x1F41C;

Ant colony optimization on the Travelling salesman problem.

Or how to find a good solution to help a travelling salesman to find one of the shortest (maybe the shortest) path joining 36 french cities and return back to the first one.

From Wikipedia:

Ant colony optimization algorithms have been applied to many combinatorial optimization problems, ranging from quadratic assignment to protein folding or routing vehicles and a lot of derived methods have been adapted to dynamic problems in real variables, stochastic problems, multi-targets and parallel implementations. It has also been used to produce near-optimal solutions to the travelling salesman problem. They have an advantage over simulated annealing and genetic algorithm approaches of similar problems when the graph may change dynamically; the ant colony algorithm can be run continuously and adapt to changes in real time. This is of interest in network routing and urban transportation systems.

The first ACO algorithm was called the ant system and it was aimed to solve the travelling salesman problem, in which the goal is to find the shortest round-trip to link a series of cities. The general algorithm is relatively simple and based on a set of ants, each making one of the possible round-trips along the cities. At each stage, the ant chooses to move from one city to another according to some rules:

    - It must visit each city exactly once;
    - A distant city has less chance of being chosen (the visibility);
    - The more intense the pheromone trail laid out on an edge between two cities, the greater the probability that that edge will be chosen;
    - Having completed its journey, the ant deposits more pheromones on all edges it traversed, if the journey is short;
    - After each iteration, trails of pheromones evaporate.

<img src="sample.gif" alt="ant colony" width=400 />

Visualization of the ant colony algorithm applied to the travelling salesman problem. The whiter the line, the more pheromones there are. Selected solution appears in cyan.

# Compilation on macOS
CMakeFile build system is used to create a Makefile._
SFML graphic library is mandatory to display the vizualisation of the algorithm.__
These tools are made availables with [homebrew](https://brew.sh/) on macOS.__

# References
- [How to Apply Ant Colony Optimization to Traveling Salesman Problem (TSP) Numerical Example~xRay Pixy](https://www.youtube.com/watch?v=8lYKzj470zc&t)
- [Ant colony optimization algorithm](https://www.youtube.com/watch?v=u7bQomllcJw&t)
