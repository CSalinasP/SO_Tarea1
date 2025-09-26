# SO_Tarea1
Repositorio que contiene la tarea 1 para el ramo "Sistemas Operativos".

**Leer informe con los detalles sobre como se hizo la Tarea.**

**Integrantes**:

    Carlos Sebastián Salinas Pereira
    (Nadie más)

**Como usar el programa respecto a la Parte 1:**

    Ejecutar "MiShell.exe" en la terminal, en mi caso sería: "./MiShell.exe".
    Luego escribir comandos como si fuera una Shell.

**Como usar el programa respecto a la Parte 2:**

    Luego de ejecutar "MiShell.exe" en la terminal, hay 3 formas de usar el programa:

    1) "miprof ejec <comando> <args>"
    2) "miprof ejec <tiempoMaximo> <comando> <args>"
    3) "miprof ejecsave <archivo>.txt <comando> <args>"

    Ejemplo: "miprof ejecsave test1.txt echo hola"

    El programa reconocerá cuando se usa tiempoMaximo o no, y reconocerá cuando se usa ejec o ejecsave.

**Notas**

    El ejecutable "MiShell.exe" se compiló usando: "g++ main.cpp funciones.cpp -o MiShell.exe".

    Puede usar los archivos "test1.txt", "test2.txt" y "ejecsave.txt" para probar la Shell.

    ejecsave puede crear nuevos archivos con tal solo escribiendo el nombre de un archivo que no existe.
    
    "ejecsave + tiempoMaximo" no está implementado ya que no lo pide el enunciado de la tarea.


