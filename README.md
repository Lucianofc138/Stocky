# STOCKY - PROYECTO ELO328, Grupo 4, 2020 S2
## Participantes
En esta tarea participaron:
- Luciano Flores
- Martín Miranda
- Tomás Muñoz 
- Francisco Romero
                            
## Descripcion

En este proyecto estamos desarrollando una herramienta para supermercados o establecimientos similares para que puedan *trackear* el *stock* o falta del mismo, entregando alertas a los reponedores cuando falte un producto en un pasillo o bien cuando haya alguno fuera de lugar.

## Para usar...

0) Se necesita tener instalado OpenCV:
    - Se deben incluir librerias de colaboradores no libres (xfeatures2d)
1) Clonar el repositorio o descargar directamente
2) Compilar con:
``` cmake -D WITH_CUDA=OFF -S . -B build && cmake --build build ```
3) Correr incluyendo ruta del video y json a utilizar (los trae por defecto si no se rellena)

## Funcionamiento actual

<img src= "demo_images/stocky_demo.png" width=500>