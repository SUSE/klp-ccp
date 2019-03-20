typedef struct tux_req_struct tux_req_t;
struct tux_req_struct
{
        struct socket *sock;
        char usermode;
        char *userbuf;
        unsigned int userlen;
        char error;
        void *private;
};
void user_send_buffer (tux_req_t *req, int cachemiss)
{
        int ret;
repeat:
        switch (ret) {
                case -11:
                        if (add_output_space_event(req, req->sock)) {
                                del_tux_atom(req);
                                goto repeat;
                        }
                        do { } while (0);
                        break;
                default:
                        add_req_to_workqueue(req);
        }
}

/* cp-out: warning: [^:]*: line 17, column 28: identifier "add_output_space_event" not declared */
/* cp-out: warning: [^:]*: line 18, column 32: identifier "del_tux_atom" not declared */
/* cp-out: warning: [^:]*: line 24, column 24: identifier "add_req_to_workqueue" not declared */
