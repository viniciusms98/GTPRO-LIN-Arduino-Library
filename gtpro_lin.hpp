#ifndef GTPRO_LIN_H
#define GTPRO_LIN_H
#include <arduino.h>

class comunicacaoLin
{
public:
    comunicacaoLin(uint8_t pin_tx, uint8_t pin_rx, uint8_t pin_desable, uint8_t tx_speed, uint8_t id_esp, uint8_t id_tx = 0x04);
    void
        rxLoop(),
        setLoopFunction(void (*received_function)(uint8_t *data_received)),
        send(uint8_t *data, uint8_t id_tx = 0);   
    uint8_t checkSumCalc(uint8_t *data, uint8_t length);

private:
    uint8_t
        pin_tx,
        pin_rx,
        pin_desable,
        tx_speed,
        id_esp,
        id_tx;

    void (*received_function)(uint8_t *data_received) = nullptr;
};

#endif