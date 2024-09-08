<head>
   <img src="https://normasgraficas.udec.cl/sites/default/files/marcaderecha.png" width="300" height="120">
</head>
<body>
   <h1 align="center"><b></b>Proyecto Sistemas Operativos: Myshell Bachelet</b></h1>
   Proyecto semestral de <i>Sistemas Operativos 2024-S2</i>, el cual consiste en el desarrollo de una interpretadora de comandos para el sistema operativo Linux.
   El código para este proyecto está desarrollado en C++.
   <h4 align="right"><i>
      Tomás Contreras<br>
      Claudia Cser<br>
      Daniela Novoa<br>
      Jorge Santis<br>
   </i></h4>

   <h3 align="center"><ins>Disclaimer:</ins></h3>
   Este proyecto requiere del sistema operativo <em>Linux</em> y un compilador de <em>C++</em><br>
   Para compilar el archivo: <strong>g++ shell.cpp -o myshell</strong><br>
   Y para ejecutarlo: <strong>./myshell</strong><br>

   <h3 align="center"><ins>Ejecutar comandos:</ins></h3>
   1. La shell imprime una prompt que espera los comandos ingresados por el usuario.<br>
      De ser un comando válido, lo ejecuta; de no serlo, lo rechaza e imprime un mensaje de error.<br>
      Si el comando ingresado es un string vacío, la shell continúa con el prompt de espera.<br>
   2. La shell acepta comandos con <i>pipes</i>, haciendo uso de un caracter "<i>|</i>" separando cada comando.<br>
   3. La shell acepta el comando "<i>end</i>" para terminar.<br>
   4. La shell acepta el comando "<i>wah</i>".<br>
   5. Recordatorio se ejecuta con "<i>set recordatorio x</i>", donde x es la cantidad de segundos que el programa espera para activar la alarma.<br>
   6. El comando "<i>favs</i>" posee varios subcomandos.<br>
   <ul>
      6.1 <b>favs crear <i>[ruta]</i>/<i>[texto.txt]</i></b>: crea un archivo en donde se almacena el archivo de texto donde se escribirán los comandos favoritos.<br>
      6.2 <b>favs mostrar</b>: despliega la lista de comandos guardados.<br>
      6.3 <b>favs eliminar <i>[número1]</i>, <i>[número2]</i></b>: elimina los comandos asociados a los números.<br>
      6.4 <b>favs buscar cmd</b>: busca comandos que contengan substring <i>cmd</i> en el archivo de favoritos y los imprime en la terminal.<br>
      6.5 <b>favs borrar</b>: limpia el archivo de texto en la lista de favorito.<br>
      6.6 <b>favs <i>[número]</i> ejecutar</b>: ejecuta el comando de número <i>[número]</i> en la lista.<br>
      6.7 <b>favs cargar</b>: despliega los comandos en el archivo de favoritos en la pantalla.<br>
      6.8 <b>favs guardar</b>: guarda los comandos agregados en la iteración actual de la shell en el archivo de favoritos.<br>
   </ul>

   <h3 align="center"><ins>Observaciones adicionales:</ins></h3>
   La shell guarda cualquier tipo de comando del tipo <i>pipe</i>, independiente de si se pudo ejecutar o no.<br>
   Se cree que este error tiene que ver con la edición de hebras distintas a la actual.
</body>
   
