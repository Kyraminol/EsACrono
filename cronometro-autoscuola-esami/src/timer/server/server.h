#ifndef TIMERSERVER_H
#define TIMERSERVER_H


class TimerServer{
    public:
        TimerServer();
        void setup();
        void loop();
    private:
        int timers[2] = {0, 0};
        float results[2];
        void timerStart(int timer);
        void timerStop(int timer);
};

#endif
