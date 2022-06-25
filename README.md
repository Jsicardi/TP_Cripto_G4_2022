# TPI

## 72.44 - Criptografía y Seguridad - 1º cuatrimestre 2022

### Instituto Tecnológico de Buenos Aires (ITBA)

## Autores

- [Sicardi, Julián Nicolas](https://github.com/Jsicardi) - Legajo 60347
- [Cruz, Ana](https://github.com/anitacruz) - Legajo 60478
- [Oseroff, Paula](https://github.com/prausela) - Legajo 58415

## Índice
- [Autores](#autores)
- [Índice](#índice)
- [Descripción](#descripción)
- [Ejecución](#ejecución)
- [Configuracion ejemplo](#configuraciones-ejemplo)

## Descripción

El proyecto fue desarrollado en C y se centra en la implementacion de un programa para ocultar mensajes dentro de archivos bmp y luego extraerlos, utilizando esteganografia. Este utiliza los algoritmos LSB1, LSB4 y LSBI.

Adicionalmente, únicamente con el motivo de poder procesar resultados de comandos de linux de manera sencilla se incluyeron dentro de la carpeta analysis una serie de archivos Bash y Python. Los archivos de Python sirven para obtener promedios y desvíos estándar de la salida de los archivos bash. 

## Requerimientos

- C
- gcc
- openssl

## Ejecución

Para compilar el programa, posicionado en el directorio principal del proyecto, ejecutar:

```bash
$ make clean
$ make all
```

Esto generara el ejecutable `stegobmp`, el cual se usa para ocultrar o extraer informacion como se menciona en la seccion Descripcion.
Pueden verse los parametros del programa ejecutando el comando:

```bash
$ ./stegobmp -h
```

## Configuraciones ejemplo

Si por ejemplo buscase ocultar el archivo `hide.txt` dentro del archivo portador `carrier.bmp` lo cual genera el archivo `modified.bmp`, todo esto utilizando el algoritmo LSB1, se puede utilizar el comando:

```bash
$ ./stegobmp --embed --in hide.txt -p carrier.bmp --out modified.bmp --steg LSB1
```

Si ahora buscase ocultar el archivo `hide.txt` dentro del archivo portador `carrier.bmp` lo cual genera el archivo `modified.bmp`, pero utilizando LSB4 con encripcion AES-128 en modo CBC con password "secreto", se puede utilizar el comando:

```bash
$ ./stegobmp --embed --in hide.txt -p carrier.bmp --out modified.bmp --steg LSB4 -a aes128 -m cbc --pass secreto
```

Finalmente si buscase extraer el archivo oculto en `modified.bmp` podria hacerlo con el comando:

```bash
$ ./stegobmp --extract -p modified.bmp --out hide --steg LSB4 -a aes128 -m cbc --pass secreto
```