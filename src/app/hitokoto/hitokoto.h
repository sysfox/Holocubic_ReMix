#ifndef _HITOKOTO_H
#define _HITOKOTO_H

#include "sys/interface.h"

class Hitokoto : public AppController
{
private:
    int deepsleep_time;  // 休眠时间，秒

public:
    Hitokoto();
    ~Hitokoto();
    void app_init(void);
    void load_image(void);
    void app_process(AppController *sys_app);
    void app_exit_callback(void);
    void on_btn_update(void);
};

#endif 