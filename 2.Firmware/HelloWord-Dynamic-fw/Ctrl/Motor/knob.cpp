#include <cmath>
#include <cstdio>
#include "knob.h"
#include "st_hardware.h"
#include "usart.h"
#include "interface_uart.hpp"
extern uint8_t choose;
uint16_t time = 0;

void KnobSimulator::Init(Motor* _motor)
{
    motor = _motor;
    motor->config.controlMode = Motor::ControlMode_t::TORQUE;
    motor->config.voltageLimit = 3.0;
    motor->config.velocityLimit = 100;
    motor->config.pidVelocity.p = 0.1;
    motor->config.pidVelocity.i = 0.0;
    motor->config.pidVelocity.d = 0.0;
    motor->config.pidAngle.p = 80;
    motor->config.pidAngle.i = 0;
    motor->config.pidAngle.d = 0.7;

    // When motor calibrated, we can replace Init with something like:

    if (motor->Init())//5.532302,EncoderBase::CCW))
    {
        printf("ZeroElectricAngleOffset: %f | Encoder direction: %s\n", motor->zeroElectricAngleOffset,
               motor->encoder->countDirection == EncoderBase::CW ? "CW" : "CCW");
        motor->target = limitPositionMax;
        motor->SetEnable(false);
    } else
        printf("Error[%d]\n", motor->error);

}

void KnobSimulator::SetMode(KnobSimulator::Mode_t _mode)
{
    mode = _mode;
    lastAngle = GetPosition();
    lastVelocity = GetVelocity();
    switch (mode)
    {
        case MODE_INIT:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(1.5);
            motor->config.controlMode = Motor::ControlMode_t::ANGLE;
            motor->config.pidVelocity.p = 0.1;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->config.pidAngle.p = 100;
            motor->config.pidAngle.i = 0;
            motor->config.pidAngle.d = 3.0;
            motor->target = limitPositionMax;
        }
            break;
        case MODE_CHOOSE:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(1.5);
            motor->config.controlMode = Motor::ControlMode_t::ANGLE;
            motor->config.pidVelocity.p = 0.1;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->config.pidAngle.p = 100;
            motor->config.pidAngle.i = 0;
            motor->config.pidAngle.d = 3.0;
            motor->target = limitPositionMax;
        }
            break;
        case MODE_VOLUME:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(1.5);
            motor->config.controlMode = Motor::ControlMode_t::ANGLE;
            motor->config.pidVelocity.p = 0.1;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->config.pidAngle.p = 80;
            motor->config.pidAngle.i = 0;
            motor->config.pidAngle.d = 3.0;
            motor->target = (limitPositionMax+limitPositionMin)/2;
            lastAngle = (limitPositionMax+limitPositionMin)/2;
        }
            break;
        case MODE_BRIGHT:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(1.5);
            motor->config.controlMode = Motor::ControlMode_t::VELOCITY;
            motor->config.pidVelocity.p = 0.1;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->config.pidAngle.p = 40;
            motor->config.pidAngle.i = 0;
            motor->config.pidAngle.d = 2;
            motor->target = 0;
        }
            break;
        case MODE_MOVE:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(6.0);
            motor->config.controlMode = Motor::VELOCITY;
            motor->config.pidVelocity.p = 5.0;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->target = 0;
        }
            break;
        case MODE_PAGE:
        {
            motor->SetEnable(true);
            motor->SetTorqueLimit(1.5);
            motor->config.controlMode = Motor::ControlMode_t::ANGLE;
            motor->config.pidVelocity.p = 0.1;
            motor->config.pidVelocity.i = 0.0;
            motor->config.pidVelocity.d = 0.0;
            motor->config.pidAngle.p = 100;
            motor->config.pidAngle.i = 0;
            motor->config.pidAngle.d = 3.5;
            motor->target = (limitPositionMax+limitPositionMin)/2;
        }
            break;
    }
}


void KnobSimulator::Tick()
{
    uint8_t message[] = {0xfe,0x00,0x00,0xef};

    switch (mode) {
        case MODE_INIT: {
            auto a = GetPosition();
            motor->target = limitPositionMax;
            switch (choose) {
                case 0: {
                    motor->target = limitPositionMax;
                    if (fabsf(a - limitPositionMax) < 0.05) {
                        mode = MODE_CHOOSE;
                        SetMode(static_cast<KnobSimulator::Mode_t>(mode));
                    }
                }
                    break;
                case 1: {
                    motor->target = limitPositionMax - 0.5F;
                    if (fabsf(a - limitPositionMax + 0.5F) < 0.05) {
                        mode = MODE_CHOOSE;
                        SetMode(static_cast<KnobSimulator::Mode_t>(mode));
                    }
                }
                    break;
                case 2: {
                    motor->target = limitPositionMax - 1;
                    if (fabsf(a - limitPositionMax + 1) < 0.05) {
                        mode = MODE_CHOOSE;
                        SetMode(static_cast<KnobSimulator::Mode_t>(mode));
                    }
                }
                    break;
                case 3: {
                    motor->target = limitPositionMin;
                    if (fabsf(a - limitPositionMin) < 0.05)
                    {
                        mode = MODE_CHOOSE;
                        SetMode(static_cast<KnobSimulator::Mode_t>(mode));
                    }
                }
            }
        }
            break;
        case MODE_CHOOSE: {
            auto a = GetPosition();
            if (a > limitPositionMax - 0.25) {
                motor->target = limitPositionMax;
                choose = 0;
            } else if (a > limitPositionMax - 0.75) {
                motor->target = limitPositionMax - 0.5F;
                choose = 1;
            } else if (a > limitPositionMax - 1.25) {
                motor->target = limitPositionMax - 1;
                choose = 2;
            } else if (a < limitPositionMin + 0.25) {
                motor->target = limitPositionMin;
                choose = 3;
            }
        }
            break;
        case MODE_VOLUME: {
            auto a = GetPosition();
            if (a - lastAngle > _PI / (float) encoderDivides) {
                motor->target += _2PI / (float) encoderDivides;
                lastAngle = motor->target;
                encoderPosition++;
                message[1] = 0x01;
                message[2] = 0x01;
                HAL_UART_Transmit(&huart2, (uint8_t *) message, sizeof(message), 1);
            } else if (a - lastAngle < -_PI / (float) encoderDivides) {
                motor->target -= _2PI / (float) encoderDivides;
                lastAngle = motor->target;
                encoderPosition--;
                message[1] = 0x01;
                message[2] = 0x02;
                HAL_UART_Transmit(&huart2, (uint8_t *) message, sizeof(message), 1);
            }
        }
            break;
        case MODE_BRIGHT: {
            if (limitPositionMax != 0 && limitPositionMin != 0) {
                auto a = GetPosition();
                message[1] = 0x02;
                if (a > limitPositionMax) {
                    motor->config.controlMode = Motor::ANGLE;
                    motor->target = limitPositionMax;
                } else if (a < limitPositionMin) {
                    motor->config.controlMode = Motor::ANGLE;
                    motor->target = limitPositionMin;
                } else {
                    motor->config.controlMode = Motor::VELOCITY;
                    motor->target = 0;
                }

                a = (a-1.5F)*66.7F;
                if(a<0)a=0;
                else if(a>100)a=100;

                if(time>200) {
                    time = 0;
                    message[1] = 0x02;
                    message[2] = (uint8_t)a;
                    HAL_UART_Transmit(&huart2, (uint8_t *) message, sizeof(message), 1);
                }
                time++;
            }
        }
            break;
        case MODE_MOVE:
        {
            auto v = GetVelocity();
            auto a = GetPosition();
            if(fabsf(v-lastVelocity)>0.5)motor->target = v;
            else if(fabsf(v-lastVelocity)<0.08)motor->target = v*0.001F+lastVelocity*0.999F;
            else motor->target = v*fabsf(v-lastVelocity)*0.5F+lastVelocity*(1-fabsf(v-lastVelocity)*0.5F);
            lastVelocity = motor->target;

            if(fabsf(lastAngle-a)>0.2) {
                if (v > 16)v = 16;
                else if (v < -16)v = -16;
                if (v > -0.05 && v < 0.05)v = 0;
                else if (v < 1 && v > 0)v = 1;
                else if (v < 0 && v > -1)v = -1;
                time = 0;
                message[1] = 0x03;
                message[2] = (uint8_t) (v + 128);
                HAL_UART_Transmit(&huart2, (uint8_t *) message, sizeof(message), 1);
                lastAngle = a;
            }

            time++;
        }
            break;
        case MODE_PAGE:
        {
            auto a = GetPosition();
            if (a - lastAngle > _PI / (float) encoderDivides)
            {
                motor->target += _2PI / (float) encoderDivides;
                lastAngle = motor->target;
                encoderPosition++;
            } else if (a - lastAngle < -_PI / (float) encoderDivides)
            {
                motor->target -= _2PI / (float) encoderDivides;
                lastAngle = motor->target;
                encoderPosition--;
            }
        }
            break;
    }

    motor->Tick();
}


void KnobSimulator::SetLimitPos(float _min, float _max)
{
    limitPositionMin = _min;
    limitPositionMax = _max;
}


void KnobSimulator::ApplyZeroPos(float _angle)
{
    if (_angle != 0)
        zeroPosition = _angle;
    else
        zeroPosition = motor->GetEstimateAngle();
}


float KnobSimulator::GetPosition()
{
    return motor->GetEstimateAngle() - zeroPosition;
}

float KnobSimulator::GetVelocity()
{
    return motor->GetEstimateVelocity();
}


int KnobSimulator::GetEncoderModePos()
{
    return std::lround(GetPosition() / (_2PI / (float) encoderDivides));
}


void KnobSimulator::SetEnable(bool _en)
{
    SetPowerMotor(_en);
    motor->SetEnable(_en);
}

