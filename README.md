# Distributed MD5 Calculation Project with IPC

## Description
The goal of this project is to demonstrate **IPC (Inter-Process Communication)** for calculating **MD5 hashes** of multiple files in a distributed way. It also allows visualizing the results by using shared memory.

## Contents
1. [Requirements](#requirements)
2. [Compilation](#compilation)
3. [Usage](#usage)
4. [Warnings](#warnings)

---

## Requirements

The project requires the following:

- **Docker**: To manage the containers.
- **The Docker image provided by the faculty**: This image includes the necessary configurations to run the project.

To install Docker and the required image, follow these instructions:
[Docker and image installation instructions](https://github.com/alejoaquili/ITBA-72.11-SO/blob/main/docker/README.md/)

---

## Compilation

To compile the project, run the following command inside the Docker container:

```bash
make all

```

## Usage

The project can be executed using either **pipes** or **separate terminals**.

### 1. By Pipes

This method allows you to process multiple files in a single line:

```bash
./md5 <path 1> <path 2> <path 3> ... | ./view
```
or you can process all the files by using *:

```bash
./md5 * | ./view
```

### 2. Using  md5 alone

`./md5` can also be run by doing:

```bash
  ./md5 <path 1> <path 2> <path 3> ...
```

### 3. Using slave

To see the MD5 hash of a single file, run `./slave`. First, call `./slave`, then pass the file path.


### 4. Using separate terminals

To work with separate terminals, follow the steps:

- On **terminal 1**, run the command `./md5` to get the MD5 hash from the files wanted:

  ```bash
  ./md5 <path 1> <path 2> <path 3> ...
  ```
- On **terminal 2**, execute the `./view` file to see the results saved in the shared memory:
- 
 ```bash
  ./view shared_memory
  ```



## Warnings

1. **Docker Execution**:  
   Ensure that all commands are executed inside the Docker container with the provided image. Otherwise, the project may not work.

2. **Execution Timing**:  
   It is important to execute `./view` while `./md5` is still processing files. If `./md5` finishes before `./view` starts, `view` will not function properly.


