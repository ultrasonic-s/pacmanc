#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//////////// JUEGO
/// Zona de juego (playfield)
#define H 24
#define W 28
#define CELL_SIZE 20 // Tamaño de los cuadraditos
#define CANT_FANTASMA 4
#define DURACION_POWER 50
#define DURACION_ESPERA 25
#define TIMER_SCALE 10
#define TIEMPO_RESPAWN_FRUTAS 45

char playfield[H][W];

// Los niveles, estos son los que pueden aparecer.
const char nivel1[H][W] = {
    {"############################"},
    {"#............##............#"},
    {"#.####.#####.##.#####.####.#"},
    {"#*####.#####.##.#####.####*#"},
    {"#..........................#"},
    {"#.####.##.########.##.####.#"},
    {"#......##....##....##......#"},
    {"######.##### ## #####.######"},
    {"     #.##          ##.#     "},
    {"######.## ###--### ##.######"},
    {"      .   # HHHH #   .      "},
    {"######.## ######## ##.######"},
    {"     #.##     F    ##.#     "},
    {"######.## ######## ##.######"},
    {"#............##............#"},
    {"#.####.#####.##.#####.####.#"},
    {"#*..##........P.......##..*#"},
    {"###.##.##.########.##.##.###"},
    {"#......##....##....##......#"},
    {"#.##########.##.##########.#"},
    {"#..........................#"},
    {"############################"}};

const char nivel2[H][W] = {
    {"############################"},
    {"#......##..........##......#"},
    {"#*####.##.########.##.####*#"},
    {"#..........................#"},
    {"###.##.#####.##.#####.##.###"},
    {"  #.##.#####.##.#####.##.#  "},
    {"###.##.......##.......##.###"},
    {" ...#####.########.#####... "},
    {"###......................###"},
    {"  #.#####.###--###.#####.#  "},
    {"  #.##....# HHHH #....##.#  "},
    {"###.##.##.########.##.##.###"},
    {" ......##.....F....##...... "},
    {"###.########.##.########.###"},
    {"  #..........##..........#  "},
    {"###.####.##########.####.###"},
    {"#............P.............#"},
    {"#.####.#####.##.#####.####.#"},
    {"#.####.##....##....##.####.#"},
    {"#*####.##.########.##.####*#"},
    {"#..........................#"},
    {"############################"}};

const char nivel3[H][W] = {
    {"############################"},
    ("#......##..........##......#"),
    ("#.####.##.########.##.####.#"),
    ("#.*.##.......##.......##.*.#"),
    ("###.##.##.##.##.##.##.##.###"),
    (" ......##.##.F..##.##...... "),
    ("###.#####.########.#####.###"),
    ("#..........................#"),
    ("#.####.##.###--###.##.####.#"),
    ("#.##...##.# HHHH #.##...##.#"),
    ("#.##.####.########.####.##.#"),
    ("#............P.............#"),
    ("####.####.########.####.####"),
    (" ............##............ "),
    ("####.#######.##.#######.####"),
    ("#....##......##......##....#"),
    ("#.##.##.##.######.##.##.##.#"),
    ("#.##*...##...##...##...*##.#"),
    ("#.####..####.##.####.#####.#"),
    ("#..........................#"),
    ("############################")};

const char nivel4[H][W] = {
    {"############################"},
    ("#..........................#"),
    ("#*####.##.########.##.####*#"),
    ("#......##....##....##......#"),
    ("###.#####.##.##.##.#####.###"),
    ("   ....##.##....##.##....   "),
    ("######.##.########.##.######"),
    ("     #................#     "),
    ("   ###.##.###--###.##.###   "),
    ("   #...##.# HHHH #.##...#   "),
    ("   #.####.########.####.#   "),
    ("   #....................#   "),
    ("   #.####.########.####.#   "),
    ("   #.........##.........#   "),
    ("####.#######.##.#######.####"),
    (" ....##.......P......##.... "),
    ("####.##.##.######.##.##.####"),
    ("#*......##...##...##......*#"),
    ("#.#####.####.##.####.#####.#"),
    ("#..........................#"),
    ("############################")};

const char nivel5[H][W] = {
    {"      ################      "},
    {"      #..............#      "},
    {"      #.##.######.##.#      "},
    {"      #.##...##...##.#      "},
    {"#######.####.##.####.#######"},
    {"........##...F....##........"},
    {"###.######.######.######.###"},
    {"#*......##...##...##......*#"},
    {"#.#####.####.##.####.#####.#"},
    {"#....##..............##....#"},
    {"####.####.###--###.####.####"},
    {" .##......# HHHH #......##. "},
    {"#....####.########.####....#"},
    {"#.##.##..............##.##.#"},
    {"#.##.##.############.##.##.#"},
    {" .##.##......##......##.##. "},
    {"####.#######.##.#######.####"},
    {"#............P.............#"},
    {"#.##.##################.##.#"},
    {"#*##.#                #.##*#"},
    {"#....#                #....#"},
    {"######                ######"}};

char playfieldEntidades[H][W];
char playfieldPelotas[H][W];
char playfieldParedes[H][W];

///// Jugabilidad
int totalPelotas;
int nivel = 1;
int gameover = 0;
int contadorRespawnFrutas = 0;
const char frutas[] = {
    // Lista de frutas que pueden aparecer aleatoriamente
    'Z', // Cereza
    'f', // Frutilla
    'N', // Naranja
    'M', // Manzana
    'o', // Melon
};

struct coord
{
    int x;
    int y;
};
struct PacMan
{
    struct coord posicion;
    int vx, vy;
    int vidas;
    int powerup;
    int powerupTimer;
    int muerto;
    int puntos;
    int moviendo;
};

// Id de los fantasmas
static const char ghostChars[] = {'B', 'p', 'I', 'C'};
int velFantasmas = 600;
int moverTimer;
struct Fantasma
{
    struct coord posicion;
    struct coord initPos;
    int vx;
    int vy;
    int comido;
    int tiempoEspera;
    HBRUSH brush;
};
struct Fantasma allFantasmas[CANT_FANTASMA];

struct PacMan jPacman = {
    .posicion = {.x = 0, .y = 0},
    .vx = 0,
    .vy = 0,
    .vidas = 3,
    .powerup = 0,
    .powerupTimer = 0,
    .puntos = 0,
    .moviendo = 0,
    .muerto = 0};

////////// HERRAMIENTAS
/// @brief Devuelve un puntero al nivel correspondiente según el nivel actual
const char (*obtenerNivelSeleccionado())[H][W]
{
    // Seleccionar el nivel fijo para niveles del 1 al 5
    switch (nivel)
    {
    case 1:
        return &nivel1;
    case 2:
        return &nivel2;
    case 3:
        return &nivel3;
    case 4:
        return &nivel4;
    case 5:
        return &nivel5;
    default:
        // Seleccionar un nivel aleatorio para niveles mayores a 5
        const char(*nivelesAleatorios[])[H][W] = {&nivel1, &nivel2, &nivel3, &nivel4, &nivel5};
        int nivelAleatorio = rand() % 5;
        return nivelesAleatorios[nivelAleatorio];
    }
}

/// @brief Actualiza la playfield
void ActualizarPlayfield(HWND hwnd, int row, int col, char Caracter, char playfield[H][W])
{
    playfield[row][col] = Caracter; // Actualizar Playfield

    // Calcular el tamaño a ocupar
    RECT updateRect = {
        col * CELL_SIZE,       // Izquierda
        row * CELL_SIZE,       // Arriba
        (col + 1) * CELL_SIZE, // Derecha
        (row + 1) * CELL_SIZE  // Abajo
    };

    // Redibujar lo que hay en pantalla
    RedrawWindow(hwnd, &updateRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void DibujarTexto(HDC hdc, int x, int y, const char *texto, int valor)
{
    char buffer[50];

    // Si el valor dado es menor a 0, evitamos mostrarlo
    // Hacemos esto para el cartel de game over, sino apareceria con un 0 al lado
    if (valor >= 0)
        sprintf(buffer, "%s: %d", texto, valor);
    else
        sprintf(buffer, "%s", texto, valor);

    // Estilo del texto
    SetTextColor(hdc, RGB(255, 255, 255)); // Blanco
    SetBkMode(hdc, RGB(5, 5, 20));         // Fondo transparente

    // Dibujar el texto
    TextOut(hdc, x, y, buffer, strlen(buffer));
}

/// @brief Redibujamos la zona del puntaje
void redibujarPuntaje(HWND hwnd)
{
    // Define el área del HUD
    RECT hudArea = {
        W * CELL_SIZE + 20,  // X inicial (justo al lado del área de juego)
        0,                   // Y inicial
        W * CELL_SIZE + 200, // X final (ancho del HUD)
        H * CELL_SIZE        // Y final (altura del HUD)
    };

    // Solo invalida el área del HUD
    RedrawWindow(hwnd, &hudArea, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

/// @brief Movimiento de los fantasmas
void moverFantasma(HWND hwnd, int i, int px, int py, int cx, int cy)
{
    int visitado[H][W] = {0};
    struct coord cola[H * W];
    int front = 0, rear = 0;
    int direcciones[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    struct coord padre[H][W] = {{{-1, -1}}};

    // Inicializar BFS
    cola[rear++] = (struct coord){.x = cx, .y = cy};
    visitado[cy][cx] = 1;

    int encontrado = 0; // Indica si se encontró a Pac-Man

    // BFS para encontrar la ruta hacia Pac-Man
    while (front < rear && !encontrado)
    {
        struct coord actual = cola[front++];
        for (int d = 0; d < 4; d++)
        {
            int nx = actual.x + direcciones[d][0];
            int ny = actual.y + direcciones[d][1];

            // Validar límites y evitar celdas bloqueadas
            if (nx >= 0 && nx < W && ny >= 0 && ny < H && !visitado[ny][nx] && playfield[ny][nx] != '#')
            {
                visitado[ny][nx] = 1;
                padre[ny][nx] = actual;
                cola[rear++] = (struct coord){.x = nx, .y = ny};

                // Verificar si se llegó a Pac-Man
                if (nx == px && ny == py)
                {
                    encontrado = 1;
                    break;
                }
            }
        }
    }

    // Reconstruir el camino desde Pac-Man hasta Blinky
    if (encontrado)
    {
        struct coord actual = (struct coord){.x = px, .y = py};
        while (padre[actual.y][actual.x].x != cx || padre[actual.y][actual.x].y != cy)
        {
            actual = padre[actual.y][actual.x];
        }

        // Mover a Blinky hacia la primera celda del camino
        allFantasmas[i].vx = actual.x - cx;
        allFantasmas[i].vy = actual.y - cy;
    }
    else
    {
        // Respaldo: elegir un movimiento aleatorio válido
        int direccionesValidas[4][2];
        int numDirecciones = 0;

        for (int d = 0; d < 4; d++)
        {
            int nx = cx + direcciones[d][0];
            int ny = cy + direcciones[d][1];

            // Verificar que la dirección no sea hacia una pared o fuera de límites
            if (nx >= 0 && nx < W && ny >= 0 && ny < H && playfield[ny][nx] != '#')
            {
                direccionesValidas[numDirecciones][0] = direcciones[d][0];
                direccionesValidas[numDirecciones][1] = direcciones[d][1];
                numDirecciones++;
            }
        }

        if (numDirecciones > 0)
        {
            // Elegir una dirección válida aleatoria
            int aleatoria = rand() % numDirecciones;
            allFantasmas[i].vx = direccionesValidas[aleatoria][0];
            allFantasmas[i].vy = direccionesValidas[aleatoria][1];
        }
        else
        {
            // Si no hay movimientos válidos (esto no debería ocurrir), quedarse en su lugar
            allFantasmas[i].vx = 0;
            allFantasmas[i].vy = 0;
        }
    }
}

char frutaAleatoria()
{
    return frutas[rand() % 5];
}

// Inicializar juego
void inicializar(HWND hwnd)
{
    int x, y;

    // Obtenermos nuevo mapa dependiendo del nivel
    const char(*nivelSeleccionado)[H][W] = obtenerNivelSeleccionado();

    for (y = 0; y < H; y++)
    {
        for (x = 0; x < W; x++)
        {
            // igualamos la playfield al mapa del nivel
            playfield[y][x] = (*nivelSeleccionado)[y][x];

            // Vaciamos los playfields para reiniciar todo cuando se pierde o gana
            playfieldParedes[y][x] = ' ';
            playfieldPelotas[y][x] = ' ';
            playfieldEntidades[y][x] = ' ';

            switch (playfield[y][x])
            {
            case 'P':
            {
                // Igualamos la posicion de Pacman con su posicion inicial, para vincularlas
                jPacman.posicion.x = x;
                jPacman.posicion.y = y;

                ActualizarPlayfield(hwnd, y, x, '<', playfieldEntidades);
                break;
            }

            case '.':
            {
                // Contar el total pelotas por comer, para asi saber cuando ganamos
                totalPelotas++;

                playfieldPelotas[y][x] = playfield[y][x];
                break;
            }

            case '*':
            {
                // Añadir los powerups al total de pelotas para que cuenten en el total.
                totalPelotas++;

                playfieldPelotas[y][x] = playfield[y][x];
                break;
            }

            case 'F':
            {
                // Establecer una fruta aleatoria al inicio
                playfieldPelotas[y][x] = frutaAleatoria();
                break;
            }

            case '-':
            case '#':
            {
                playfieldParedes[y][x] = playfield[y][x];
                break;
            }
            case 'H':
            {
                playfieldEntidades[y][x] = 'H';
                break;
            }
            default:
            {
                playfieldParedes[y][x] = ' ';
                playfieldPelotas[y][x] = ' ';
                playfieldEntidades[y][x] = ' ';
                break;
            }
            }
        }
    }

    // Inicializar los fantasmas
    for (int i = 0; i < CANT_FANTASMA; i++)
    {
        allFantasmas[i].vx = 0;
        allFantasmas[i].vy = 0;
        allFantasmas[i].comido = 0;
        allFantasmas[i].tiempoEspera = 0;

        // Ubicar fantasmas aleatoriamente en las celdas con 'H'
        do
        {
            x = 1 + rand() % (W - 2); // Evitar bordes
            y = 1 + rand() % (H - 2);
        } while (playfieldEntidades[y][x] != 'H');

        allFantasmas[i].posicion.x = x;
        allFantasmas[i].posicion.y = y;
        allFantasmas[i].initPos.x = x;
        allFantasmas[i].initPos.y = y;

        ActualizarPlayfield(hwnd, y, x, ghostChars[i], playfieldEntidades);
    }
}

void reiniciarJuego(HWND hwnd, int esVictoria)
{
    // Anular movimientos de Pac-Man y fantasmas
    jPacman.vx = 0;
    jPacman.vy = 0;
    for (int i = 0; i < CANT_FANTASMA; i++)
    {
        allFantasmas[i].vx = 0;
        allFantasmas[i].vy = 0;
    }

    // Actualizar estado de Pac-Man
    jPacman.moviendo = 0;
    jPacman.muerto = 0;
    jPacman.powerup = 0;

    if (esVictoria)
    {
        // Si es victoria, aumentar nivel y velocidad
        nivel += 1;
        velFantasmas = ceil(velFantasmas * 0.8);

        if (velFantasmas < 0) // Evitar puntajes negativos
        {
            velFantasmas = 0;
        }
    }
    else if (!esVictoria && !gameover)
    {
        // Si es muerte, descontar vidas y puntos
        jPacman.vidas -= 1;
        jPacman.puntos = (jPacman.puntos > 20) ? jPacman.puntos - 20 : 0; // Evitar puntajes negativos

        // Si nos quedamos sin vidas, terminar el juego
        if (jPacman.vidas <= 0)
        {
            gameover = 1;
            return;
        }
    }

    // Reiniciar posiciones y pantalla
    inicializar(hwnd);

    // Redibujar completamente la pantalla
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void muerte(HWND hwnd)
{
    reiniciarJuego(hwnd, 0); // Llamada con "0" indicando que no es victoria
}

void ganar(HWND hwnd)
{
    reiniciarJuego(hwnd, 1); // Llamada con "1" indicando que es victoria
}

// Detectar flechas del teclado
void input(WPARAM wParam, HWND hwnd)
{
    switch (wParam)
    {
    case VK_UP:
    {
        jPacman.vy = -1;
        jPacman.vx = 0;
        jPacman.moviendo = 1;
        break;
    }
    case VK_DOWN:
    {
        jPacman.vy = 1;
        jPacman.vx = 0;
        jPacman.moviendo = 1;
        break;
    }
    case VK_LEFT:
    {
        jPacman.vx = -1;
        jPacman.vy = 0;
        jPacman.moviendo = 1;
        break;
    }
    case VK_RIGHT:
    {
        jPacman.vx = 1;
        jPacman.vy = 0;
        jPacman.moviendo = 1;
        break;
    }
    case VK_F11:
    {
        ganar(hwnd);
        break;
    }
    default:
        jPacman.moviendo = 0;
        break;
    }
}

void Pacman(HWND hwnd)
{
    // si termino el juego, parar todo
    if (gameover)
        return;

    // Calcular nueva posición
    int nx = (jPacman.posicion.x + jPacman.vx + W) % W; // Manejo de screenwrap
    int ny = (jPacman.posicion.y + jPacman.vy + H) % H; // Asegura que la posición siempre esté dentro del rango

    // Verificar colisión con fantasmas
    for (int i = 0; i < CANT_FANTASMA; i++)
    {
        if (jPacman.posicion.x == allFantasmas[i].posicion.x &&
            jPacman.posicion.y == allFantasmas[i].posicion.y)
        {
            if (jPacman.powerup)
            {
                jPacman.puntos += 200;
                allFantasmas[i].comido = 1;
                allFantasmas[i].tiempoEspera = DURACION_ESPERA;
                redibujarPuntaje(hwnd);
            }
            else if (!allFantasmas[i].comido)
            {
                // Manejar muerte y finalizar
                jPacman.muerto = 1;
                muerte(hwnd);
                return;
            }
        }
    }

    // Reducir el temporizador del powerup si está activo
    if (jPacman.powerup && --jPacman.powerupTimer <= 0)
    {
        jPacman.powerup = 0; // Desactivar el powerup
    }

    // Si no hay movimiento, salir
    if (jPacman.vx == 0 && jPacman.vy == 0)
        return;

    // Verificar colisión con paredes
    if (playfieldParedes[ny][nx] == '#' || playfieldParedes[ny][nx] == '-')
    {
        jPacman.vx = 0;
        jPacman.vy = 0;
        return;
    }

    // Manejar colisiones con pelotas o frutas
    char celda = playfieldPelotas[ny][nx];
    if (celda != ' ')
    {
        switch (celda)
        {
        case '.': // Pelota pequeña
            jPacman.puntos += 10;
            break;
        case '*': // Power-up
            jPacman.powerup = 1;
            jPacman.powerupTimer = DURACION_POWER;
            jPacman.puntos += 50;
            break;
        case 'Z':
            jPacman.puntos += 100;
            break; // Cereza
        case 'f':
            jPacman.puntos += 300;
            break; // Frutilla
        case 'N':
            jPacman.puntos += 500;
            break; // Naranja
        case 'M':
            jPacman.puntos += 700;
            break; // Manzana
        case 'o':
            jPacman.puntos += 1000;
            break; // Melón
        }

        // Comer el objeto
        playfieldPelotas[ny][nx] = ' ';
        totalPelotas--;
        redibujarPuntaje(hwnd);
    }

    // Actualizar el playfield
    ActualizarPlayfield(hwnd, jPacman.posicion.y, jPacman.posicion.x, ' ', playfieldEntidades);

    // Actualizar la posición de Pac-Man
    jPacman.posicion.x = nx;
    jPacman.posicion.y = ny;

    // Dibujar Pac-Man en la nueva posición
    ActualizarPlayfield(hwnd, ny, nx, '<', playfieldEntidades);
}

void Fantasma(HWND hwnd)
{
    if (moverTimer)
    {
        for (int i = 0; i < CANT_FANTASMA; i++)
        {
            // Obtener nueva posición (nx, ny)
            int nx = allFantasmas[i].vx + allFantasmas[i].posicion.x;
            int ny = allFantasmas[i].vy + allFantasmas[i].posicion.y;

            if (playfieldParedes[ny][nx] == '#') // Verificar si hay una pared en la nueva posición
            {
                // Cancelar movimiento si hay una pared
                allFantasmas[i].vx = 0;
                allFantasmas[i].vy = 0;
            }
            else if ((allFantasmas[i].vx != 0 || allFantasmas[i].vy != 0))
            {
                // Actualizar posición si el fantasma se está moviendo
                ActualizarPlayfield(hwnd, allFantasmas[i].posicion.y, allFantasmas[i].posicion.x, ' ', playfieldEntidades);

                // Mover a la nueva posición
                allFantasmas[i].posicion.x = nx;
                allFantasmas[i].posicion.y = ny;

                // Actualizar la nueva posición en el playfield
                ActualizarPlayfield(hwnd, ny, nx, ghostChars[i], playfieldEntidades);
            }

            // Comportamiento de los fantasmas
            if (jPacman.powerup && !allFantasmas[i].comido)
            {
                // Evitar a Pacman
                int dx = jPacman.posicion.x - allFantasmas[i].posicion.x;
                int dy = jPacman.posicion.y - allFantasmas[i].posicion.y;

                // Elegir dirección para alejarse
                int objetivoX = allFantasmas[i].posicion.x + (dx > 0 ? -5 : 5);
                int objetivoY = allFantasmas[i].posicion.y + (dy > 0 ? -5 : 5);

                // Limitar el objetivo dentro del mapa
                objetivoX = objetivoX < 0 ? 0 : (objetivoX >= W ? W - 1 : objetivoX);
                objetivoY = objetivoY < 0 ? 0 : (objetivoY >= H ? H - 1 : objetivoY);

                moverFantasma(hwnd, i, objetivoX, objetivoY, allFantasmas[i].posicion.x, allFantasmas[i].posicion.y);
            }
            else if (allFantasmas[i].comido)
            {
                // Regresar al punto de spawn
                int objetivoX = allFantasmas[i].initPos.x;
                int objetivoY = allFantasmas[i].initPos.y;

                moverFantasma(hwnd, i, objetivoX, objetivoY, allFantasmas[i].posicion.x, allFantasmas[i].posicion.y);

                if (allFantasmas[i].posicion.x == objetivoX && allFantasmas[i].posicion.y == objetivoY)
                {
                    // Esperar en el punto inicial
                    if (allFantasmas[i].tiempoEspera > 0)
                    {
                        allFantasmas[i].tiempoEspera--;
                    }
                    else
                    {
                        allFantasmas[i].comido = 0; // Restaurar estado
                        return;
                    }
                }
            }
            else // Si esta todo usual (Pacman sin powerup, fantasma no fue comido), usar comportamiento normal de cada fantasma
            {
                switch (i)
                {
                case 0: // Blinky: Solo persigue a Pac-Man.
                {
                    // Coordenadas actuales de Blinky
                    int cx = allFantasmas[i].posicion.x;
                    int cy = allFantasmas[i].posicion.y;

                    // Coordenadas de Pac-Man
                    int px = jPacman.posicion.x;
                    int py = jPacman.posicion.y;

                    moverFantasma(hwnd, i, px, py, cx, cy);

                    break;
                }
                case 1: // Pinky: Intenta anticipar los movimientos de Pac-Man
                {
                    int anticipar = 3; // Número de celdas a anticipar
                    int px = jPacman.posicion.x + jPacman.vx * anticipar;
                    int py = jPacman.posicion.y + jPacman.vy * anticipar;

                    int cx = allFantasmas[i].posicion.x;
                    int cy = allFantasmas[i].posicion.y;

                    // Asegurarse de que la posición anticipada esté dentro del mapa
                    px = (px < 0) ? 0 : (px >= W ? W - 1 : px);
                    py = (py < 0) ? 0 : (py >= H ? H - 1 : py);

                    moverFantasma(hwnd, i, px, py, cx, cy);

                    break;
                }
                case 2: // Inky: Persigue una posición calculada como un punto simétrico en relación con otro fantasma (Blinky).
                {
                    // Coordenadas actuales de Inky
                    int cx = allFantasmas[i].posicion.x;
                    int cy = allFantasmas[i].posicion.y;

                    // Coordenadas de Blinky
                    int blinkyX = allFantasmas[0].posicion.x;
                    int blinkyY = allFantasmas[0].posicion.y;

                    // Calcular un punto adelantado a Pac-Man (2 casillas hacia su dirección actual)
                    int anticipar = 2;
                    int targetX = (jPacman.posicion.x + jPacman.vx * anticipar) + blinkyX;
                    int targetY = (jPacman.posicion.y + jPacman.vy * anticipar) + blinkyY;

                    // Asegurar que el nuevo objetivo esté dentro de los límites del mapa
                    targetX = (targetX < 0) ? 0 : (targetX >= W ? W - 1 : targetX);
                    targetY = (targetY < 0) ? 0 : (targetY >= H ? H - 1 : targetY);

                    moverFantasma(hwnd, i, targetX, targetY, cx, cy);

                    break;
                }
                case 3: // Clyde: Alterna entre perseguir a Pac-Man cuando está lejos y alejarse cuando está cerca.
                {
                    // Coordenadas actuales de Clyde
                    int cx = allFantasmas[i].posicion.x;
                    int cy = allFantasmas[i].posicion.y;

                    // Coordenadas de Pac-Man
                    int px = jPacman.posicion.x;
                    int py = jPacman.posicion.y;

                    // Calcular la distancia al cuadrado entre Clyde y Pac-Man
                    int dx = px - cx;
                    int dy = py - cy;
                    int distanciaCuadrada = dx * dy;

                    // Determinar objetivo según la distancia
                    int objetivoX, objetivoY;

                    if (distanciaCuadrada < 32) // Si está cerca, alejarse
                    {
                        // Elegir una celda más alejada que no sea una pared
                        objetivoX = (dx > 0) ? cx - 5 : cx + 5;
                        objetivoY = (dy > 0) ? cy - 5 : cy + 5;
                    }
                    else // Si está lejos, perseguir a Pac-Man
                    {
                        objetivoX = px;
                        objetivoY = py;
                    }

                    // Asegurar que las coordenadas del objetivo estén dentro del mapa
                    objetivoX = (objetivoX < 0) ? 0 : (objetivoX >= W ? W - 1 : objetivoX);
                    objetivoY = (objetivoY < 0) ? 0 : (objetivoY >= H ? H - 1 : objetivoY);

                    moverFantasma(hwnd, i, objetivoX, objetivoY, cx, cy);

                    break;
                }
                }
            }
            moverTimer = 0;
        }
    }
}

// Función de reaparecer frutas con control de tiempo
void reaparecerFruta(HWND hwnd)
{
    if (++contadorRespawnFrutas < TIEMPO_RESPAWN_FRUTAS)
        return; // Aún no es tiempo de reaparecer frutas

    // Si se prosigue, reiniciar el contador
    contadorRespawnFrutas = 0;

    // Reaparecer frutas en las celdas marcadas como 'F'
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            if (playfield[y][x] == 'F' && playfieldPelotas[y][x] == ' ')
            {
                // Esta funcion pone una fruta aleatoria :)
                ActualizarPlayfield(hwnd, y, x, frutaAleatoria(), playfieldPelotas);
            }
        }
    }
}

//////////// VENTANA
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Nombre
    const char CLASS_NAME[] = "PacmanJuego";

    // Estructura
    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;                        // Función de procesamiento de mensajes
    wc.hInstance = hInstance;                           // Instancia de la aplicación
    wc.lpszClassName = CLASS_NAME;                      // Nombre de la clase
    wc.hbrBackground = CreateSolidBrush(RGB(5, 5, 20)); // Fondo
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);           // Cursor estándar

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Error al registrar la clase de ventana.", "Error", MB_ICONERROR);
        return 0;
    }

    // Creación de la ventana
    HWND hwnd = CreateWindowEx(
        0,                            // Estilo extendido
        CLASS_NAME,                   // Nombre de la clase
        "Pacman",                     // Título de la ventana
        WS_OVERLAPPEDWINDOW,          // Estilo de ventana
        CW_USEDEFAULT, CW_USEDEFAULT, // Posición inicial
        800, 600,                     // Tamaño (ancho, alto)
        NULL,                         // Sin ventana padre
        NULL,                         // Sin menú
        hInstance,                    // Instancia de la aplicación
        NULL                          // Sin parámetros adicionales
    );

    // Mostrar la ventana
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Bucle de mensajes
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        inicializar(hwnd);

        // ID de los Temporizadores:
        // 1 = Pacman y fantasmas
        // 2 = Movimiento de los fantasmas
        // 3 = Respawn de Frutas
        SetTimer(hwnd, 1, 16000 / 60, NULL);
        SetTimer(hwnd, 2, velFantasmas, NULL);
        SetTimer(hwnd, 3, 1000, NULL);

        break;
    }

    case WM_KEYDOWN:
    {
        input(wParam, hwnd);
        break;
    }

    case WM_TIMER:
    {
        switch (wParam)
        {

        // Logica del Juego
        case 1:
        {
            Pacman(hwnd);
            Fantasma(hwnd);

            if (totalPelotas == 0)
            {
                ganar(hwnd);
            }

            if (gameover)
            {
                KillTimer(hwnd, 1);
                KillTimer(hwnd, 2);
                KillTimer(hwnd, 3);
            }

            break;
        }

        case 2:
        {
            // Cuando se active, avisar que el fantasma se puede mover.
            moverTimer = 1;
        }

        // Reaparecer Fruta
        case 3:
        {
            reaparecerFruta(hwnd);
            break;
        }
        }

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Crear pinceles reutilizables
        HBRUSH brushHUD = CreateSolidBrush(RGB(5, 5, 20));
        HBRUSH brushPacMan = CreateSolidBrush(RGB(255, 255, 0));
        HBRUSH brushPelota = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH brushBorrar = CreateSolidBrush(RGB(5, 5, 20));

        /// DEFINIR COLOR DE LOS FANTASMAS
        for (int i = 0; i < CANT_FANTASMA; i++)
        {
            HBRUSH nuevoBrush;

            // Determinar el color del pincel basado en el Fantasma y su estado
            if (jPacman.powerup && !allFantasmas[i].comido)
            {
                nuevoBrush = CreateSolidBrush(RGB(255, 255, 255)); // Fantasma vulnerable (blanco)
            }
            else if (allFantasmas[i].comido)
            {
                nuevoBrush = CreateSolidBrush(RGB(56, 48, 130)); // Fantasma vulnerable (blanco)
            }
            else
            {
                switch (i)
                { // Estado normal
                case 0:
                    nuevoBrush = CreateSolidBrush(RGB(255, 0, 0));
                    break; // Blinky
                case 1:
                    nuevoBrush = CreateSolidBrush(RGB(255, 184, 255));
                    break; // Pinky
                case 2:
                    nuevoBrush = CreateSolidBrush(RGB(1, 255, 255));
                    break; // Inky
                case 3:
                    nuevoBrush = CreateSolidBrush(RGB(255, 184, 81));
                    break; // Clyde
                default:
                    continue;
                }
            }

            // Eliminar el pincel existente antes de asignar el nuevo (evitar fugas)
            if (allFantasmas[i].brush)
            {
                DeleteObject(allFantasmas[i].brush);
            }

            // Asignar el nuevo pincel al fantasma
            allFantasmas[i].brush = nuevoBrush;
        }

        /// Definir los colores de los niveles
        HBRUSH brushPared;
        switch (nivel)
        {
        case 1:
            brushPared = CreateSolidBrush(RGB(48, 39, 114)); // Azul
            break;
        case 2:
            brushPared = CreateSolidBrush(RGB(255, 184, 174)); // Rosa
            break;
        case 3:
            brushPared = CreateSolidBrush(RGB(222, 151, 81)); // Naranja
            break;
        case 4:
            brushPared = CreateSolidBrush(RGB(53, 156, 156)); // Celeste
            break;
        case 5:
            brushPared = CreateSolidBrush(RGB(129, 129, 129)); // Verde
            break;
        default:
        {
            // Colores de los niveles del 6 en adelante, se elige uno aleatoriamente
            COLORREF colores[] = {
                RGB(48, 39, 114),   // Azul
                RGB(255, 184, 174), // Rosa
                RGB(222, 151, 81),  // Naranja
                RGB(53, 156, 156),  // Celeste
                RGB(53, 138, 113),  // Verde
                RGB(129, 129, 129), // Gris
                RGB(128, 0, 0),     // Rojo
                RGB(74, 35, 115)    // Violeta
            };

            // Seleccionar un color al azar
            int colorIndex = rand() % 8;

            brushPared = CreateSolidBrush(colores[colorIndex]);
            break;
        }
        }

        // Si el juego terminó, limpiar pantalla y mostrar mensaje
        if (gameover)
        {
            RECT rectPantalla;
            GetClientRect(hwnd, &rectPantalla);                           // Obtener dimensiones completas de la ventana
            FillRect(hdc, &rectPantalla, CreateSolidBrush(RGB(0, 0, 0))); // Fondo negro
            SetTextColor(hdc, RGB(255, 255, 255));                        // Texto blanco
            SetBkMode(hdc, TRANSPARENT);                                  // Fondo transparente para texto
            DrawText(hdc, "FIN DEL JUEGO", -1, &rectPantalla, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            EndPaint(hwnd, &ps);
            return 0; // Salir para evitar más redibujado
        }

        //// INTERFAZ DE PUNTOS (HUD)
        int textoX = W * CELL_SIZE + 20; // Posición X inicial del texto
        int textoY = 20;                 // Posición Y inicial del texto

        // Definir el área del HUD
        RECT rectHUD = {W * CELL_SIZE, 0, W * CELL_SIZE + 200, H * CELL_SIZE};

        // Borrar el área del HUD, para que no se dibuje encima del otro
        FillRect(hdc, &rectHUD, brushHUD);

        // Dibujar nivel, vidas y puntaje
        DibujarTexto(hdc, textoX, textoY, "Nivel", nivel);
        textoY += 30;
        DibujarTexto(hdc, textoX, textoY, "Vidas", jPacman.vidas);
        textoY += 30;
        DibujarTexto(hdc, textoX, textoY, "Puntaje", jPacman.puntos);

        // Borrar brocha
        DeleteObject(brushHUD);

        // Dibujar entidades
        for (int row = 0; row < H; ++row)
        {
            for (int col = 0; col < W; ++col)
            {
                RECT cell = {
                    col * CELL_SIZE,
                    row * CELL_SIZE,
                    (col + 1) * CELL_SIZE,
                    (row + 1) * CELL_SIZE};

                switch (playfieldEntidades[row][col])
                {
                case '<': // Pac-Man
                    SelectObject(hdc, brushPacMan);
                    Pie(hdc, cell.left, cell.top, cell.right, cell.bottom, 470 * col, 200 * row, (290 + 1) * col, (300 + 1) * row);
                    break;

                case 'B': // Blinky
                case 'I': // Inky
                case 'p': // Pinky
                case 'C': // Clyde
                    for (int i = 0; i < CANT_FANTASMA; ++i)
                    {
                        // Verificar si el fantasma actual está en esta celda
                        if (allFantasmas[i].posicion.x == col && allFantasmas[i].posicion.y == row)
                        {
                            SelectObject(hdc, allFantasmas[i].brush); // Usar el pincel correspondiente
                            RoundRect(hdc, cell.left, cell.top, cell.right, cell.bottom, 10, 10);
                            break;
                        }
                    }
                    break;

                default: // Celdas vacías o sin entidad
                    SelectObject(hdc, brushBorrar);
                    FillRect(hdc, &cell, brushBorrar);
                    break;
                }
            }
        }

        // Dibujar pelotas y frutas
        for (int row = 0; row < H; ++row)
        {
            for (int col = 0; col < W; ++col)
            {
                RECT cell = {
                    col * CELL_SIZE,
                    row * CELL_SIZE,
                    (col + 1) * CELL_SIZE,
                    (row + 1) * CELL_SIZE};

                int centroX = (cell.left + cell.right) / 2;
                int centroY = (cell.top + cell.bottom) / 2;

                switch (playfieldPelotas[row][col])
                {
                case '.': // Pelota pequeña
                    SelectObject(hdc, brushPelota);
                    Ellipse(hdc, centroX - CELL_SIZE / 5, centroY - CELL_SIZE / 5,
                            centroX + CELL_SIZE / 5, centroY + CELL_SIZE / 5);
                    break;

                case '*': // Power-up (pelota grande)
                    SelectObject(hdc, brushPelota);
                    Ellipse(hdc, centroX - CELL_SIZE / 2.5, centroY - CELL_SIZE / 2.5,
                            centroX + CELL_SIZE / 2.5, centroY + CELL_SIZE / 2.5);
                    break;

                case 'Z':                                                       // Cereza
                    SelectObject(hdc, CreateSolidBrush(RGB(255, 0, 0)));        // Rojo
                    Ellipse(hdc, centroX - 10, centroY - 10, centroX, centroY); // Primera cereza
                    Ellipse(hdc, centroX, centroY - 10, centroX + 10, centroY); // Segunda cereza
                    SelectObject(hdc, CreateSolidBrush(RGB(0, 128, 0)));        // Tallo verde
                    MoveToEx(hdc, centroX, centroY - 10, NULL);
                    LineTo(hdc, centroX, centroY - 20);
                    break;

                case 'f':                                                // Frutilla
                    SelectObject(hdc, CreateSolidBrush(RGB(255, 0, 0))); // Rojo
                    POINT frutilla[] = {
                        {centroX - 10, centroY - 10}, // Vértice izquierdo
                        {centroX + 10, centroY - 10}, // Vértice derecho
                        {centroX, centroY + 10}       // Punta invertida (abajo)
                    };
                    Polygon(hdc, frutilla, 3);                                          // Triángulo invertido
                    SelectObject(hdc, CreateSolidBrush(RGB(0, 128, 0)));                // Corona verde
                    Ellipse(hdc, centroX - 5, centroY - 15, centroX + 5, centroY - 10); // Parte superior
                    break;

                case 'N':                                                  // Naranja
                    SelectObject(hdc, CreateSolidBrush(RGB(255, 165, 0))); // Naranja
                    Ellipse(hdc, centroX - 10, centroY - 10, centroX + 10, centroY + 10);
                    break;

                case 'M':                                                // Manzana
                    SelectObject(hdc, CreateSolidBrush(RGB(255, 0, 0))); // Rojo
                    Ellipse(hdc, centroX - 10, centroY - 10, centroX + 10, centroY + 10);
                    SelectObject(hdc, CreateSolidBrush(RGB(0, 128, 0))); // Hoja verde
                    MoveToEx(hdc, centroX, centroY - 10, NULL);
                    LineTo(hdc, centroX + 5, centroY - 15);
                    break;

                case 'o':                                                // Melón
                    SelectObject(hdc, CreateSolidBrush(RGB(0, 255, 0))); // Verde
                    Ellipse(hdc, centroX - 10, centroY - 10, centroX + 10, centroY + 10);
                    SelectObject(hdc, CreateSolidBrush(RGB(0, 128, 0))); // Rayas verdes
                    for (int i = -8; i <= 8; i += 4)
                    {
                        MoveToEx(hdc, centroX + i, centroY - 10, NULL);
                        LineTo(hdc, centroX + i, centroY + 10);
                    }
                    break;
                }
            }
        }

        // Dibujar paredes con dither
        for (int row = 0; row < H; ++row)
        {
            for (int col = 0; col < W; ++col)
            {
                RECT cell = {
                    col * CELL_SIZE,       // Coordenada izquierda
                    row * CELL_SIZE,       // Coordenada superior
                    (col + 1) * CELL_SIZE, // Coordenada derecha
                    (row + 1) * CELL_SIZE  // Coordenada inferior
                };

                if (playfieldParedes[row][col] == '#')
                {
                    // Si es una pared sólida, dibujamos con el color normal
                    SelectObject(hdc, brushPared);
                    FillRect(hdc, &cell, brushPared);
                }
            }
        }

        // Liberar pinceles
        DeleteObject(brushPacMan);
        DeleteObject(brushPelota);
        DeleteObject(brushPared);
        DeleteObject(brushBorrar);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}