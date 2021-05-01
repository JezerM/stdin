# Tasky
Tasky es un pequeño proyecto, escrito en C/C++, orientado a la organización mediante una lista de tareas o agenda, y un sistema de monitorización del tiempo.

Presenta un menú interactivo hecho desde 0 con `termios.h`, con la capacidad de detectar el mouse, así como la implementación de procesos asíncronos, en el caso del temporizador.

## Minimal
Esta versión de Tasky no implementa clases, por lo que su funcionamiento se reduce estructuras y funciones.

## Instalación
```bash
git clone https://github.com/JezerM/stdin
```
Es 100% necesario tener un entorno Linux, debido a la librería `termios.h`

Para la compilación, se recomienda usar `make`.

## Funcionamiento
Se usa la librería `termios.h` para activar el modo `raw`, el cual permite que cada carácter sea recibido inmediatamente por el programa y evitar que éste sea imprimido en pantalla.

Luego, se inicia un bucle que espera a que un carácter se reciba y realiza las acciones debidas.

Gracias a una funcionalidad de `termios.h`, se mandará una señal de "escritura" cada cierto tiempo, con lo que el menú se podrá actualizar automáticamente sin necesidad de esperar la interacción del usuario, permitiendo que menús como el **Temporizador** puedan actualizarse y renderizarse correctamente.

El **Temporizador** hace uso de procesos asíncronos con `future` y `async`, para poder actualizar paralelamente el tiempo transcurrido y así no parar el programa.

## Uso
Se recomienda leer todos los archivos.
### Menús
El header `menu.h` posee las estructuras correspondientes para cada menú.

Con configuraciones globales se especifica qué menú se usa actualmente y se realizan las acciones debidas con el mismo, usando la función `render` para ello.

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
Sólo basta centrarse en las dos primeras, que son fundamentales de definir (si no se definen generará errores).

Cuando una tecla es recibida, `processKey` se encarga de hacer lo necesario. Si se recibe una tecla de movimiento (flechas o "hjkl"), se llama a `moveCursor` y a `manageMenus`.

Este último realiza la interacción con el menú. Recibe el `id` tanto del menú actual como de la opción actual (o bien cualquier otra opción). Así, luego sólo se efectúan las acciones según la opción.

```c++
auto act = listMenus[conf.actualMenu];
manageMenus(act->id + "/exit"); // Regresa al menú anterior
manageMenus("main/exit"); // Cierra el programa
string element = act->id + "/" + act->options[act->actualPos].id;
manageMenus(element); // Envía la opción actual, y hace lo que tenga que hacer
``
