# Integrantes:

- Pablo Araneda - 18639283
- Diego Bustamante - 18636713
- Christian Klempau - 18640877
- Macarena Muñoz - 18637086
- Maximiliano Narea - 17626463

# Ejecución
* Ejecutar `make` dentro de las carpetas `client` y `server`.
* Ejecutar el servidor como se especifica en el enunciado.
* Ejecutar el cliente como se especifica en el enunciado utilizando los mismos argumentos que con el servidor.
* Seguir las instrucciones de la interfaz.

# Supuestos y Consideraciones
* Reprobatón-9000 afecta tanto al recibir como dar healing (en ambos casos se disminuye la cantidad a la mitad, por lo que si están ambas, se reduce al cuarto).
* Duración de efectos se mide en rondas ( https://github.com/IIC2333/Foro-2021-1/discussions/282 ).
* Si alguien que esta afectado por un tipo de veneno es afectado por el otro tipo de veneno, se sobreescribe.
* El monstruo ataca un objetivo al azar.
* El ID se usa para no enviar más de un tipo de información en el payload, por lo que sí se envía un tipo de información, y sabe quién la recibe quién es, no es necesario enviar todo en el ID (Esto es solo para justificar el motivo de que el cliente usa solo un bit del ID, ya que todo texto de respuesta, en caso de ser necesaria, se envía en el payload).
* Asumimos que en el caso donde el monstruo intente saltar dos veces seguidas, el turno se consume y se imprime un mensaje.
* El envenenamientod del ataque Espina Venenosa hace su efecto al final del turno, y en caso de tratar de envenar al ya envenenado, se realiza 500 de daño extra.
* Destello regenerador hace el heal según el daño realizado al enemigo (inlcuyendo bufs, entre otras cosas).
* El jugador al rendirse queda como expectador.
* El número de habilidad en consola parte desde el 1, pero en código de 0 y se hace la traducción.
* Los status que se utilizan en los pasivos decrementan su contador al fin del turno.

# Protocolos
## cliente -> servidor:
### ID
<table>
<tr>
<th>numero bit</th>
<th>Uso</th>
</tr>

<!-- <tr>
<td>0</td>
<td>0 si es primera conexión, de ser 0, se ignoran las otras entradas, 1 e.o.c.</td>
</tr> -->
<!-- 
<tr>
<td>1-3</td>
<td>id jugador</td>
</tr> -->

<!-- <tr>
<td>4</td>
<td>1 si se trata de entregar el nombre del usuario</td>
</tr> -->

<!-- <tr>
<td>5</td>
<td>1 para indicar que se recibió algo del servidor (para el caso de indicar que se recibió un trozo de un string largo o imágen), 0 en otro caso.</td>
</tr> -->


<!-- <tr>
<td>6</td>
<td>1 si se se trata de un mensaje de desconexión (desconectarse del servidor), 0 e.o.c.</td>
</tr> -->


<tr>
<td>7</td>
<td>1 si se enviarán varios mensajes. Es 0 si es el último de varios o si se envía uno solo</td>
</tr>
</table>

### Payload
Solo un byte indicando el char de respuesta al input es lo esperado. Se puede enviar un string en caso de ser necesario (nombre del jugador).

## servidor -> cliente:
### ID
<table>
<tr>
<th>numero bit</th>
<th>Uso</th>
</tr>

<tr>
<td>0</td>
<td>1 si se requiere input del usuario mas largo que un solo char como respuesta, 0 e.o.c.</td>
</tr>

<tr>
<td>1</td>
<td>1 si se desea un char como input del usuario, 0 e.o.c.</td>
</tr>

<tr>
<td>2</td>
<td>1 si UNICAMENTE se quiere que el cliente muestre texto en su pantalla 0 e.o.c.</td>
</tr>

<!-- <tr>
<td>1</td>
<td>1 si se trata de conexión (conectar el cliente al servidor), 0 e.o.c.</td>
</tr> -->

<!-- <tr>
<td>2-3</td>
<td>0 si se trata de antes de una partida, 1 si se inicia la partida, 2 si es dentro de la partida, 3 si es de termino de la partida</td>
</tr> -->

<tr>
<td>3</td>
<td>1 si se se trata de enviar el nombre del archivo a guardar posteriormente una imágen. 0 e.o.c.</td>
</tr>

<tr>
<td>4</td>
<td>1 si se se trata de un envío de imágen, 0 e.o.c.</td>
</tr>

<!-- 
<tr>
<td>5</td>
<td>1 para indicar que se recibió algo del servidor (para el caso de indicar que se recibió un trozo de un string largo o imágen), 0 en otro caso.</td>
</tr> -->


<tr>
<td>6</td>
<td>1 si se se trata de un mensaje de desconexión (desconectar al cliente), 0 e.o.c.</td>
</tr>

<tr>
<td>7</td>
<td>1 si se enviarán varios mensajes. Es 0 si es el último de varios o si se envía uno solo</td>
</tr>
</table>

### Payload
Texto que se deberá mostrar en consola del cliente. O los bytes de la imagen.
