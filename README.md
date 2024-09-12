SO
======
La idea del proyecto fue mostrar el uso de IPC, en donde distribuimos archivos para calcular el md5 de multiples procesos. Luego tenemos el viewen donde junto a la memoria compartida se nos permite ver lo que se logro con los md5.

Contenidos
-----------------

 * requerimientos
 * creacion
 * uso


requerimientos
------------

El proyecto necesita de:
 * Docker
 * Imagen provista por la catedra

Los cuales se pueden instalar siguiendo la guia de la catedra provisto en el siguiente link:
https://github.com/alejoaquili/ITBA-72.11-SO/blob/main/docker/README.md/

Creacion
-----
Para generar la compilacion total del trabajo, y poder usar tanto ./md5 como ./view se debe correr el make all dentro del container
```s
make all
```

Uso
-----
Se puede usar tanto desde el pipe o en 2 terminales.

Usando lo con el pipe podemos poner un archivo tras otro:
```sh
./md5 file1 file2 file3 ... | ./view

```
o podemos poner un asterisco para todos los archivos:
```sh
./md5 * | ./view

```
Para usar en 2 terminales distintas hacemos lo siguiente:

En la terminal 1 corremos:
```sh
./md5 file1 file2 file3 ...
```
En la terminal 2 corremos:
```sh
./view shared_memory
```
(tener en cuenta que esto se debe hacer rapido, mientras este corriendo el md5. Ademas no olvidarse que se debe estar corriendo dentro del container)


