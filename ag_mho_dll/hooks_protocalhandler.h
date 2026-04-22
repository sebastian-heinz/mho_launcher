#ifndef MHO_HOOKS_PROTOCALHANDLER_H
#define MHO_HOOKS_PROTOCALHANDLER_H

void install_protocalhandler_hooks();

/* Send interface — callable after first packet is processed */
int protocalhandler_send_raw(void *data, int len);

#endif
