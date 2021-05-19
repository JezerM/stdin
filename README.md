# Tasky
Tasky es un pequeño proyecto, escrito en C/C++, orientado a la organización mediante una lista de tareas o agenda, y un sistema de monitorización del tiempo.

## Simple
Esta versión de Tasky no implementa un menú interactivo, sino un menú simple, sencillo, que se controla mediante opciones a través del teclado.

## Dependencias
```bash
sudo apt install libopenal-dev build-essential
```

## Instalación
```bash
git clone https://github.com/JezerM/stdin
```
Es 100% necesario tener un entorno Linux, debido a la librería `termios.h`

Para la compilación, se recomienda usar `make`.

## Funcionamiento
Se ingresa en un loop infinito, que imprimirá las distintas opciones y solicitará una opción al usuario. Según sea la opción elegida se hará una acción u otra, ya sea entrar a otro menú o efectuar una acción directa.

## Uso
Se recomienda leer todos los archivos.
