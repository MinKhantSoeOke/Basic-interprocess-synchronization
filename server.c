#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include "common_05.h"

registration registrations[100];

void register_client(myMsg_t msg, int rcvid);

int main() {
    int rcvid, i, ss_coid;
    name_attach_t *attach;
    int find_status = 0;

    if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
        perror("Failed to attach name");
        return EXIT_FAILURE;
    }

    printf("Main server is running.\n");

    while (1) {
        myMsg_t msg;
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

        if (rcvid == -1) { /* Error condition, exit */
            perror("MsgReceive failed");
            break;
        } else if (rcvid == 0) { /* Pulse received */
            printf("Received a pulse\n");
            continue;
        }

        switch (msg.msg_type) {
            case MSG_TYPE_ORDER:
                printf("Main server received order: %d %c %d\n", msg.arg1, msg.operation, msg.arg2);
                for (i = 0; i < 100; i++) {
                    if (registrations[i].operation == msg.operation) {
                        if (registrations[i].msg_type == MSG_TYPE_R) {
                            // Pulse-triggered worker
                            MsgSendPulse(registrations[i].ss_coid, -1, registrations[i].p_code, rcvid);
                        } else if (registrations[i].msg_type == MSG_TYPE_REGISTER && msg.operation == OP_SINE) {
                            // Signal-triggered sine worker
                            if (kill(registrations[i].ss_coid, registrations[i].p_code) == -1) {
                                perror("Signal to sine worker failed");
                            }
                            // Store the rcvid for later use when the sine worker sends the result
                            registrations[i].client_rcvid = rcvid;
                        } else {
                            msg.rcvid = rcvid;
                            MsgReply(registrations[i].client_rcvid, 0, &msg, sizeof(msg));
                        }
                        find_status = 1;
                    }
                }
                if (find_status == 0) {
                    // Operation not supported, send error response
                    myMsg_t resp;
                    resp.msg_type = 'e';
                    MsgReply(rcvid, EOK, &resp, sizeof(resp));
                }
                find_status = 0;
                break;

            case MSG_TYPE_REGISTER:
                register_client(msg, rcvid);
                printf("Main server received registration: %c\n", msg.operation);
                break;

            case MSG_TYPE_R:
                ss_coid = ConnectAttach(0, msg.ss_pid, msg.chid, _NTO_SIDE_CHANNEL, 0);
                for (i = 0; i < 100; i++) {
                    if (registrations[i].client_rcvid == 0) { // Assuming 0 is unused
                        registrations[i].msg_type = msg.msg_type;
                        registrations[i].operation = msg.operation;
                        registrations[i].client_rcvid = rcvid;
                        registrations[i].ss_coid = ss_coid;
                        registrations[i].p_code = msg.p_code;
                        MsgReply(rcvid, 0, 0, 0);
                        break;
                    }
                }
                printf("Main server received registration: %c and Channel Attached.\n", msg.operation);
                break;

            case MSG_TYPE_ANSWER:
                printf("Main server received answer: %d\n", msg.result);
                // Respond to the client that requested the sine value

                MsgReply(msg.rcvid, 0, &msg, sizeof(msg));

                break;

            default:
                printf("Unknown message type received: %d\n", msg.msg_type);
                break;
        }
    }

    name_detach(attach, 0);
    return EXIT_SUCCESS;
}

void register_client(myMsg_t msg, int rcvid) {
    int i;
    for (i = 0; i < 100; i++) {
        if (registrations[i].client_rcvid == 0) { // Assuming 0 is unused
            registrations[i].msg_type = msg.msg_type;
            registrations[i].operation = msg.operation;
            registrations[i].client_rcvid = rcvid;
            registrations[i].ss_coid = msg.ss_pid;
            registrations[i].p_code = msg.p_code;
            break;
        }
    }
}
