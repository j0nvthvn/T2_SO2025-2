# Tarea 2 - Simulación Doom | Sistemas Operativos

## Índice

- [Tarea 2 - Simulación Doom | Sistemas Operativos](#tarea-2---simulación-doom--sistemas-operativos)
  - [Índice](#índice)
  - [Descripción del proyecto](#descripción-del-proyecto)
  - [Funcionalidades del proyecto](#funcionalidades-del-proyecto)
  - [Requisitos](#requisitos)
  - [Instalación y Compilación](#instalación-y-compilación)
  - [Uso](#uso)
  - [Formato del Archivo de Configuración](#formato-del-archivo-de-configuración)
    - [Tamaño de la Grilla](#tamaño-de-la-grilla)
    - [Formato de Héroes](#formato-de-héroes)
      - [Opción 1: Héroe único](#opción-1-héroe-único)
      - [Opción 2: Múltiples héroes (N héroes)](#opción-2-múltiples-héroes-n-héroes)
    - [Formato de Monstruos](#formato-de-monstruos)
  - [Configuraciones de Ejemplo](#configuraciones-de-ejemplo)
  - [Estructura del Proyecto](#estructura-del-proyecto)
    - [Cómo funciona](#cómo-funciona)
    - [Detalles de implementación importantes](#detalles-de-implementación-importantes)
  - [Autor](#autor)

## Descripción del proyecto

Este proyecto es una simulación del clásico videojuego Doom implementada en C con programación concurrente. La simulación utiliza threads para modelar el comportamiento de héroes y monstruos en un grid 2D, donde los héroes siguen rutas predefinidas mientras los monstruos los detectan, alertan a otros y los atacan.

El proyecto implementa dos versiones:

1. **Versión básica**: 1 héroe (1 thread) y M monstruos (M threads)
2. **Versión extendida**: N héroes (N threads) y M monstruos (M threads), cada héroe con su ruta independiente

## Funcionalidades del proyecto

- `Múltiples héroes (N threads)`: Cada héroe tiene su propio camino independiente y atributos personalizados (HP, daño, rango de ataque)
- `Sistema de monstruos (M threads)`: IA con comportamiento de alerta, persecución mediante distancia Manhattan y combate
- `Grid 2D`: Sistema de coordenadas con detección por rangos de visión y ataque
- `Combate sincronizado`: Sistema de turnos alternados entre entidades usando semáforos
- `Sincronización thread-safe`: Uso de mutexes, semáforos y variables atómicas para evitar race conditions
- `Parser de configuración`: Lee archivos de configuración con diferentes formatos (1 héroe o N héroes)

## Requisitos

- Sistema operativo **Linux/UNIX**
- Compilador **GCC** con soporte para C11
- Biblioteca **POSIX threads** (pthread)
- Make (para compilación automatizada)
- (Opcional) Valgrind — herramienta para detectar memory leaks y errores de memoria (más información para instalación: [valgrind.org](https://valgrind.org/docs/manual/hg-manual.html)).

## Instalación y Compilación

1. Descarga los archivos
2. Navega al directorio del proyecto:

   ```bash
   cd T2_SO2025-2
   ```

3. Compila el proyecto:

   ```bash
   make
   ```

Otros comandos útiles:

```bash
# Limpiar archivos compilados
make clean

# Ejecutar con configuración por defecto
make test

# Verificar memory leaks con Valgrind
make valgrind

# Detectar race conditions con Helgrind
make helgrind
```

## Uso

```bash
./doom_simulator <archivo_configuracion>
```

**Ejemplos:**

```bash
./doom_simulator config.txt
./doom_simulator ejemplos/ejemplo1.txt
./doom_simulator ejemplos/ejemplo2.txt
```

## Formato del Archivo de Configuración

### Tamaño de la Grilla

```text
GRID_SIZE <ancho> <alto>
```

### Formato de Héroes

#### Opción 1: Héroe único

```text
HERO_HP 150
HERO_ATTACK_DAMAGE 20
HERO_ATTACK_RANGE 3
HERO_START 2 2
HERO_PATH (3,2)(4,2)(5,2)(6,2)
```

#### Opción 2: Múltiples héroes (N héroes)

```text
HERO_1_HP 220
HERO_1_ATTACK_DAMAGE 25
HERO_1_ATTACK_RANGE 3
HERO_1_START 3 3
HERO_1_PATH (4,3)(5,3)(6,3)

HERO_2_HP 200
HERO_2_ATTACK_DAMAGE 22
HERO_2_ATTACK_RANGE 4
HERO_2_START 10 10
HERO_2_PATH (11,10)(12,10)(13,10)
```

### Formato de Monstruos

```text
MONSTER_COUNT 3

MONSTER_1_HP 50
MONSTER_1_ATTACK_DAMAGE 10
MONSTER_1_VISION_RANGE 5
MONSTER_1_ATTACK_RANGE 1
MONSTER_1_COORDS 8 4

MONSTER_2_HP 60
MONSTER_2_ATTACK_DAMAGE 12
MONSTER_2_VISION_RANGE 6
MONSTER_2_ATTACK_RANGE 1
MONSTER_2_COORDS 15 20
```

## Configuraciones de Ejemplo

El directorio [ejemplos/](ejemplos/) contiene tres configuraciones de prueba:

- **ejemplo1.txt**: Héroe único, 24 monstruos, grilla grande (60×40), camino extenso
- **ejemplo2.txt**: 3 héroes con caminos independientes, 24 monstruos, grilla 60×40
- **ejemplo3.txt**: 2 héroes, 12 monstruos, grilla pequeña (20×15)

## Estructura del Proyecto

El proyecto está organizado en los siguientes archivos:

```text
├── main.c              # Punto de entrada y orquestación de threads
├── config.h            # Estructuras de datos y configuración global
├── parser.c/h          # Parseo de archivos de configuración
├── hero.c/h            # Comportamiento y thread de héroes
├── monster.c/h         # Comportamiento y thread de monstruos
├── combat.c/h          # Sistema de combate por turnos
├── simulation.c/h      # Utilidades (distancia Manhattan, movimiento)
├── sync.c/h            # Inicialización de primitivas de sincronización
├── Makefile            # Sistema de compilación
├── config.txt          # Configuración por defecto
└── ejemplos/           # Archivos de configuración de ejemplo
```

### Cómo funciona

El programa funciona de la siguiente manera:

1. **Inicialización**: El `main.c` lee el archivo de configuración usando `parser.c`, inicializa las primitivas de sincronización (`sync.c`) y crea los threads para héroes y monstruos.

2. **Movimiento de héroes**: Cada héroe se mueve por su ruta predefinida hasta completarla o morir. Cuando encuentra monstruos en su rango de ataque, se detiene y combate hasta eliminarlos o morir.

3. **Comportamiento de monstruos**:
   - **Estado pasivo**: Los monstruos inician esperando en su posición inicial
   - **Alerta**: Cuando un héroe entra en su rango de visión, el monstruo se alerta
   - **Propagación de alerta**: El monstruo alerta a todos los otros monstruos dentro de su rango de visión
   - **Persecución**: Una vez alertado, el monstruo se mueve hacia el héroe más cercano usando la ruta más corta (distancia Manhattan)
   - **Ataque**: Cuando el héroe está en su rango de ataque, inicia el combate

4. **Sistema de combate**: Cuando una entidad ataca a otra, se inicia un combate por turnos controlado por semáforos. Los ataques se alternan hasta que uno de los dos muere (HP llega a 0).

5. **Sincronización**:
   - **Mutexes**: Protegen el acceso a la grilla y al sistema de combate
   - **Semáforos**: Controlan los turnos de combate y el contador de monstruos activos
   - **Variables atómicas**: Manejan el estado global de la simulación para evitar race conditions

6. **Finalización**: La simulación termina cuando todos los héroes completan su ruta/mueren o todos los monstruos son eliminados.

### Detalles de implementación importantes

- **Distancia Manhattan**: Se utiliza para calcular la ruta más corta de los monstruos hacia los héroes: `distancia = |x1 - x2| + |y1 - y2|`
- **Detección por rangos**: Los monstruos tienen un rango de visión para detectar héroes y un rango de ataque menor para iniciar combate
- **Alerta en cadena**: Un monstruo puede alertar a otros, creando una reacción en cadena
- **Héroes se detienen al combatir**: Cuando un héroe encuentra monstruos en su rango de ataque, deja de moverse hasta eliminarlos todos o morir

## Autor

Jonathan Flores - Sistemas Operativos 2025

