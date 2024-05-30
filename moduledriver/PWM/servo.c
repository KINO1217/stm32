#include "servo.h"
#include "stdlib.h"
#include "string.h"
#include "my_pwm.h"
#include "math.h"

typedef struct __MSERVO
{
    u8 timId;
    u8 chId;
}m_servo;

static void set_angle(const c_servo* this, float angle);

c_servo servo_create(u8 timId,u8 channel)
{
    c_servo new = {0};

    /**为c_servo结构体赋值*/
    new.this = malloc(sizeof(m_servo));
    ((m_servo*)new.this)->timId = timId;
    ((m_servo*)new.this)->chId = channel;
    new.set_angle = set_angle;

    /**初始化PWM*/
    my_pwm.timInit(timId,72,20000);
    my_pwm.chInit(timId,channel);
    my_pwm.chSet(timId,channel,500);

    return new;
}

static void set_angle(const c_servo* this, float angle)
{
    m_servo* m_this = {0};
    m_this = (m_servo*)this->this;

    u16 duty = 0;
    duty = (u16)round(11.111111*angle + 500);
    my_pwm.chSet(m_this->timId,m_this->chId,duty);
}
