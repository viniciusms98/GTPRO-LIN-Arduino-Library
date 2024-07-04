#include "gtpro_lin.hpp"

// - Criação de objeto
comunicacaoLin::comunicacaoLin(uint8_t pin_tx, uint8_t pin_rx, uint8_t pin_desable, uint8_t tx_speed, uint8_t id_esp, uint8_t id_tx = 0x04)
{
    this->pin_tx = pin_tx;
    this->pin_rx = pin_rx;
    this->pin_desable = pin_desable;
    this->tx_speed = tx_speed;
    this->id_esp = id_esp;
    this->id_tx = id_tx;
    pinMode(pin_desable, OUTPUT); // Define o pin_desable como saída
    Serial1.begin(tx_speed);      // Inicia o Serial1 na velocidade definida em tx_speed
}

// Função de  que executa as funções de recepção dos dados
void comunicacaoLin::rxLoop()
{
    if (!Serial1.available()) // Inicia a configuração de recepção dos dados e verifica se houve erro
    {
        return; // Em caso de erro sai da função
    }

    uint8_t *data_received = new uint8_t[15]; // Criação de Ponteiro de Dados

    Serial1.readBytes(data_received, 15); // Armazena os dados recebidos no ponteiro data_received
    if (data_received[0] != id_tx)
    {
        delete[] data_received;
        return;
    }

    data_received[0] = 0;

    uint8_t checkSumRx = data_received[14];
    data_received[14] = 0;

    uint8_t sum = checkSumCalc(data_received, 14); // Calculando a soma dos dado de informação

    if (checkSumRx == sum) // Verifica se os identificadores e soma estão corretos
    {
        received_function(data_received); // Se ocorrer tudo corretamente, retorna ponteiro de dados de informação
    }

    delete[] data_received;
}

// Funções de Recepção

uint8_t comunicacaoLin::checkSumCalc(uint8_t *data, uint8_t length)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        sum += data[i];
        // Verifica se houve overflow e ajusta
        if (sum > 0xFF)
        {
            sum -= 0xFF;
        }
    }
    // O checksum é o complemento de dois da soma
    return ~sum & 0xFF;
}

void comunicacaoLin::setLoopFunction(void (*received_function)(uint8_t *data_received))
{
    this->received_function = received_function;
}

// Função de  que executa as funções de envio dos dados
void comunicacaoLin::send(uint8_t *data, uint8_t id_tx = 0)
{
    digitalWrite(pin_desable, LOW);
    if (id_tx)
        Serial1.write(id_tx);
    else
        Serial1.write(this-> id_tx);
    Serial1.write(data, 13);
    Serial1.write(checkSumCalc(data, 12));
    digitalWrite(pin_desable, HIGH);
}
