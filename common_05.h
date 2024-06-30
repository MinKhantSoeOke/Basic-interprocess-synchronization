#ifndef COMMON_05_H_
#define COMMON_05_H_

#define ATTACH_POINT "calculator_service"

#define MSG_TYPE_ORDER 'o'
#define MSG_TYPE_REGISTER 'r'
#define MSG_TYPE_R 'R'
#define MSG_TYPE_ANSWER 'a'
#define MSG_TYPE_SINE 's'

#define OP_ADD '+'
#define OP_SUB '-'
#define OP_MUL '*'
#define OP_DIV '/'
#define OP_SINE 's'

typedef struct {
    struct _pulse pulse;
    char msg_type;
    char operation;
    int arg1, arg2;
    int result;
    int rcvid, chid, ss_pid, p_code;
} myMsg_t;

typedef struct {
    char msg_type;
    char operation;
    int client_rcvid, ss_coid, p_code;
} registration;

#endif /* COMMON_05_H_ */