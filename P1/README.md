Integrantes:

- Pablo Araneda - 18639283
- Diego Bustamante - 18636713
- Christian Klempau - 18640877
- Macarena Muñoz - 18637086
- Maximiliano Narea - 17626463

Suposiciones:

- Se asume que el disco puede ser cambiado durante la ejecución. El path global se vuelve a setear y el struct de la MBT se inicializa nuevamente cada vez que s ellama a `os_mount`.

- Se asume que no existen 2 entradas válidas para un mismo `id` de partición.

- De lo anterior, se asume que al intentar escribir una nueva partición en el disco (porque la partición con ese `id` no existe) habrá en el disco una entrada con bit de validez 0.