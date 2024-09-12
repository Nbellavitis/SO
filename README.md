# Proyecto de Cálculo de MD5 Distribuido con IPC

## Descripción
Este proyecto muestra el uso de la Comunicación entre Procesos (IPC) para calcular el **hash MD5** de múltiples archivos de manera distribuida. También permite visualizar los resultados utilizando memoria compartida.

## Contenidos
1. [Requerimientos](#requerimientos)
2. [Creación](#creacion)
3. [Uso](#uso)

---

## Requerimientos

El proyecto requiere de:

- **Docker**: Para la gestión de contenedores.
- **Imagen provista por la cátedra**: Para las configuraciones específicas.

Puedes seguir las instrucciones de instalación en el siguiente enlace:

[Guía de instalación Docker e imagen](https://github.com/alejoaquili/ITBA-72.11-SO/blob/main/docker/README.md/)

---

## Creación

Para compilar todo el proyecto s `./md5` y `./view`, se debe correr el siguiente comando dentro del contenedor Docker:

```bash
make all
```
## Uso

El proyecto se puede ejecutar de dos maneras: **usando pipes** o **en dos terminales separadas**.

### 1. Uso con pipes

Este método permite procesar varios archivos en una sola línea:

```bash
./md5 archivo1 arcivo2 archivo3 ... | ./view
```
O puedes procesar todos los archivos en el directorio con:

```bash
./md5 * | ./view
```

### 2. Uso en terminales separadas

Para usar dos terminales distintas, sigue estos pasos:

- En la **terminal 1**, corre el comando `./md5` para calcular los hashes MD5 de los archivos:

  ```bash
  ./md5 archivo1 archivo2 archivo3 ...
  ```
- En la terminal 2, ejecuta el visor ./view para ver los resultados almacenados en la memoria compartida:
- 
 ```bash
  ./view shared_memory
  ```

##  Aviso Importante

1. **Ejecución en Docker**:  
   Asegúrate de que todos los comandos (`./md5` y `./view`) se ejecuten dentro del contenedor Docker provisto por la cátedra. De lo contrario, no deberia funcionar correctamente.

2. **Tiempo de Ejecución**:  
   Es importante ejecutar `./view` mientras `./md5` aún está procesando los archivos. Si `./md5` termina antes de que se inicie `./view`, los datos en la memoria compartida podrían no estar disponibles para su visualización.

3. **Docker Make**:  
   No olvidase  decompilar el proyecto dentro del contenedor usando el comando `make all` antes de intentar ejecutar cualquier proceso.


