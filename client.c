#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include "common_05.h"

int main(int argc, char *argv[]) {
    char operation, c;
    int num1 = 0, num2 = 0, argsNum;
    myMsg_t send_msg, reply_msg;
    int server_coid;

    if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
        perror("name_open failed - server not found!");
        return EXIT_FAILURE;
    }
    printf("rtos-03-client started, server found\n");

    while (1) {
        printf("Enter [number] [operator] [number] or just [operator] for sine: ");
        fflush(stdout);
        argsNum = scanf("%d %c %d", &num1, &operation, &num2);

        if (argsNum == EOF || operation == 'q') // exit condition
            break;

        // Check for sine operation without arguments
        if (argsNum == 1 && operation == 's') {
            printf("Requesting sine value\n");

            send_msg.operation = operation;
            send_msg.msg_type = MSG_TYPE_ORDER;

            if (MsgSend(server_coid, &send_msg, sizeof(send_msg), &reply_msg, sizeof(reply_msg)) == -1) {
                perror("MsgSend Failed");
                break;
            }

            printf("    ... result of sine operation is %f ...\n", (double)reply_msg.result / 10000.0); // Assume result is scaled
            continue;
        }

        // Normal arithmetic operations
        if (argsNum == 3 && strchr("*+-/s", operation)) {
            printf(" >> Thanks, you entered: %d %c %d\n", num1, operation, num2);

            send_msg.arg1 = num1;
            send_msg.arg2 = num2;
            send_msg.operation = operation;
            send_msg.msg_type = MSG_TYPE_ORDER;

            if (MsgSend(server_coid, &send_msg, sizeof(send_msg), &reply_msg, sizeof(reply_msg)) == -1) {
                perror("MsgSend Failed");
                break;
            }

            printf("    MsgSend(... &send_msg, ..., &reply_msg, ...);\n");
            printf("    ... result of operation is %d ...\n", reply_msg.result);
        } else {
            printf(" >> Operation %c is not valid or input format is wrong. Try again - \n", operation);
            fflush(stdout);
            while ((c = getchar()) != '\n' && c != EOF) /* discard */;
        }
    }

    printf("Console client exits correctly\n");
    return EXIT_SUCCESS;
}
