#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include "common_05.h"

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL
#define MY_PULSE_CODE1 _PULSE_CODE_MINAVAIL+1

double current_sine_value = 0.0;

void sine_calculator(timer_t timerid) {
    static double t = 0.0;
    current_sine_value = sin(t);
    t += 0.01;
    // Scale the sine value for integer transmission
    current_sine_value *= 10000;
}

void setup_timer(timer_t *timerid, int chid) {
    struct sigevent event;
    struct itimerspec itime;

    event.sigev_notify = SIGEV_PULSE;
    event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
    event.sigev_priority = getprio(0);
    event.sigev_code = MY_PULSE_CODE;

    if (timer_create(CLOCK_REALTIME, &event, timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 10000000; // 10ms
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = 10000000;

    if (timer_settime(*timerid, 0, &itime, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }
}

int main() {
    timer_t timerid;
    int chid, rcvid;
    myMsg_t msg;
    struct _pulse pulse;

    chid = ChannelCreate(0);
    if (chid == -1) {
        perror("ChannelCreate");
        exit(EXIT_FAILURE);
    }

    int server_coid = name_open(ATTACH_POINT, 0);
    if (server_coid == -1) {
        perror("name_open failed - server not found!");
        return EXIT_FAILURE;
    }

    myMsg_t reg_msg;
    reg_msg.msg_type = MSG_TYPE_R;
    reg_msg.operation = OP_SINE;
    reg_msg.chid = chid;
    reg_msg.ss_pid = getpid();
    reg_msg.p_code = MY_PULSE_CODE1;
    MsgSend(server_coid, &reg_msg, sizeof(reg_msg), &reg_msg, sizeof(reg_msg));

    setup_timer(&timerid, chid);

    for (;;) {
        rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

        if (rcvid == 0) { // Pulse received
            if (msg.pulse.code == MY_PULSE_CODE1) {
                printf("Sine client received pulse to send sine value. %f\n", current_sine_value);

                myMsg_t response;
                response.msg_type = MSG_TYPE_ANSWER;
                response.result = (int)(current_sine_value * 100);
                response.rcvid = msg.pulse.value.sival_int;
                if (MsgSend(server_coid, &response, sizeof(response), &response, sizeof(response)) == -1) {
                    perror("MsgSend Failed");
                    break;
                }
            } else if (msg.pulse.code == MY_PULSE_CODE) {
                sine_calculator(timerid);
                //printf("Current Sine %f\n", current_sine_value);
            }
        }
    }

    timer_delete(timerid);
    ChannelDestroy(chid);
    return 0;
}