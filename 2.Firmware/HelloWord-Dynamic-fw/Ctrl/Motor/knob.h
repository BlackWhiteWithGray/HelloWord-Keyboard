#ifndef HELLOWORD_DYNAMIC_FW_KNOB_H
#define HELLOWORD_DYNAMIC_FW_KNOB_H

#include "motor.h"


class KnobSimulator
{
public:
    KnobSimulator() = default;;

    enum Mode_t
    {
        MODE_INIT = 0,
        MODE_CHOOSE,
        MODE_VOLUME,
        MODE_BRIGHT,
        MODE_MOVE,
        MODE_PAGE,
    };

    void Init(Motor* _motor);
    void Tick();
    void SetEnable(bool _en);
    void ApplyZeroPos(float _angle = 0);
    void SetMode(Mode_t _mode);
    void SetLimitPos(float _min, float _max);
    float GetPosition();
    float GetVelocity();
    int GetEncoderModePos();


private:
    Motor* motor{};
    Mode_t mode = MODE_INIT;
    float zeroPosition = 0;
    float limitPositionMax = 3.0;
    float limitPositionMin = 1.5;
    int encoderDivides = 36;
    int encoderPosition = 0;

    float lastAngle;
    float lastVelocity;
};

#endif //HELLOWORD_DYNAMIC_FW_KNOB_H
