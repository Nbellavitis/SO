# Distributed MD5 Calculation Project with IPC


## Description
The goal of this project was to demonstrate IPC for calculating **hash MD5** of multiple files in a distributed way. It also alows to visualize the results by using shared memory.

## Contents
1. [Requirements](#requirements)
2. [Creation](#creation)
3. [Use](#use)
4. [Warinings](#warnings)

---

## Requirements

The project requires the following:

- **Docker**: To manage the containers.
- **The docker image provided by the faculty**: This image includes the necessary configurations to be able to run the project.

To install docker and the required image, follow the next instrucions:
[Docker and image instalation instructions](https://github.com/alejoaquili/ITBA-72.11-SO/blob/main/docker/README.md/)

---

## Creation

To compile the project, we should run the following command inside the Docker container:

```bash
make all
```

## Use

The project can be executed, both **using pipes** or **with two terminals**.

### 1. By pipes

This method allows us to process multiple files in a single line;

```bash
./md5 file1 file2 file3 ... | ./view
```
or you can process all the files by using *:

```bash
./md5 * | ./view
```

### 2. Using separate terminals

To work with both terminals, follow the code:

- On **terminal 1**, run the command `./md5` to get the MD5 hash from the files wanted:

  ```bash
  ./md5 archivo1 archivo2 archivo3 ...
  ```
-On **terminal 2**, execute the `./view` file to see the results saved in the shared memory:
- 
 ```bash
  ./view shared_memory
  ```

##  Warnings

1. **Docker Execution**:  
   Be sure that all the commands are executed in the docker contanier, with the provided image. To contrary it shouldnt work.

2. **Execution Time**:  
   Its important to execute `./view` as `./md5` is still processing files. If `./md5` finishes before `./view` starts, view is not going to be able to work/


