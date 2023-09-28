/*
 * Sample mongoose webserver
 *
 * Reference: https://mongoose.ws/documentation/tutorials/http-server/
 */

#include <iostream>

#include "mongoose.h"

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    struct mg_http_serve_opts opts = {.root_dir = "."};  // Serve local dir
    if (ev == MG_EV_HTTP_MSG) mg_http_serve_dir(c, static_cast<mg_http_message*>(ev_data), &opts);
}

int main(int argc, char *argv[])
{
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                        // Init manager
    mg_http_listen(&mgr, "http://localhost:8000", fn, &mgr);  // Setup listener
    std::cout << "Started up webserver at http://localhost:8000" << std::endl;

    for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop
    mg_mgr_free(&mgr);                                        // Cleanup
    return 0;
}
