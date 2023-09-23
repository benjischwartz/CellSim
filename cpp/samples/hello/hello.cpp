#include <iostream>

#include "libcellml/analyser.h"
#include "mongoose/mongoose.h"


//NEXT STEPS:
// 1) Build a toy C++ Webserver
// 2) Start designing API routes for cellml server
// 3) Spin up a VueJs application with NodeJs or Python backend

static const char *s_http_addr = "http://localhost:8000";  // HTTP port
static const char *s_root_dir = "web_root";

// minimal static server
//static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
//  if (ev == MG_EV_HTTP_MSG) {
//    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
//    if (mg_http_match_uri(hm, "/api/hello")) {              // On /api/hello requests,
//      mg_http_reply(c, 200, "", "{%m:%d}\n",
//                    MG_ESC("status"), 1);                   // Send dynamic JSON response
//    } else {                                                // For all other URIs,
//      struct mg_http_serve_opts opts = {.root_dir = "."};   // Serve files
//      mg_http_serve_dir(c, hm, &opts);                      // From root_dir
//    }
//  }
//}

// Taken from https://github.com/cesanta/mongoose/blob/master/examples/webui-rest/main.c

// Dynamic RESTful server
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_http_match_uri(hm, "/api/f1")) {
      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m:%d}\n",
                    MG_ESC("result"), 123);
    } else if (mg_http_match_uri(hm, "/api/sum")) {
      // Attempt to fetch a JSON array from the body, hm->body
      struct mg_str json = hm->body;
      double num1, num2;
      if (mg_json_get_num(json, "$[0]", &num1) &&
          mg_json_get_num(json, "$[1]", &num2)) {
        // Success! create a JSON response
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m:%g}\n",
                      MG_ESC("result"), num1 + num2);
      }
    } else {
      struct mg_http_serve_opts opts = {.root_dir = s_root_dir};
      mg_http_serve_dir(c, hm, &opts);
    }
  }
  (void) fn_data;
}

int main(void) {

    std::cout << "Hello, starting server..." << std::endl;

	struct mg_mgr mgr;                            // Event manager
	mg_log_set(MG_LL_DEBUG);                      // Set log level
	mg_mgr_init(&mgr);                            // Initialise event manager
	mg_http_listen(&mgr, s_http_addr, fn, NULL);  // Create HTTP listener
	for (;;) mg_mgr_poll(&mgr, 1000);             // Infinite event loop
	mg_mgr_free(&mgr);
	return 0;
}
