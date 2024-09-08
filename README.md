# Proyecto Sistemas Operativos: Myshell Bachelet
Proyecto semestral de Sistemas Operativos 2024-S2, el cual consiste en el desarrollo de una interpretadora de comandos para el sistema operativo Linux.
El código para este proyecto está desarrollado en C++.

DISCLAIMER:
Este proecto requiere del sistema operativo Linux y un compilador de C++
Para compilar el archivo: g++ shell.cpp -o myshell
Y para ejecutarlo: ./myshell

Ejecutar comandos:
1. La shell imprime una prompt que espera los comandos ingresados por el usuario.
   De ser un comando válido, lo ejecuta; de no serlo, lo rechaza e imprime un mensaje de error.
   Si el comando ingresado es un string vacío, la shell continúa con el prompt de espera.
2. La shell acepta comandos con pipes, haciendo uso de un caracter "|" separando cada comando.
3. La shell acepta el comando "end" para terminar.
4. La shell acepta el comando "wah".
5. El comando de recordatorio se ejecuta con "set recordatorio x", donde x es la cantidad de segundos
   que el programa espera para accionar la alarma.
6. El comando favs posee varios subcomandos.
   6.1 favs crear [ruta]/[texto.txt]: crea un archivo en donde se almacena el archivo de texto donde
       se escribirán los comandos favoritos.
   6.2 favs mostrar: despliega la lista de comandos guardados.
   6.3 favs eliminar [número1], [número2]: elimina los comandos asociados a los números.
   6.4 favs buscar cmd: busca comandos que contengan substring cmd en el archivo de favoritos y los imprime en la terminal.
   6.5 favs borrar: limpia el archivo de texto en la lista de favorito.
   6.6 favs [número] ejecutar: ejecuta el comando de número [número] en la lista.
   6.7 favs cargar: despliega los comandos en el archivo de favoritos en la terminal.
   6.8 favs guardar: guarda los comandos agregados en la iteración actual de la shell en el archivo de favoritos.
   
