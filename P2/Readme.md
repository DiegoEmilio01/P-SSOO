# Integrantes:

- Pablo Araneda - 18639283
- Diego Bustamante - 18636713
- Christian Klempau - 18640877
- Macarena Muñoz - 18637086
- Maximiliano Narea - 17626463

# Supuestos
* personajes solo son afectados por la ultima habilidad que les afecta algun stat o alguna alteración a sus status.
* El mounstro ataca un objetivo al azar.
* El ID se usa para no enviar mas de un tipo de información en el payload, por lo que si se envía un tipo de información, y sabe quien la recibe quien es, no es necesarioenviar todo en el ID (Esto es solo para justificar el motivo de que el cliente usa solo un bit del ID, ya que todo texto de respuesta, en caso de ser necesaria, se envía en el payload)

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
Texto que se deberá mostrar en consola del cliente. O los bytes de la imágen