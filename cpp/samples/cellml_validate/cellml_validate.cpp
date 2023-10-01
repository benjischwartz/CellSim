#include <iostream>

#include "libcellml/analyser.h"
#include "libcellml/model.h"
#include "libcellml/printer.h"
#include "libcellml/validator.h"
#include "libcellml/parser.h"
#include "mongoose.h"


//NEXT STEPS:
// 1) Build a toy C++ Webserver
// 2) Start designing API routes for cellml server
// 3) Spin up a VueJs application with NodeJs or Python backend

static const char *s_http_addr = "http://localhost:8000";  // HTTP port
//static const char *s_root_dir = "./samples"; // root directory 

// Taken from https://github.com/cesanta/mongoose/blob/master/examples/webui-rest/main.c

// Dynamic RESTful server
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_http_match_uri(hm, "/api/validate")) {
     
      // Extract file from body
      struct mg_str file = hm->body;
      std::cout <<file.ptr<<std::endl;

      // Why does this work, but it segfaults below...
      struct mg_str json = mg_str("{\"a\":\"hi\"}");  // json = {"a": "hi"}
      std::cout<<json.ptr<<std::endl;
      char *str = mg_json_get_str(json, "$.a");        // str = "hi"
      std::cout<<str<<std::endl;
      free(str);

      // This SEGFAULTS!!!!! WHY!!!
      
      //char *file_str = mg_json_get_str(file, "$.file");
      //std::cout<<file_str<<std::endl;
      //free(file_str);


      // Create model
      auto parser = libcellml::Parser::create();
      auto model = parser->parseModel(file.ptr);


      // Create validator
      auto validator = libcellml::Validator::create();
      validator->validateModel(model);

      // Retrieve number of issues from validator
      size_t numIssues = validator->issueCount();


      // Create serialised model string
      auto printer = libcellml::Printer::create();
      std::string serialisedModelString = printer->printModel(model);


      mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                        "Access-Control-Allow-Headers: content-type\r\n"
                        "Access-Control-Request-Method: POST\r\n"
                        "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                        "{\"Number of issues\": %d}", numIssues);

    } else if (mg_http_match_uri(hm, "/api/create")) {
          // Extract headers
          struct mg_str *model_name = mg_http_get_header(hm, "Model-Name") ;
          struct mg_str *model_id = mg_http_get_header(hm, "Model-Id") ;

          // Create new model
          auto model = libcellml::Model::create(model_name->ptr);
          model->setId(model_id->ptr);

          // Create serialised model string
          auto printer = libcellml::Printer::create();
          std::string serialisedModelString = printer->printModel(model);

          // Send JSON response
          char *json = mg_mprintf("{%m:%s}", MG_ESC("model"), serialisedModelString.c_str());
          mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Headers: content-type\r\n"
                                "Access-Control-Request-Method: POST\r\n"
                                "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                                "%s\n", json);
          free(json);
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
      // struct mg_http_serve_opts opts = {.root_dir = s_root_dir};
      // mg_http_serve_dir(c, hm, &opts);
        mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                              "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
							  "{\"Connected to backend!\": %d}", 200);

    }
  }
  (void) fn_data;
}


// Curl command to hit server: 
// curl -X POST -d '[69, 420]' -H "Content-Type: application/json" http://localhost:8000/api/sum

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