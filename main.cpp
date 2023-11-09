#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

// Mutex para garantir exclusão mútua ao acessar variáveis compartilhadas
mutex mtx;

// Variáveis compartilhadas
int totalClientesAtendidos = 0;
int clientesTatuador = 0;
int clientesPiercer = 0;
int clientesSemAtendimento = 0;
int clientesAtendidosSemEspera = 0;

// Função para simular o atendimento
void atenderCliente(string profissional, int clienteNum, int atendimentoMin, int atendimentoMax) {
    {
        lock_guard<mutex> lock(mtx);
        cout << profissional << " iniciou o atendimento do cliente " << clienteNum << endl;
    }

    // Simulando o tempo de atendimento
    int tempoAtendimento = rand() % (atendimentoMax - atendimentoMin + 1) + atendimentoMin;
    usleep(tempoAtendimento * 1000);  // Convertendo minutos para milissegundos

    {
        lock_guard<mutex> lock(mtx);
        cout << profissional << " finalizou o atendimento do cliente " << clienteNum << endl;
        totalClientesAtendidos++;
        if (profissional == "Tatuador") {
            clientesTatuador++;
        } else {
            clientesPiercer++;
        }
    }
}

// Função para simular a chegada de clientes
void chegadaClientes(int numClientes, int pufsTatuagem, int pufsPiercing, float probTatuagem) {
    srand(time(nullptr));

    for (int i = 1; i <= numClientes; ++i) {
        // Simulando intervalo de chegada
        usleep(100 * 1000);  // Convertendo minutos para milissegundos

        // Determinando o que o cliente deseja fazer
        bool atendimentoTatuagem = ((float)rand() / RAND_MAX) <= probTatuagem;

        {
            lock_guard<mutex> lock(mtx);
            if (atendimentoTatuagem) {
                cout << "Cliente " << i << " chegou para tatuagem ";
                if (pufsTatuagem > 0) {
                    cout << "e foi atendido imediatamente." << endl;
                    clientesAtendidosSemEspera++;
                    pufsTatuagem--;
                } else {
                    cout << "e foi sentar em um puf de espera." << endl;
                }
            } else {
                cout << "Cliente " << i << " chegou para body piercing ";
                if (pufsPiercing > 0) {
                    cout << "e foi atendido imediatamente." << endl;
                    clientesAtendidosSemEspera++;
                    pufsPiercing--;
                } else {
                    cout << "e foi sentar em um puf de espera." << endl;
                }
            }
        }

        if (!atendimentoTatuagem) {
            thread(atenderCliente, "Body Piercer", i, 100, 500).detach();
        } else {
            thread(atenderCliente, "Tatuador", i, 100, 500).detach();
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        cerr << "Uso correto: ./esperta_simulador CLI PT PBP PAT MINATEN MAXATEN" << endl;
        return 1;
    }

    int numClientes = atoi(argv[1]);
    int pufsTatuagem = atoi(argv[2]);
    int pufsPiercing = atoi(argv[3]);
    float probTatuagem = atof(argv[4]);
    int minAtendimento = atoi(argv[5]);
    int maxAtendimento = atoi(argv[6]);

    // Iniciando o gerador de números aleatórios
    srand(time(nullptr));

    // Criando thread para chegada de clientes
    thread(chegadaClientes, numClientes, pufsTatuagem, pufsPiercing, probTatuagem).detach();

    // Aguardando a finalização da chegada de clientes
    usleep(numClientes * 100 * 1000);  // Convertendo minutos para milissegundos

    // Mostrando estatísticas finais
    cout << "\n--- Estatísticas Finais ---" << endl;
    cout << "Total de clientes atendidos: " << totalClientesAtendidos << endl;
    cout << "Clientes atendidos pelo Tatuador: " << clientesTatuador << endl;
    cout << "Clientes atendidos pelo Body Piercer: " << clientesPiercer << endl;
    cout << "Clientes que foram embora sem serem atendidos: " << clientesSemAtendimento << endl;
    cout << "Clientes atendidos sem esperar nos pufs: " << clientesAtendidosSemEspera << endl;

    return 0;
}