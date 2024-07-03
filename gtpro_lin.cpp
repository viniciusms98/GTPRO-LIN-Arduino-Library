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
}

uint8_t comunicacaoLin::function_sum(uint8_t *data, uint8_t length)
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

uint8_t comunicacaoLin::function_dataSeparation(uint8_t *data, uint8_t length)
{

    uint8_t data_received[length-3];

    for (uint8_t i = 0; i < length-3; i++)
    {
        data_received[i] = data[i+2];
    }
    return data_received[12];
}

uint8_t* comunicacaoLin::function_concatenateData(uint8_t id_esp, uint8_t id_tx, uint8_t *data){

    uint8_t concatenatedData [15];
    concatenatedData[0] = id_esp;
    concatenatedData[1] = id_tx;

    for (uint8_t i = 2; i < 12; i++){
        concatenatedData[i] = data[i-2];
    }
    concatenatedData[14] = function_sum(data, 12);
    concatenatedData[15] = 0;
    

    return concatenatedData;
}
// - Inicialização da recepção
void comunicacaoLin::begin()
{
    Serial1.begin(tx_speed);

    pinMode(pin_desable, OUTPUT);
}

uint8_t comunicacaoLin::send(uint8_t *data, uint8_t data_length)
{   
    digitalWrite(pin_desable, LOW);
    Serial1.write(data, data_length);
    digitalWrite(pin_desable, HIGH);
    return 0;
}

// Definição da função de loop
void comunicacaoLin::setLoopFunction(void (*received_function)(uint8_t *data_received))
{
    this->received_function = received_function;
}

uint8_t comunicacaoLin::loop(void (*received_function)(uint8_t *data_received))
{
    if (this->received_function != nullptr)
    {
        this->received_function = received_function;
    }
    if (this->received_function == nullptr)
    {
        return 1; // Erro - No function received
    }

    if (Serial1.available())
    {
        uint8_t *data_received;

        data_received = new uint8_t[15];

        Serial1.readBytes(data_received, 15);

        if (data_received[14] != 0 && data_received[0] == id_tx)
        {
            uint8_t dataSepareted = function_dataSeparation(data_received, 15);
            uint8_t checksum = function_sum(&dataSepareted, 12);

            if (data_received[1] == id_esp && data_received[1] == id_tx && checksum == data_received[14])
            {
                this->received_function(data_received);
                send(data_received, 15);

                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }

        return 0;
    }
}