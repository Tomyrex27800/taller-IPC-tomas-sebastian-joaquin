#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>

using namespace std;

mutex mtx;
condition_variable cv;
string palabra_actual;

vector<string> palabras = {"mariposa","computadora","pelota","murciélago","caracol","helicóptero","sandía","micrófono","jirafa","avión","astronauta","escalera","planeta","sombrero","telescopio","lámpara","pirámide","camaleón","volcán","tren","canguro","tigre","elefante","guitarra"};

int turno = 0;

// generador por hilo (semilla distinta por hilo)
thread_local std::mt19937 rng(
    std::random_device{}() ^
    (uint32_t)std::hash<std::thread::id>{}(std::this_thread::get_id())
);

string generar_palabra() {
    std::uniform_int_distribution<int> dist(1, (int)palabras.size()-1);
    return palabras[dist(rng)];
}

void telefono_descompuesto(int id) {
    // esperar turno
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [&]{ return turno == id; });

    // generar palabra SIN soltar el turno pero SIN usar rand global
    if (id == 0) {
        palabra_actual = generar_palabra();
        cout << "Hilo " << id << " inicia con: " << palabra_actual << endl;
    } else {
        string nueva = generar_palabra();
        palabra_actual = palabra_actual + " " + nueva;
        cout << "Hilo " << id << " recibe y cambia a: " << palabra_actual << endl;
    }

    turno = (turno + 1) % 5; // siguiente turno
    lock.unlock();
    cv.notify_all();
}

void crear_threads(int n){
    vector<thread> hilos;
    for (int i = 0; i < 100; i++) {
        hilos.push_back(i); // el vector se agranda varias veces en el proceso
    }
    for (auto &h : hilos) h.join();
    cout << "Palabra final: " << palabra_actual << endl;
}

int main() {
    cout << "Ejercicio 1: Telefono descompuesto" << endl;
    int n_threads;
    cout << "Introduce el numero de hilos: ";
    cin >> n_threads;
    crear_threads(n_threads);
    return 0;
}
