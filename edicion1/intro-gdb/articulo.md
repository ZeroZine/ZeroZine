<!-- vim: set spelllang=es,en:
-->

---
geometry: margin=2.5cm
output: pdf_document
---

---

Este artículo tiene el propósito de servir como una introducción, un primer
contacto con GDB tanto si se pretende usar como debuger, o como herramienta de
ingeniería inversa. Al ser una introducción, no se necesita ningún conocimiento
previo especial, pero es conveniente haber tocado algo de C y algo de ASM.

Introducción
============

¿Qué es un depurador?
---------------------

Un depurador (o debugger en ingles) es un programa que nos permite analizar el
comportamiento de un programa. Algunos depuradores, como GDB nos pueden llegar a
mostrar la línea de código original a alto nivel que se esta ejecutando además
de mostrarnos el código ensamblador y el estado de los registros.

Interfaces gráficas de GDB
--------------------------

Existen varias interfaces gráficas para GDB (como DDD), sin embargo en
este texto se usara principalmente el modo consola. Las interfaces
gráficas de GDB suelen dejar bastante que desear y al final es mucho más
didáctico aprender a usarlo por consola. Además habrá ocasiones en los
que no sea posible usar las interfaces gráficas.

Primeros pasos
==============

*Nota: Los códigos fuente que se usaran están en la carpeta
”ejemplos”.*
En este capítulo haremos un repaso sobre que son y como colocar,
breakpoints, como ver los registros, leer el stack, imprimir variables, 
y como configurar GDB. 

Hello world GDB
---------------

Una vez ya tengamos instalado GCC y GDB procederemos a compilar y
analizar un simple Hello World. Como se ve el programa solo imprime
“Hello world GDB” por pantalla.\
Compilemos el código usando gcc, **gcc hello.c -ggdb -o hello**. La
opción o flag *-ggdb* generara los *debugging symbols* que el compilador usara
para mostrarnos información acerca del código fuente. 
Una vez compilado debemos abrir este ejecutable con GDB. Para hacerlo
podemos especificar el nombre como un parámetro, o usar el comando
*file* seguido del nombre del ejecutable.

Si ejecutamos el comando *gdb* podremos ver un mensaje introductorio y
información sobre la licencia.

    GNU gdb (GDB) 7.8.50.20150104-cvs
    Copyright (C) 2015 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "x86_64-unknown-linux-gnu".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.
    For help, type "help".
    Type "apropos word" to search for commands related to "word".
    (gdb) 

Una vez ya iniciado GDB podemos usar el comando *file* como se dijo
antes. Como vemos a continuación, al ejecutar este comando GDB nos informara de
que ha podido leer los símbolos de depuración con el siguiente mensaje:

    (gdb) file hello
    Reading symbols from hello...done.
    (gdb)

Ahora gracias a la información de depuración y que tenemos el código
fuente, podemos ejecutar el comando *list* o *l* (l se puede usar como abreviación
para list). Este comando nos mostrara el código fuente de 10 en 10
líneas. Para listar rangos concretos se le puede pasar como parámetro la
primera y última línea a listar. Por ejemplo para listar entre la 4 y la
6 escribiríamos *l 4,6*. 

    (gdb) l 4,6
    4 {
    5   puts("Hello world GDB");
    6 }
    (gdb)

Para salir sepuede usar el comando *quit* (o su versión abreviada *q*) o bien con la
combinación típica Ctrl-D.

Jugando con variables
---------------------

A veces puede interesarnos leer una variable, o cambiarle el valor. Por
ejemplo para ver el valor apuntado por un puntero.
Para poder estudiar el estado de las variables en un cierto punto
usaremos breakpoints (un breakpoint es un punto de parada, donde se pausa la
ejecución para poder analizar el estado del programa en ese punto). En esta sección
solo veremos un ejemplo, más adelante estudiaremos los distintos tipos
de breakpoints y algunos de sus usos.
*El código que se usara esta en la carpeta **ejemplos/vars.c***
En el código podemos ver que hay dos variables, una es un puntero.
`unsigned int x,*y;`\
También podemos ver que hay 2 llamadas a scanf, la segunda tiene un
fallo claro que producirá un fallo de segmentación al intentar escribir
en el valor apuntado por **y** (ya que **y** no contendrá una dirección
válida).
`scanf("%d", y);`
Como en el ejemplo anterior, deberemos especificar la opción *-ggdb*. Dado que
la información acerca de la licencia y demás a veces puede ser molesta, podemos
ejecutar GDB con el flag *-q*, *-quiet" o *-silent* para evitar mostrarla.

    $ gdb -q vars
    Reading symbols from vars...done.
    (gdb)

Para poder analizar el valor de la variable **y** debemos poner un
breakpoint antes de ejecutarse la línea 12. Para poner un breakpoint en
ese segundo scanf simplemente podemos escribir el comando *break 12* o
*b 12*.

    (gdb) b 12
    Breakpoint 1 at 0x4005af: file vars.c, line 12.
    (gdb) 

Como vemos nos dice en que posición de memoria coloco el breakpoint, en
que archivo y en que línea. También nos dice que este breakpoint tiene asignado
ID 1.
Ahora que ya tenemos el breakpoint podemos ejecutar el programa con el
comando *run* (o ”r”). 

    (gdb) r
    Starting program: /tmp/codes/1/vars 
    5 

    Breakpoint 1, main (argc=1, argv=0x7fffffffeb08) at vars.c:12
    12	  scanf("%d", y);
    (gdb) 

Como vemos, GDB nos informa de la función en la que nos encontramos y el
valor de los parámetros. En este caso argc vale 1 y argv 0x7fffffffeb08.
Ahora por ejemplo podemos jugar con el comando *print* (o la
abreviación ”p”) para mirar el valor de las variables. Por ejemplo si
miramos la variable **x** nos mostrara 5, ya que le acabamos de asignar
un valor.

    (gdb) p x
    $1 = 5
    (gdb) p y
    $2 = (unsigned int *) 0x0
    (gdb)

Como vemos **x** vale 5 y **y** vale 0. En este caso en el siguiente
scanf se producirá un error al intentar acceder a la posición 0.\
Para que no diera fallo de segmentación podríamos jugar a modificar el
valor de **y** y asignarle la dirección de **x**. De esta forma
sobre-escribiremos el valor de **x**.\
Para modificar un valor a una variable podemos usar el comando *set
var*.

    (gdb) p &x
    $3 = (unsigned int *) 0x7fffffffea14
    (gdb) set var y= 0x7fffffffea14
    (gdb) p y
    $4 = (unsigned int *) 0x7fffffffea14
    (gdb) p *y
    $5 = 5
    (gdb)

Como vemos simplemente hicimos que **y** apuntara al valor de **x**.
Ahora ya podríamos usar el comando *continue* (o su abreviación ”c”)
para seguir con le ejecución.

    (gdb) c
    Continuing.
    2   
    Suma: 4
    [Inferior 1 (process 28169) exited normally]
    (gdb) 

Como vemos le dimos el valor 2 al segundo scanf sobre-escribiendo **x**,
por tanto la suma es 4. GDB ya nos dice que el programa finalizo
normalmente con el PID (ID del proceso) 28169.\
Otra forma más ”elegante” de corregir temporalmente este error podría
ser reservar memoria para **y**. GDB permite hacer llamadas a funciones
como malloc. Así que simplemente podríamos llamar a malloc(4) y
modificar la variable **y** para que apunte a esa dirección.

    (gdb) call malloc(4)
    $1 = 6295568
    (gdb) p y
    $2 = (unsigned int *) 0x0
    (gdb) set var y = 6295568
    (gdb) p *y
    $3 = 0
    (gdb) c
    Continuing.
    2
    Suma: 7
    [Inferior 1 (process 29501) exited normally]
    (gdb) 

Como vemos al principio **y** no apunta a nada y luego modificamos su
valor por la dirección devuelta por malloc. Como vemos la memoria que
reservo malloc contiene un 0. Continuamos con la ejecución y escribimos
un 2 en **\*y**. De esta forma suma el primer 5 introducido en **x** más
el 2 de **\*y**.

Tip: GDB recuerda el último comando ejecutado, así que si no introducimos ningún
comando ejecutara el último comando, lo cual viene bien para ejecutar acciones
repetitivas.

Modificando los registros
-------------------------

En esta sección veremos un ejemplo de como podemos leer y modificar
registros para que ejecute una función que en teoría no debería
ejecutarse.
*El código que se usara esta en **ejemplos/registros.c***
Empezaremos por poner un breakpoint en la función main

    (gdb) break main
    Breakpoint 1 at 0x400520: file registros.c, line 9.
    (gdb) r
    Starting program: /tmp/codes/1/registros 

    Breakpoint 1, main () at registros.c:9
    9	{}
    (gdb) 

Para mostrar las instrucciones ensamblador cercanas al punto donde nos
encontramos podemos usar el comando *disassemble* o la abreviación *disas*.

    (gdb) disas
    Dump of assembler code for function main:
    0x0000000000400517 <+0>:	push   %rbp
    0x0000000000400518 <+1>:	mov    %rsp,%rbp
    0x000000000040051b <+4>:	mov    $0x0,%eax
    => 0x0000000000400520 <+9>:	pop    %rbp
    0x0000000000400521 <+10>:	retq   
    End of assembler dump.
    (gdb) 

Como vemos nos indica con **=&gt;** la dirección a la que apunta el
registro EIP (o RIP en x86-64). También se puede observar que por defecto GDB
usa la sintáxis at&t, para cambiarlo a intel podemos usar el comando `set
disassembly-flavor intel`.

Si queremos que se ejecute la shell debemos cambiar el
registro rip para que apunte a la dirección correspondiente. Para eso
primero tenemos que saber la dirección de la función shell por ejemplo podemos 
usar los comandos vistos anteriormente.

    (gdb) p shell
    $1 = {void (void)} 0x400506 <shell>
    (gdb)

Ahora que sabemos donde esta solo tenemos que cambiar el registro RIP.
Para ver el estado de los registros podemos usar el comando:
`info registers ` (o `i r`)

    (gdb) i r
    rax            0x0	0
    rbx            0x0	0
    rcx            0x0	0
    rdx            0x7fffffffeb08	140737488349960
    rsi            0x7fffffffeaf8	140737488349944
    rdi            0x1	1
    rbp            0x7fffffffea10	0x7fffffffea10
    rsp            0x7fffffffea10	0x7fffffffea10
    r8             0x7ffff7dd6dd0	140737351871952
    r9             0x7ffff7dea6d0	140737351952080
    r10            0x833	2099
    r11            0x7ffff7a57760	140737348204384
    r12            0x400410	4195344
    r13            0x7fffffffeaf0	140737488349936
    r14            0x0	0
    r15            0x0	0
    rip            0x400520	0x400520 <main+9>
    eflags         0x246	[ PF ZF IF ]
    cs             0x33	51
    ss             0x2b	43
    ds             0x0	0
    es             0x0	0
    fs             0x0	0
    gs             0x0	0
    (gdb)

Cambiamos el registro RIP por la dirección correspondiente.

    (gdb) set var $rip = 0x400506
    (gdb) i r rip
    rip            0x400506	0x400506 <shell>
    (gdb)

Es importante destacar que los registros van con el símbolo del dolar
”\$” delante. También se puede apreciar que al comando `i r` se le puede
especificar el registro a imprimir, en este caso rip. Ahora si continuamos la 
ejecución con el comando `continue` tendremos nuestra shell.

    (gdb) c
    Continuing.
    $ exit
    [Inferior 1 (process 22319) exited normally]
    (gdb)

Otra forma más rápida de hacer lo mismo hubiera sido usar el comando
`set var` con el nombre de la función directamente.

    (gdb) set var $rip = shell
    (gdb) c
    Continuing.
    $

Crackeando binarios
-------------------

*El código del binario esta en **ejemplos/crackme.c***\
Esta vez compilamos el binario sin los flags de depuración (para hacerlo
más realista).\
Lo cargamos en GDB como de costumbre y ponemos un breakpoint en la
función main, le damos run y desensamblamos.
En el código desensamblado se pueden observar 3 llamadas a la función
puts. La primera simplemente imprime la string ”Introduzca el serial:” ,
las otras dos que se muestran a continuación son más interesantes.

     0x00000000004005b0 <+74>:	jne    0x4005be <main+88>
     0x00000000004005b2 <+76>:	mov    edi,0x40066e
     0x00000000004005b7 <+81>:	call   0x400430 <puts@plt>
     0x00000000004005bc <+86>:	jmp    0x4005c8 <main+98>
     0x00000000004005be <+88>:	mov    edi,0x400677
     0x00000000004005c3 <+93>:	call   0x400430 <puts@plt>
     0x00000000004005c8 <+98>:	mov    eax,0x0

Como vemos depende de una cierta condición imprimirá la string de la
dirección **0x40066e** y sino de la **0x400677**. Si imprimimos el valor
de las dos direcciones podemos ver que la primera dice ”Correcto” y la
segunda ”Incorrecto”.

    (gdb) p (char*)0x40066e
    $2 = 0x40066e "Correcto"
    (gdb) p (char*)0x400677
    $3 = 0x400677 "Incorrecto"
    (gdb)

*Obsérvese que se puede especificar el tipo al usar el comando print*
Por tanto una primera solución podría ser nopear el salto condicional
para que nunca saltara, así siempre imprimiría el mensaje ”Correcto”.
Otra solución más bonita es mirar con que compara y generar el serial
correcto. Justo después de la llamada a scanf hay una secuencia de
comparaciones. Para mirar exactamente que valores mueve pondremos un
breakpoint en el scanf.

     0x0000000000400589 <+35>:	call   0x400460 <__isoc99_scanf@plt>

En este caso en la dirección 0x400589 o lo que es lo mismo, en
\*main + 35.

     (gdb) b *main+35
     Breakpoint 2 at 0x400589
     (gdb) 

Una vez se ejecute la instrucción

    0x000000000040058e <+40>:	movzx  eax,BYTE PTR [rbp-0x10]

podemos mirar que valor tiene el registro EAX. En este caso el serial
introducido fue ”123456”. Como vemos si imprimimos el valor de EAX
equivale a la primera letra del serial, en este caso un ”1” como se
puede ver.

    (gdb) p (char)$rax
    $20 = 49 '1'
    (gdb)

Luego vemos que compara esa primera letra y si no es igual a 0x35 (”5”)
salta al mensaje de ”Incorrecto”.

    0x0000000000400592 <+44>:	cmp    al,0x35
    => 0x0000000000400594 <+46>:	jne    0x4005be <main+88>

*0x4005be es la dirección donde se guarda el mensaje de ”Incorrecto”.* Ya
tenemos el primer carácter del serial, ”5”.\
Haciendo lo mismo vemos que la siguiente comparación es contra 0x61
(letra ”a”) y la siguiente 0x37 (”7”), así que los tres primeros
caracteres son ”5a7”. Finalmente podemos ver que compara el registro dl
y el registro al.

    0x00000000004005a6 <+64>:	movzx  edx,BYTE PTR [rbp-0xd]
    0x00000000004005aa <+68>:	movzx  eax,BYTE PTR [rbp-0xc]
    0x00000000004005ae <+72>:	cmp    dl,al
    0x00000000004005b0 <+74>:	jne    0x4005be <main+88>

El registro **dl** contendrá la penúltima letra a comparar y **al** la
última, por tanto los dos últimos caracteres del serial tienen que ser
los mismos. Así que ”5a700’ o ”5a7zz” serian seriales correctos.

Obteniendo ayuda
----------------

Si tenemos alguna duda lo primero que podemos hacer es consultar el manual (*man
gdb*), si queremos un resumen de las opciones podemos ejecutar *gdb
–help* o *gdb -h*. Si tenemos dudas más genéricas una vez dentro de GDB
el comando *help* puede servir. Sino podemos mirar la documentación de
GDB con el comando *info gdb*.\
Si no encontramos nada podemos ir a su web oficial
<https://www.gnu.org/software/gdb/> donde hay información sobre el canal
IRC y las listas de correo que también pueden ser de utilidad.\
GDB tiene un sistema de auto-completado de comandos (usando la tecla **TAB**
como de costumbre), suele venir muy bien para los vagos que no quieren escribir
mucho o no recuerdan como se escribía un comando.

Iniciando GDB
=============

Anteriormente ya vimos como cargar un ejecutable con GDB pero aún no
sabemos como podemos debugear un proceso en ejecución, ni que ficheros
carga al iniciar, etc. En esta sección veremos otras opciones para
depurar un programa.

Ejecutable
----------

Como vimos para cargar un ejecutable en GDB se lo podemos especificar
como parámetro. También se puede usar el comando *file* una vez iniciado
GDB.

En estos dos casos el ejecutable es cargado y los símbolos son leídos.
Es equivalente a usar el flag **-se** que lee los símbolos del archivo y
lo carga como ejecutable.\
El flag **-s** o **-symbols** solo lee los símbolos, y el flag **-e** o
**-exec** especifica que archivo se ejecutara pero no lee los símbolos.

Proceso en ejecución
--------------------

Otra opción es depurar un proceso en ejecución especificando su PID
(identificador de proceso).\
Para este ejemplo usaremos el programa **ejemplos/attach.c**. Dicho
código imprime su PID, y luego espera algún input, finalmente dice ”Bye”
y sale.

Despues de compilar y ejecutar el programas nos dice que su PID es el ”5146”
(este ID puede ser distinto). Dejamos el programa esperando el input y en otra consola
abrimos GDB donde el primer parámetro es la ruta al ejecutable y el
segundo el PID. Por defecto GDB si se le especifica un valor numérico
primero comprobara si es un proceso, si no es un proceso probara si es
un archivo ”core” (el tipo de archivo que algunos sistemas crean cuando hay
un error crítico con el estado del programa en ese instante).

    $ gdb -q attach 5146
    Reading symbols from attach...(no debugging symbols found)...done.
    Attaching to program: /tmp/codes/2/attach, process 5146
    Reading symbols from /lib/x86_64-linux-gnu/libc.so.6...(no debugging symbols found)...done.
    Reading symbols from /lib64/ld-linux-x86-64.so.2...(no debugging symbols found)...done.
    0x00007f26d5f5a8e0 in read () from /lib/x86_64-linux-gnu/libc.so.6
    (gdb) 

Como vemos primero lee el ejecutable y luego se une al PID
especificado. Ahora ya podemos trabajar como de costumbre.\

También se puede attachear al programa usando el comando *attach* (previamente
abriendo el ejecutable).
También se existen las opciones **-pid** o **-p** que a parte de unirse
(attach) al proceso que se esta ejecutando busca el ejecutable y lo lee.

Símbolos de depuración
----------------------

Los símbolos de depuración pueden estar en un mismo ejecutable o venir a
parte. Estos símbolos normalmente son stripeados (eliminados) del
binario para reducir su tamaño y/o hacer más difícil su análisis.
Para la prueba usaremos el programa **ejemplos/attach.c** y las
herramientas *strip* y *objcopy*, la primera para eliminar estos datos y
la segunda para copiarlos a otro fichero.
Primero deberemos compilar con el flag **-ggdb** como de costumbre.
Luego usaremos la herramienta *objcopy* para extraer algunos símbolo a
otro archivo. Ejecutando *objcopy –only-keep-debug attach attach.dbg* 
nos creara el archivo attach.dbg con los símbolos. Luego eliminamos los
símbolos del binario original, para esto podríamos usar la misma
herramienta (objcopy) o usar *strip*. Una vez ejecutemos el comando
*strip –strip-debug attach* ya tendremos los dos archivos.
Ahora ya podemos abrir el fichero ejecutable por una parte y cargar los
símbolos por otra. En GDB hay los flags **-s** y **-symbols** y el
comando *symbol-file* para cargar los símbolos.
Si intentamos abrir GDB son el ejecutable nos dirá que no encontró los
símbolos porque fueron eliminados del binario original.
Se podría pensar que un comando como *gdb -q attach -symbols attach.dbg*
debería leer los símbolos, pero si nos fijamos no los lee.

    gdb -q attach -symbols attach.dbg
    Reading symbols from attach...(no debugging symbols found)...done.
    (gdb)

Eso es porque implícitamente intenta leer los símbolos de depuración del
ejecutable, pero no los encuentra porque los borramos. Para que no los
intente leer del ejecutable podríamos especificar cual es el archivo
ejecutable y cual el de los símbolos. Por ejemplo ejecutando *gdb -q
-exec attach -symbols attach.dbg*. Como dijimos anteriormente el flag
**-exec** servía para especificar el ejecutable.

    gdb -q -exec attach -symbols attach.dbg
    Reading symbols from attach.dbg...done.
    (gdb)

Archivo ”core”
--------------

A veces los errores mortales generaran ”core files”. Esos ficheros
guardan una imagen del proceso cuando murió y pueden ser usados por GDB
u otras herramientas para analizar dicho ejecutable e intentar averiguar
que causo su muerte.
Muchas veces este archivo core es generado automáticamente después de un
fallo grave. Si no es generado automáticamente, se puede generar uno con
GDB usando el comando *generate-core-file* o *gcore* donde el primer
argumento es el nombre del archivo.\
Para este ejemplo usaremos el code **ejemplos/dead.cpp**. Es un simple
programa en C++ que peta porque se intenta dereferenciar un puntero
nulo.
Luego usando el flag **-c** o **-core** o usando el comando *core-file*
leemos el archivo core que en este caso se llama ”dead.core”.\
*A veces el sistema operativo no genera estos archivos cores de forma
automática, en este caso podemos habilitarlo escribiendo **ulimit -c
unlimited** o si estamos dentro de GDB con el comando
**generate-core-file***

    $ gdb -q dead -core dead.core
    Reading symbols from attach...(no debugging symbols found)...done.

    [New LWP 8888]
    Core was generated by `/tmp/ejemplos/dead'.
    Program terminated with signal SIGSEGV, Segmentation fault.
    #0  0x00000000004009eb in ?? ()
    (gdb)

Como vemos nos indica que se produjo un SIGSEV y en que dirección se
produjo. Luego podemos desensamblar el punto donde se produjo el error y mirar que
estaba haciendo.

    (gdb) disas main
    Dump of assembler code for function main:
    ...
    0x00000000004009d3 <+61>:	mov    rax,QWORD PTR [rbp-0x8]
    0x00000000004009d7 <+65>:	mov    rdi,rax
    0x00000000004009da <+68>:	call   0x400800 <_ZdlPv@plt>
    0x00000000004009df <+73>:	mov    QWORD PTR [rbp-0x8],0x0
    0x00000000004009e7 <+81>:	mov    rax,QWORD PTR [rbp-0x8]
    => 0x00000000004009eb <+85>:	movzx  eax,BYTE PTR [rax]
    ...
    (gdb) 

Como vemos primero copió **0x0** a **rbp-0x8** y luego lo copió a
**rax**. RAX ahora vale 0, así que al intentar obtener el valor apuntado
por RAX dio el fallo de segmentación.

Archivos de GDB
---------------

Al iniciar, GDB lee una serie de archivos de configuración que pueden
servir, por ejemplo, para configurar que use la sintaxis de intel y
no la de at&t. GDB por defecto leera la configuración de `$HOME/.gdbinit` que se
procesara antes que los flags. Obviamente este archivo de configuración es global
para el usuario. Si el archivo `gdibit` esta en la carpeta actual, este fichero se leerá
después de procesar las opciones del comando, con la excepción de **-x**
y **-ex** que se procesan después. Depende del caso puede existir otro
archivo que se carga antes que los comandos, que los otros ficheros y
que es global para todo el sistema.
Si preferimos la sintaxis intel podemos escribir el comando que dijimos
anteriormente (*set disassembly-flavor intel*) en el archivo
`$HOME/.gdbinit`. Más tarde veremos como podemos jugar más con estos
ficheros.

En caso de que algún momento no queramos que estos archivos sean leídos
podemos usar el flag **-nh** para omitir el archivo `$HOME/.gdbinit` o
los flags **-n** o **-nx** que omiten todos los archivos `.gdbinit`

Breakpoints
===========

Un breakpoint es una orden que damos al depurador para que nos pause la
ejecución en un lugar concreto. GDB soporta 3 tipos de breakpoints, los
*breakpoints*, *watchpoints*, y *catchpoints*.\
Los *breakpoints* detienen el programa cuando llega a un cierto punto,
estos pueden tener condiciones. Los *watchpoints* (o *data breakpoints*)
detienen el programa cuando se accede a una cierta zona de datos, por
ejemplo cuando una variable es leída y/o escrita. Los *catchpoints*
detienen el programa cuando se da un evento concreto, por ejemplo al
cargar una librería.\
Para breakpoint que pongamos GDB le pondrá un número. Este identificador
numérico servirá para operar sobre los breakpoints por ejemplo para deshabilitarlos,
borrarlos, etc.
Para listar todos los breakpoints se puede usar el comando *info
breakpoints*, para listar solo los watchpoints el comando a usar sería
*info watchpoints*.

Breakpoints
-----------

El código que se usara esta en **ejemplos/breakpoints.c**.

Hay 2 formas de poner estos breakpoints, por software o por hardware.
Los ”software breakpoints” son más flexibles y tienen menos limitaciones
que los ”hardware breakpints” pero son más fáciles de detectar. Las
técnicas antid-debug las veremos más adelante.
*NOTA: Si solo decimos ”breakpoints” nos referimos a software
breakpoints*

La forma más rápida de colocar un breakpoint es usando el comando
*break* tal y como vimos anteriormente. El argumento puede ser el nombre
de una función, un número de línea o una dirección. Si no se especifica
ningún parámetro pondrá el breakpoint en la siguiente instrucción a ser
ejecutada. También se puede especificar una condición (conditional breakpoint),
por ejemplo si quisiéramos poner un breakpoint al llegar a la interacción número
100 podríamos hacer algo tan simple como *break \*main+21 if i==100*. Si luego 
imprimimos la información de los breakpoints veremos.

    (gdb) i b
    Num     Type           Disp Enb Address            What
    1       breakpoint     keep y   0x00000000004004cb in main at breakpoints.c:5
           stop only if i == 100
    (gdb) 

Si no tuviéramos esa información de depuración no podríamos referencias
directamente la variable y deberíamos hacer algo como:

    (gdb) disas main
    Dump of assembler code for function main:
    0x00000000004004b6 <+0>:	push   rbp
    0x00000000004004b7 <+1>:	mov    rbp,rsp
    0x00000000004004ba <+4>:	mov    DWORD PTR [rbp-0x4],0x0
    0x00000000004004c1 <+11>:	jmp    0x4004c7 <main+17>
    0x00000000004004c3 <+13>:	add    DWORD PTR [rbp-0x4],0x1
    0x00000000004004c7 <+17>:	cmp    DWORD PTR [rbp-0x4],0x63
    0x00000000004004cb <+21>:	jle    0x4004c3 <main+13>
    0x00000000004004cd <+23>:	mov    eax,0x0
    0x00000000004004d2 <+28>:	pop    rbp
    0x00000000004004d3 <+29>:	ret    
    End of assembler dump.
    (gdb) b *main+21 if *(int*)($rbp-0x4) == 100
    Breakpoint 1 at 0x4004cb
    (gdb) 

Como vemos la variable **i** en la máquina equivale a la dirección
(\$rbp-0x4). Puesto que esa dirección apuntara a un int, lo podemos
”castear”. Como no queremos comparar con la dirección sino con el valor
al que apunta usamos el operador **\***. Así que la comparación
finalmente queda *\*(int\*)(\$rbp-0x4) == 100*.\
También podemos añadir la condición después de colocar el breakpoint con
el comando *condition*. Este comando tiene 2 argumentos, el primero es
el número de breakpoint, y el segundo la condición.

    (gdb) break *main+21
    Breakpoint 1 at 0x4004cb
    (gdb) condition 1 *(int*)($rbp-0x4) == 100
    (gdb) i b
    Num     Type           Disp Enb Address            What
    1       breakpoint     keep y   0x00000000004004cb <main+21>
    stop only if *(int*)($rbp-0x4) == 100
    (gdb)

Para colocar breakpoints temporales podemos usar el comando *tbreak*.
Este comando funciona igual que el comando break solo que una vez usado
se borra automáticamente.\
El comando *hbreak* permite colocar breakpoints por hardware. Al
depender del hardware el número de breakpoints es limitado, y en algunos
casos puede que el hardware no lo permita. El uso de este comando es
igual que en los dos anteriores casos.

Si el comando *tbreak* eran breakpoints temporales y el comando *hbreak*
eran breakpoints por hardware, el comando *thbreak* son breakpoints
temporales por hardware.
El comando *rbreak* es algo distinto. Este comando permite poner varios
breakpoints incondicionales que cumplan un regexp (como el que usa
”grep”). Por ejemplo para poner un breakpoint en todas las funciones que
empiecen por ”str” se puede usar el comando *rbreak \^str*. Si queremos
limitar la búsqueda a un archivo podemos especificar el nombre del
archivo seguido de dos puntos antes del ”regexp”, por ejemplo *rbreak
archivo.c:\^str*

Watchpoints
-----------

El código que se usara esta en **ejemplos/watchpoints.c**.

Hay 3 formas de colocar estos watchpoints. Se puede poner un watchpoint
al leer una variable, al escribir en ella o en cualquiera los dos casos.
GDB intentara colocar hardware breakpoints en vez de software
breakpoints para los watchpoints si es posible.

Para poner un watchpoint cuando una variable sea escrita (modificada)
usaremos la orden *watch* seguida de una expresión. Este comando
adicionalmente acepta 2 parámetros, el número de hilo y una máscara. El
threadnum (número de hilo) sirve para especificar que solo vigile un
thread en especifico, este caso solo funciona con los hardware
breakpoints. La máscara sirve principalmente para poder poner varios
watchpoints de forma simultanea, aunque en este texto no le daremos
demasiada importancia ya que los PCs de ir por casa (arquitecturas x86)
no lo soportan.\
Por ejemplo si tenemos la variable **i** y a **x**, que en el inicio
valen 0 y ponemos un *watch i\*x*

    (gdb) watch i*x
    Hardware watchpoint 2: i*x
    (gdb) c
    Continuing.
    123
    Hardware watchpoint 2: i*x

    Old value = 0
    New value = 12300
    0x00007ffff7a8f5ea in _IO_vfscanf () from /lib/x86_64-linux-gnu/libc.so.6
    (gdb) 

Podemos ver que cuando el programa entra en el scanf y nos pide el nuevo
valor de **x**, si le introducimos algo distinto que cero saltara el
watchpoint y nos avisara del valor antiguo y del nuevo. Si hubiéramos
ingresado un 0, i\*0 sigue siendo 0 y no habría saltado.\
También podemos poner watchpoints en valores booleanos, por ejemplo

    (gdb) watch i*x==500
    Hardware watchpoint 2: i*x==500
    (gdb) c
    Continuing.
    5
    Hardware watchpoint 2: i*x==500

    Old value = 0
    New value = 1
    0x00007ffff7a8f5ea in _IO_vfscanf () from /lib/x86_64-linux-gnu/libc.so.6
    (gdb)

Como vemos el valor antiguo era 0 (falso) y el nuevo 1 (cierto) ya que
al acabar el bucle la variable **i** vale 100, y **x** le cambiamos el
valor por 5.
Para poner un watchpoint al leer un valor usaremos el comando *rwatch* y
para ponerlo en lectura o escritura usaremos *awatch*. El funcionamiento
es idéntico al caso anterior.
El programa de ejemplo que vamos a usar se puede encontrar en
**ejemplos/rwatch.cpp** y lo podemos compilar con **g++ rwatch.cpp -o
rwatch -std=c++11 -pthread**. Este programa tiene un error por el que la
mayoría de veces se quedara en un bucle infinito.

Una vez tengamos cargado el ejecutable en GDB podemos poner un break en
la función main y ir avanzando en la ejecución para ver como se crean
los dos threads.

    Reading symbols from rwatch...(no debugging symbols found)...done.
    (gdb) tbreak main
    Temporary breakpoint 1 at 0x400ef7
    (gdb) r
    Starting program: /home/david/Soft/Proyectos/fennix-GDB/src/ejemplos/rwatch 
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Temporary breakpoint 1, 0x0000000000400ef7 in main ()
    (gdb) nexti
    ...
    (gdb) 
    [New Thread 0x7ffff7008700 (LWP 10771)]
    ....
    (gdb) 
    [New Thread 0x7ffff6807700 (LWP 10815)]
    0x0000000000400f1e in main ()
    (gdb)

Aquí vemos dos valores LWP, estos números son identificadores de la
tarea. Si ejecutamos el comando **thread apply all bt** podremos ver el
backtrace de todos los threads para mirar que función estaban
ejecutando, en este caso de los 3.

    (gdb) thread apply all bt

    Thread 3 (Thread 0x7ffff6807700 (LWP 10815)):
    #0  0x00007ffff73b42bd in nanosleep () from /lib64/libpthread.so.0
    #1  0x000000000040157c in void std::this_thread::sleep_for<long, std::ratio<1l, 1000l> >(std::chrono::duration<long, std::ratio<1l, 1000l> > const&) ()
    #2  0x0000000000400ee1 in f2() ()
    #3  0x0000000000402711 in void std::_Bind_simple<void (*())()>::_M_invoke<>(std::_Index_tuple<>) ()
    #4  0x0000000000402659 in std::_Bind_simple<void (*())()>::operator()() ()
    #5  0x00000000004025d6 in std::thread::_Impl<std::_Bind_simple<void (*())()> >::_M_run() ()
    #6  0x00007ffff7b86980 in execute_native_thread_routine () from /usr/lib/gcc/x86_64-pc-linux-gnu/4.9.3/libstdc++.so.6
    #7  0x00007ffff73ab644 in start_thread () from /lib64/libpthread.so.0
    #8  0x00007ffff70f0edd in clone () from /lib64/libc.so.6

    Thread 2 (Thread 0x7ffff7008700 (LWP 10771)):
    #0  0x00007ffff73b42bd in nanosleep () from /lib64/libpthread.so.0
    #1  0x000000000040157c in void std::this_thread::sleep_for<long, std::ratio<1l, 1000l> >(std::chrono::duration<long, std::ratio<1l, 1000l> > const&) ()
    #2  0x0000000000400e9f in f1() ()
    #3  0x0000000000402711 in void std::_Bind_simple<void (*())()>::_M_invoke<>(std::_Index_tuple<>) ()
    #4  0x0000000000402659 in std::_Bind_simple<void (*())()>::operator()() ()
    #5  0x00000000004025d6 in std::thread::_Impl<std::_Bind_simple<void (*())()> >::_M_run() ()
    #6  0x00007ffff7b86980 in execute_native_thread_routine () from /usr/lib/gcc/x86_64-pc-linux-gnu/4.9.3/libstdc++.so.6
    #7  0x00007ffff73ab644 in start_thread () from /lib64/libpthread.so.0
    #8  0x00007ffff70f0edd in clone () from /lib64/libc.so.6

    Thread 1 (Thread 0x7ffff7fc7740 (LWP 10688)):
    #0  0x0000000000400f1e in main ()
    (gdb)

Luego podríamos poner un breakpoint en la variable *global* en el thread
3 (f2) y ver que se esta leyendo.

    (gdb) rwatch global thread 3
    Hardware read watchpoint 2: global
    (gdb) c
    Continuing.
    [Thread 0x7ffff7008700 (LWP 10771) exited]
    [Switching to Thread 0x7ffff6807700 (LWP 10815)]

    Thread 3 "rwatch" hit Hardware read watchpoint 2: global

    Value = 1
    0x0000000000400ee1 in f2() ()
    (gdb)

Como vemos f2 leyó de la variable *global* el valor 1 y esperaba un 2
para finalizar el thread.\

Catchpoints
-----------

Los catchpoints nos permiten pausar la ejecución al producirse algún
evento como una excepción de C++, una llamada a sistema, al cargar una
librería dinámica, etc. Por ejemplo con el mismo ejecutable que en el
punto anterior podríamos poner un catch al ejecutar la función *clone*
(syscall 56).

    (gdb) catch syscall 56
    Catchpoint 1 (syscall 56)
    (gdb) r
    Starting program: /home/david/Soft/Proyectos/fennix-GDB/src/ejemplos/rwatch 
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Catchpoint 1 (call to syscall 56), 0x00007ffff70f0ea1 in clone () from /lib64/libc.so.6
    (gdb) 

Eliminando y deshabilitando breakpoints
---------------------------------------

Para eliminar breakpoints de cualquiera de estos 3 tipos podemos usar la
orden *delete*. Si no se le especifica ningún argumento borrara todos
los breakpoints. Si se le especifica un número **N**, borrara el
breakpoint **N**. Si se le especifica un rango **N-M** eliminara todos
los breakpoints desde el **N** hasta el **M** (**N** y **M** incluidos):
Por ejemplo, la orden `delete 5-7` borrara los breakpoints 5, 6 y 7.

Para desactivar los breakpoints en vez de borrarlos existe el comando
`enable` y `disable`. Como parámetros se les puede especificar un rango
o el número del breakpoint. La orden `enable` también permite activar un
breakpoint para que automáticamente tras saltar sea eliminado,
deshabilitado, o bien ponerle un contador a un breakpoint para que al
cabo de X acciones (aka ”hits”) se desactive. Por ejemplo:

    (gdb) disas main
    Dump of assembler code for function main:
    0x0000000000400526 <+0>:	push   rbp
    0x0000000000400527 <+1>:	mov    rbp,rsp
    0x000000000040052a <+4>:	sub    rsp,0x10
    0x000000000040052e <+8>:	mov    DWORD PTR [rbp-0x8],0x0
    0x0000000000400535 <+15>:	mov    DWORD PTR [rbp-0x4],0x0
    0x000000000040053c <+22>:	jmp    0x400542 <main+28>
    0x000000000040053e <+24>:	add    DWORD PTR [rbp-0x4],0x1
    0x0000000000400542 <+28>:	cmp    DWORD PTR [rbp-0x4],0x63
    0x0000000000400546 <+32>:	jle    0x40053e <main+24>
    ...
    0x000000000040055e <+56>:	mov    eax,0x0
    0x0000000000400563 <+61>:	leave  
    0x0000000000400564 <+62>:	ret    
    End of assembler dump.
    (gdb) i b
    Num     Type           Disp Enb Address            What
    1       breakpoint     keep y   0x0000000000400546 in main at watchpoints.c:6
    2       breakpoint     keep y   0x0000000000400542 in main at watchpoints.c:6
    3       breakpoint     keep y   0x000000000040053e in main at watchpoints.c:6
    (gdb)

Podemos poner que el breakpoint 1 se desactive al cabo de 5 ”hits”, el 2
se deshabilite en el primer ”hit” y el 3 se borre al primer ”hit”.

    (gdb) enable once 2
    (gdb) enable delete 3
    (gdb) enable count 5 1
    (gdb) i b
    Num     Type           Disp Enb Address            What
    1       breakpoint     dis  y   0x0000000000400546 in main at watchpoints.c:6
    disable after next 5 hits
    2       breakpoint     dis  y   0x0000000000400542 in main at watchpoints.c:6
    3       breakpoint     del  y   0x000000000040053e in main at watchpoints.c:6
    (gdb) 

Si ahora damos run y volvemos a mirar los breakpoints veremos como el 3
ya no existe y el 2 se deshabilito.

    (gdb) i b
    Num     Type           Disp Enb Address            What
    1       breakpoint     dis  y   0x0000000000400546 in main at watchpoints.c:6
    breakpoint already hit 1 time
    disable after next 4 hits
    2       breakpoint     dis  n   0x0000000000400542 in main at watchpoints.c:6
    breakpoint already hit 1 time
    (gdb)


TO BE CONTINUED
==============

Hasta aquí la primera parte. Dado que esta primera parte puede haber sido algo
aburrida, tratare de hacer una segunda parte introduciendo y profundizando sobre
más conceptos.

Para cualquier duda consultar el [manual oficial de GDB](https://sourceware.org/gdb/current/onlinedocs/gdb/)

