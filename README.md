# NNS: NNS is Not SSH
## Project Overview
NNS (Not SSH) is a reverse proxy load balancer built with C that dynamically manages multiple shell sessions for users connecting over a network. Unlike traditional SSH, NNS provides a custom solution where the load balancer spawns a specified number of forked processes, each running its own shell for executing Unix commands.

The number of forks (n) and the port range for these forks are defined as arguments when the load balancer is started. The load balancer listens on a primary port, and each forked process listens on its own designated port from the provided range. When a user connects, the load balancer routes them to a forked shell based on traffic, ensuring efficient session distribution. Each user gets their own dedicated shell session, allowing them to execute Unix commands as needed.

Additionally, the project includes a client CLI application that enables users to connect to the service remotely over the network.

### Key Features
Reverse proxy load balancer built in C
Dynamic process spawning for n forked processes, defined at startup
Port range allocation for forked processes
Custom Unix shell built as part of the project, running in each fork
Load balancer intelligently routes users to the appropriate shell based on traffic
Each connecting user is provided with their own session
Includes a client CLI application for remote connections
