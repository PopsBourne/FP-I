
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Windows.h"
#include "conio.h"
#include "colores.h"


using namespace std;

//-----------------------CONSTANTES Y TIPOS ----------------------------

const int MAX = 50;
const int MAXH = 10;
const int MAXE = 100;


typedef enum tCasilla { Libre, Muro, DestinoL, DestinoC, DestinoJ, Jugador, Caja };
typedef tCasilla tTablero[MAX][MAX];


typedef struct {
	tTablero tablero;
	int nfilas;
	int ncolumnas;
	int filaJugador;
	int columnaJugador;
	int numCajas;
	int numCajasDestino;
	int fila;
	int columna;
}tSokoban;

typedef struct {
	tSokoban tab[MAXH];
	int cont;
	int fin;
	bool des;
}tHistoria;

typedef struct {
	tSokoban sokoban;//estado del tablero
	int numMovimientos;
	string nFichero;//nombre
	int nivel;
	tHistoria historia;
}tJuego;

typedef struct {//partida ganada
	string nombreFich;
	int nivel;
	int numM;
}tPartida;

typedef tPartida tExitos[MAXE];

typedef struct {//informacion jugador
	string nombre;
	tExitos exitos;
	int contR;
}tInfo;


typedef enum { Arriba, Abajo, Derecha, Izquierda, Salir, Nada, Deshacer } tTecla;


//-------------------------- PROTOTIPOS --------------------------------


//--------------------------VERSION 1--------------------------

int menu();

//--------------------------Visualizacion del tablero--------------------------

// muestra una casilla del tablero.
void dibujaCasilla(tCasilla casilla);

//muestra el tablero del juego, el nombre del fichero desde que se ha cargado, su nivel y el número de movimientos realizados.
void dibujar(const tJuego &juego);

//ajusta los colores de fondo y primer plano
void colorFondo(int color);


//--------------------------Carga de un nivel del juego--------------------------

//inicializa el tablero, haciendo que todas las MAX x MAX casillas estén libres y el número de movimientos a 0.
void inicializa(tJuego &juego);

//solicita al usuario el fichero y el nivel que desea jugar y lo carga desde dicho fichero
bool cargarJuego(tJuego & juego);

//busca en el fichero el nivel solicitado y carga el tablero correspondiente. Devuelve un booleano indicando si lo ha encontrado
bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel);


//--------------------------El juego en accion--------------------------

//devuelve un valor de tipo tTecla, que puede ser una de las cuatro direcciones si se pulsan las flechas de dirección correspondientes 
//el valor Salir, si se pulsa la tecla Esc; o Nada si se pulsa cualquier otra tecla.
tTecla leerTecla();


//--------------------------Movimiento del tablero--------------------------

//realiza el movimiento del jugador en la dirección indicada.Si no se puede realizar el movimiento,
//no tiene efecto y no se incrementa tampoco el número de movimientos registrados.
void hacerMovimiento(tJuego &juego, tTecla tecla);


//--------------------------VERSION 2--------------------------

//Comprueba si alguna de las cajas no colocadas en destino ha quedado atrapada en una esquina
bool bloqueado(const tJuego &juego);

//Permite deshacer los ultimos 10 movimientos
bool deshacerMovimiento(tJuego &juego);

//guarda la pantalla del tablero antes de realizar el movimiento
void guardarPantalla(tJuego &juego);

//Indica si el jugador ha ganado
bool terminado(tJuego &juego);

//--------------------------VERSION 3--------------------------
void operator<(tJuego const &juego, tInfo &info);
bool& operator==(tJuego const &juego, tInfo &info);

//muestra la informacion de la ultima partida ganada
void mostrarPartida(tInfo info);

//guarda la informacion de la partida ganada en un txt creado con el nombre del usuario
void guardarFichero(tInfo info);



//------------------------ IMPLEMENTACION ------------------------------

int main() {
	int opc;
	tInfo info;
	tJuego juego;
	ifstream fichInfo;
	string compruebaFich;

	cout << "Nombre: ";
	cin >> info.nombre;
	info.contR = 0;

	fichInfo.open(info.nombre + ".txt");
	//se carga la informacion del usuario
	if (fichInfo.is_open()) {
		cout << "Usuario encontrado" << endl;
		fichInfo >> compruebaFich;
		while (compruebaFich != "") {
			info.exitos[info.contR].nombreFich = compruebaFich;
			fichInfo >> info.exitos[info.contR].nivel;
			fichInfo >> info.exitos[info.contR].numM;
			info.contR++;
			compruebaFich = "";
			fichInfo >> compruebaFich;
		}
	}
	//se crea uno nuevo
	else cout << "Usuario nuevo registrado" << endl;

	system("pause");
	system("cls");


	opc = menu();

	do {
		switch (opc) {

		case 1:
			if (cargarJuego(juego)) {

				if (terminado(juego)) {
					if (juego == info) {}
					else {
						juego < info;
						guardarFichero(info);
						system("pause");
					}
				}
			}
			break;

		case 2:
			mostrarPartida(info);
			system("pause");
			system("cls");
			break;

		case 0: break;
		}
		opc = menu();
	} while (opc != 0);


	return 0;
}


int menu() {
	int opc;

	do {
		cout << "********************************" << endl;
		cout << "             MENU               " << endl;
		cout << "********************************" << endl;
		cout << "1 - Jugar partida" << endl << endl;
		cout << "2 - Informacion jugador" << endl << endl;
		cout << "0 - Salir" << endl;
		cout << "********************************" << endl << endl;
		cout << "Opcion: ";
		cin >> opc;
		cout << endl;
	} while (opc < 0 || opc > 2);

	return opc;
}

void colorFondo(int color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 15 | (color << 4));
}

void dibujaCasilla(tCasilla casilla) {
	int color;
	string texto;

	switch (casilla) {
	case Libre:
		color = 25; // azul
		texto = "  ";
		break;
	case Muro:
		color = 10; // verde
		texto = "  ";
		break;
	case DestinoL:
		color = 44; // magenta
		texto = ". ";
		break;
	case DestinoC:
		color = 38; // rojo
		texto = "[]";
		break;
	case DestinoJ:
		color = 44; // rojo
		texto = "00";
		break;
	case Jugador:
		color = Verde; // amarillo
		texto = "00";
		break;
	case Caja:
		color = 56; // gris
		texto = "()";
		break;
	}
	colorFondo(color);
	cout << texto;
	setColor(0);
}

void dibujar(const tJuego &juego) {

	cout << endl;
	cout << "Fichero: " << juego.nFichero << "   ";
	cout << "Nivel: " << juego.nivel << endl << endl;

	for (int i = 0; i < juego.sokoban.nfilas; i++) {
		for (int j = 0; j < juego.sokoban.ncolumnas; j++) {
			dibujaCasilla(juego.sokoban.tablero[i][j]);
		}
		cout << endl;
	}
	cout << endl;
	cout << "Numero de movimientos: " << juego.numMovimientos << endl << endl;
}

void inicializa(tJuego &juego) {

	juego.numMovimientos = 0;

	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			juego.sokoban.tablero[i][j] = Libre;
		}
	}
}

bool cargarJuego(tJuego &juego) {
	tPartida partida;
	tInfo info;
	tTecla tecla;
	bool cargado = false, salir = false;
	ifstream fichero;
	int nivel, opc;

	inicializa(juego);

	cout << "Nombre del fichero: ";
	cin >> juego.nFichero;
	cout << "Nivel: ";
	cin >> juego.nivel;
	cout << endl << endl;
	nivel = juego.nivel;
	system("cls");

	fichero.open(juego.nFichero + ".txt");
	if (fichero.is_open()) {

		juego.sokoban.numCajas = 0;

		if (cargarNivel(fichero, juego.sokoban, nivel)) {
			cargado = true;
			dibujar(juego);
			juego.sokoban.numCajasDestino = 0;
			juego.numMovimientos = 0;
			juego.historia.fin = 0;
			juego.historia.cont = 0;

			while (!terminado(juego) && !salir) {

				tecla = leerTecla();
				hacerMovimiento(juego, tecla);
				if (tecla == Salir) salir = true;
				system("cls");
				dibujar(juego);
				if (bloqueado(juego)) cout << "Caja bloqueada" << endl;
				if (terminado(juego)) {
					system("cls");
					cout << endl;
					cout << "------------------------" << endl;
					cout << "HAS SUPERADO EL NIVEL " << juego.nivel << endl;
					cout << "------------------------" << endl;
					system("pause");
				}
			}

			fichero.close();
		}
	}
	else {
		cout << "***ERROR AL ABRIR EL ARCHIVO***" << endl;
		system("pause");
	}

	system("cls");

	return cargado;
}

bool terminado(tJuego &juego) {
	bool fin = false;
	if (juego.sokoban.numCajasDestino == juego.sokoban.numCajas) fin = true;
	return fin;
}

bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel) {
	bool encontrado = false, ok = false;
	string fila, f, lectura;
	int l;
	sokoban.filaJugador = 0;
	sokoban.columnaJugador = 0;

	f = to_string(nivel);
	fila = "Level " + f;

	getline(fichero, lectura);

	while (lectura != fila && !ok) {
		getline(fichero, lectura);
		if (fichero.eof()) {
			cout << "Nivel no encontrado" << endl;
			system("pause");
			ok = true;
			fichero.close();
		}
	}

	if (lectura == fila) {
		encontrado = true;

		sokoban.fila = 0;
		sokoban.columna = 0;
		sokoban.ncolumnas = 0;
		getline(fichero, lectura);
		l = lectura.size();

		while (lectura != "") {

			l = lectura.size();

			if (sokoban.ncolumnas < l) sokoban.ncolumnas = l;

			for (sokoban.columna = 0; sokoban.columna < l; sokoban.columna++) {

				switch (lectura[sokoban.columna]) {
				case '#':
					sokoban.tablero[sokoban.fila][sokoban.columna] = Muro;
					break;
				case '@':
					sokoban.tablero[sokoban.fila][sokoban.columna] = Jugador;
					sokoban.filaJugador = sokoban.fila;
					sokoban.columnaJugador = sokoban.columna;
					break;
				case ' ':
					sokoban.tablero[sokoban.fila][sokoban.columna] = Libre;
					break;
				case '.':
					sokoban.tablero[sokoban.fila][sokoban.columna] = DestinoL;
					break;
				case '$':
					sokoban.tablero[sokoban.fila][sokoban.columna] = Caja;
					sokoban.numCajas++;
					break;
				case '*':
					sokoban.tablero[sokoban.fila][sokoban.columna] = DestinoC;
					break;
				case '+':
					sokoban.tablero[sokoban.fila][sokoban.columna] = DestinoJ;
					break;
				}
			}

			if (lectura != "") {
				sokoban.fila++;
				sokoban.nfilas = sokoban.fila;
				getline(fichero, lectura);
			}
		}
	}

	return encontrado;
}


tTecla leerTecla() {
	tTecla tecla;
	int dir;

	cin.sync();
	dir = _getch();
	if (dir == 0xe0) {
		dir = _getch();
		switch (dir) {
		case 72:
			tecla = Arriba;
			break;
		case 80:
			tecla = Abajo;
			break;
		case 77:
			tecla = Derecha;
			break;
		case 75:
			tecla = Izquierda;
			break;
		}
	}
	else {
		if (dir == 27) tecla = Salir;
		else if (dir == 100 || dir == 68) tecla = Deshacer;
		else tecla = Nada;
	}
	return tecla;
}

void hacerMovimiento(tJuego &juego, tTecla tecla) {
	int f, f2, c, c2;
	juego.historia.des = false;
	bool mover = true;

	switch (tecla) {
	case Arriba: f = -1; f2 = -2; c = 0; c2 = 0; break;
	case Abajo: c = 0;  c2 = 0; f = 1; f2 = 2; break;
	case Derecha: c = 1; c2 = 2; f = 0; f2 = 0; break;
	case Izquierda: c = -1; c2 = -2; f = 0; f2 = 0; break;
	case Nada: mover = false; break;
	case Salir:break;
	case Deshacer:
		mover = false;
		if (juego.numMovimientos > 0) {
			if (juego.historia.cont < MAXH) {
				if (deshacerMovimiento(juego)) {
					juego.historia.des = true;
				}
			}
			else {
				cout << char(254) << "--No puedes deshacer mas--" << char(254) << endl;
				system("pause");
				juego.historia.des = true;
			}
		}
		if (juego.numMovimientos == 0) juego.historia.des = true;
		break;
	}
	if (tecla != Deshacer) {
		juego.historia.des = false;
		if (juego.historia.cont != 0) juego.historia.cont--;
	}

	guardarPantalla(juego);

	if (mover && tecla != Salir) {
		switch (juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c]) {
			//---------------------------------------------------------------------------------------

		case Libre:

			juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] = Jugador;
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] != DestinoJ) {
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Libre;
			}
			else juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = DestinoL;
			juego.numMovimientos++;

			break;
			//---------------------------------------------------------------------------------------

		case DestinoL:

			juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] = DestinoJ;
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] != DestinoJ) {
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Libre;
			}
			else juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = DestinoL;
			juego.numMovimientos++;

			break;
			//---------------------------------------------------------------------------------------

		case Caja:

			if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == Libre ||
				juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == DestinoL) {

				if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == Libre)
					juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] = Caja;

				else if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == DestinoL) {
					juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] = DestinoC;
					juego.sokoban.numCajasDestino++;
				}

				juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] = Jugador;
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] != DestinoJ) {
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Libre;
				}
				else juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = DestinoL;
				juego.numMovimientos++;
			}
			else juego.numMovimientos++;

			break;
			//--------------------------------------------------------------------------------------------

		case DestinoC:

			if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == Libre ||
				juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == DestinoL) {

				if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == Libre) {
					juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] = Caja;
					juego.sokoban.numCajasDestino--;
				}

				else if (juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] == DestinoL)
					juego.sokoban.tablero[juego.sokoban.filaJugador + f2][juego.sokoban.columnaJugador + c2] = DestinoC;

				juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] = DestinoJ;
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] != DestinoJ) {
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Libre;
				}
				else juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = DestinoL;
				juego.numMovimientos++;
			}
			else juego.numMovimientos++;

			break;
			//--------------------------------------------------------------------------------------------

		case Muro: juego.numMovimientos++; break;
		}

		if (juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] != Muro &&
			juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] != Caja &&
			juego.sokoban.tablero[juego.sokoban.filaJugador + f][juego.sokoban.columnaJugador + c] != DestinoC) {
			juego.sokoban.filaJugador += f;
			juego.sokoban.columnaJugador += c;
		}
	}
}
//----------------------------------------------------------------------------------------------------------

//------------------------------VERSION 2-----------------------------------

bool bloqueado(const tJuego &juego) {
	bool bloqueada = false;

	for (int i = 0; i < juego.sokoban.fila; i++) {
		for (int j = 0; j < juego.sokoban.columna; j++) {

			if (juego.sokoban.tablero[i][j] == Caja) {

				if (juego.sokoban.tablero[i + 1][j] == Muro || juego.sokoban.tablero[i - 1][j] == Muro) {
					if (juego.sokoban.tablero[i][j - 1] == Muro || juego.sokoban.tablero[i][j + 1] == Muro) {
						bloqueada = true;
					}
				}
			}
		}
	}
	return bloqueada;
}

bool deshacerMovimiento(tJuego &juego) {
	bool deshecho = false;
	juego.historia.fin = (MAXH + juego.historia.fin - 1) % MAXH;

	for (int i = 0; i < juego.sokoban.nfilas; i++) {
		for (int j = 0; j < juego.sokoban.ncolumnas; j++) {
			juego.sokoban.tablero[i][j] = juego.historia.tab[juego.historia.fin].tablero[i][j];
		}
		deshecho = true;
	}

	juego.sokoban.filaJugador = juego.historia.tab[juego.historia.fin].filaJugador;
	juego.sokoban.columnaJugador = juego.historia.tab[juego.historia.fin].columnaJugador;
	juego.sokoban.nfilas = juego.historia.tab[juego.historia.fin].nfilas;
	juego.sokoban.ncolumnas = juego.historia.tab[juego.historia.fin].ncolumnas;
	juego.sokoban.numCajas = juego.historia.tab[juego.historia.fin].numCajas;
	juego.sokoban.numCajasDestino = juego.historia.tab[juego.historia.fin].numCajasDestino;

	juego.historia.cont++;
	juego.numMovimientos--;

	return deshecho;
}

void guardarPantalla(tJuego &juego) {

	for (int i = 0; i < juego.sokoban.nfilas; i++) {
		for (int j = 0; j < juego.sokoban.ncolumnas; j++) {
			juego.historia.tab[juego.historia.fin].tablero[i][j] = juego.sokoban.tablero[i][j];
		}
	}

	juego.historia.tab[juego.historia.fin].filaJugador = juego.sokoban.filaJugador;
	juego.historia.tab[juego.historia.fin].columnaJugador = juego.sokoban.columnaJugador;
	juego.historia.tab[juego.historia.fin].nfilas = juego.sokoban.nfilas;
	juego.historia.tab[juego.historia.fin].ncolumnas = juego.sokoban.ncolumnas;
	juego.historia.tab[juego.historia.fin].numCajas = juego.sokoban.numCajas;
	juego.historia.tab[juego.historia.fin].numCajasDestino = juego.sokoban.numCajasDestino;

	if (!juego.historia.des) juego.historia.fin = (juego.historia.fin + 1) % MAXH;

}
//----------------------------------------------------------------------------------------------------------



//-----------------------------------VERSION 3-----------------------------------

void guardarFichero(tInfo info) {
	ofstream fich;
	fich.open(info.nombre + ".txt");
	for (int i = 0; i < info.contR; i++) {
		fich << info.exitos[i].nombreFich << " " << info.exitos[i].nivel << " " << info.exitos[i].numM << endl;
	}
}


void mostrarPartida(tInfo info) {
	cout << "Jugador: " << info.nombre << endl << endl;
	cout << "Archivo" << setw(13) << "Nivel" << setw(19) << "Movimientos" << endl;
	for (int i = 0; i < info.contR; i++) {
		cout << info.exitos[i].nombreFich << setw(13) << info.exitos[i].nivel << setw(19) << info.exitos[i].numM << endl;
	}
}


bool& operator==(tJuego const &juego, tInfo &info) {
	bool ok = false;
	int i = 0;

	while (!ok && i < info.contR) {
		if (juego.nFichero == info.exitos[i].nombreFich && juego.nivel == info.exitos[i].nivel) {
			ok = true;
			if (juego.numMovimientos < info.exitos[i].numM) info.exitos[i].numM = juego.numMovimientos;
		}
		i++;
	}
	return ok;
}

void operator<(tJuego const &juego, tInfo &info) {
	int pos = 0;
	while ((pos < info.contR) && juego.nFichero > info.exitos[pos].nombreFich) pos++;

	if (juego.nFichero == info.exitos[pos].nombreFich) {
		while ((pos < info.contR) && juego.nivel > info.exitos[pos].nivel) pos++;
	}

	for (int i = pos; i < info.contR; i++) {
		info.exitos[i + 1].nombreFich = info.exitos[i].nombreFich;
		info.exitos[i + 1].nivel = info.exitos[i].nivel;
		info.exitos[i + 1].numM = info.exitos[i].numM;
	}

	info.exitos[pos].nombreFich = juego.nFichero;
	info.exitos[pos].nivel = juego.nivel;
	info.exitos[pos].numM = juego.numMovimientos;
	info.contR++;
}

