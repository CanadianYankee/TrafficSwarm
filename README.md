# TrafficSwarm

## Swarm-intelligence-based traffic optimizer

A windows application written for Windows in C++ and HLSL with Visual Studio 2019 (DirectX 11 capable graphics card required).

### Purpose of this project

This application was written as a final project for the Fall 2019 Univeristy of Toronto Continuing Education course "Intelligent Agents and Reinforcement Learning". 

### Abstract

In this project, I adapt the well-known “boids” algorithm, building on recent work in swarm intelligence for free-flying drones. Rather than optimize for swarm cohesion in a relatively wide-open field of flight, I have am optimizing for two-dimensional travel down a restricted-width road, possibly with twists or obstacles, and maximize the total through-put of agents while minimizing the number of collisions between agents or with the walls of the course. To do this, I have parameterized the agent behavior, with some adjustments from the traditional behavior models, notably the interaction with the walls and adding an asymmetry between “forward” and “backward” neighbors. The model is then trained via a genetic algorithm, where a large ensemble of randomly-chosen parameter sets are all tested, and then the best 10% of the run are allowed to survive and are “bred” with each other to create the next generation of candidates. The resulting optimized agents perform well even in other courses they were not trained on, although there are certainly other types of courses that trained agents could not navigate. 

### Key source code files

* `AgentIterateCS.hlsl` – implements the interaction model and the physics of motion and collisions. 
* `EvolutionDlg.cpp` – implements the genetic algorithm, handling the sorting, culling, and breeding of the next generation of candidates.
* `TrialRun.h` – defines a structure called `RUN_RESULTS` that stores all of the statistics from a trial run, including the parameter set of the agents in the run, as well as calculating the run’s score. 
* `AgentGenome.cpp` – stores the parameter set (a genome) and has the code for breeding together two parent genomes sets to make a child genome.

There are a number of test courses (described in json files) in the folder `Courses` and some sets of trained agent sets stored in text files in the folder `TrainedSets`.

### References

Reynolds, Craig (1987). [Flocks, herds and schools: A distributed behavioral model.](https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.103.7187) SIGGRAPH '87: Proceedings of the 14th Annual Conference on Computer Graphics and Interactive Techniques. Association for Computing Machinery. pp. 25–34. 

Gábor Vásárhelyi, et.al. (2018). [Optimized flocking of autonomous drones in confined environments.](https://robotics.sciencemag.org/content/3/20/eaat3536) Science Robotics 18 Jul 2018: Vol. 3, Issue 20, eaat3536.