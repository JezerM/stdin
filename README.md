# STDIN
Esto es un pequeño proyecto, para que vean más o menos lo que se hará como base. Creando un loop que detecta las teclas pulsadas y reacciona a ellas.

## Instalación
```bash
git clone https://github.com/JezerM/stdin
```
Es 100% necesario tener un entorno Linux, debido a la librería `termios.h`

Y luego lo compilan con `Code::Blocks` ó con `GCC`.

## Funcionamiento
Se usa la librería `termios.h` para activar el modo `raw`, el cual permite que cada carácter sea recibido inmediatamente por el programa y evitar que éste sea imprimido en pantalla.

Luego, se inicia un bucle que espera a que un carácter se reciba y realiza las acciones debidas.

## Uso
Se recomienda leer todos los archivos.
### Menús
El header `menu.h` posee las clases correspondientes para cada menú. Principalmente, poseen la función `update` y `render`, que sirven para actualizar su información e imprimirla en pantalla por medio de `abWrite`.

Además, existe la función `gotoPos` para actualizar la posición del cursor.

### Escribir en pantalla
Para imprimir en pantalla se usa un buffer, el cual cada vez que se llame a la función `refreshScreen` se imprimirá en pantalla, y posteriormente se reiniciará.

Se implementa la función `abAppend` que agrega una cadena de carácteres con su tamaño específico. Debido a su "complejidad" a la hora de usarlo, se implementó la función `abWrite` que pasa una cadena de carácteres como `string`, sin necesidad de especificar el buffer ni su tamaño.

Así, para escribir basta con usar `abWrite`.

Se recomienda borrar la línea antes de escribir al buffer, para evitar mantener carácteres "en el aire", con `"\x1b[K"`

No se recomienda borrar la pantalla entera, pues puede generar parpadeos molestos.

```c++
abWrite("\x1b[K");
abWrite("Hola a todos!");
```

### Manejar los menús
Las opciones de un menú están definidas por una estructura llamada `MenuOption`, que consiste en:
```c++
struct MenuOption {
  char name[50]; // El nombre a mostrar en pantalla
  char id[20]; // El identificador
  int indentTimes = 0; // La cantidad de veces que usará *indentSpace* sobre la base
  int posx = 0; // La posición x en pantalla (No inicializar manualmente)
  int posy = 0; // La posición y en pantalla (No inicializar manualmente)
};
```
Sólo basta centrarse en las dos primeras, que son fundamentales de defindir (si no se definen generará errores).

Cuando una tecla es recibida, `processKey` se encarga de hacer lo necesario. Si se recibe una tecla de movimiento (flechas o "hjkl"), se llama a `moveCursor` y a `manageMenus`.

Este último realiza la interacción con el menú. Recibe el `id` tanto del menú actual como de la opción actual (o bien cualquier otra opción). Así, luego sólo se efectúan las acciones según la opción.

```c++
manageMenus(menu.id + "/exit"); // Regresa al menú anterior
manageMenus("main/exit"); // Cierra el programa
string element = menu.id + "/" + menu.options[menu.actualPos].id;
manageMenus(element); // Envía la opción actual, y hace lo que tenga que hacer
``
