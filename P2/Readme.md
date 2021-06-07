# Integrantes:

- Pablo Araneda - 18639283
- Diego Bustamante - 18636713
- Christian Klempau - 18640877
- Macarena Muñoz - 18637086
- Maximiliano Narea - 17626463


# Protocolos
## cliente -> servidor:
### ID
<table>
<tr>
<th>numero byte</th>
<th>Uso</th>
</tr>

<tr>
<td>0</td>
<td>0 si es primera conexión, de ser 0, se ignoran las otras entradas, 1 e.o.c.</td>
</tr>

<tr>
<td>1-2</td>
<td>id jugador</td>
</tr>

<tr>
<td>3</td>
<td>Si es mounstro, en caso de que sea verdad, se ignora el id del jugador</td>
</tr>

<tr>
<td>4</td>
<td>1 si se trata de entregar el nombre del usuario</td>
</tr>

<tr>
<td>6</td>
<td>1 si se se trata de un mensaje de desconexión (desconectarse del servidor), 0 e.o.c.</td>
</tr>


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
<th>numero byte</th>
<th>Uso</th>
</tr>

<tr>
<td>0</td>
<td>1 si se requiere input del usuario como respuesta, 0 e.o.c.</td>
</tr>

<tr>
<td>1</td>
<td>1 si se trata de conexión (conectar el cliente al servidor), 0 e.o.c.</td>
</tr>

<tr>
<td>2-3</td>
<td>0 si se trata de antes de una partida, 1 si se inicia la partida, 2 si es dentro de la partida, 3 si es de termino de la partida</td>
</tr>

<tr>
<td>4</td>
<td>1 si se se trata de un envío de imágen, 0 e.o.c.; En caso de enviar una imágen, los siguientes 3 bytes indican el numero de imágen que se está enviando</td>
</tr>


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
Texto que se deberá mostrar en consola del cliente. O la información de la imágen