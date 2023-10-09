#include <iostream>

#include "libcellml/analyser.h"
#include "libcellml/model.h"
#include "libcellml/printer.h"
#include "libcellml/validator.h"
#include "libcellml/parser.h"
#include "libcellml/issue.h"
#include "libcellml/units.h"
#include "libcellml/component.h"
#include "mongoose.h"

std::string create_issue_list(libcellml::ValidatorPtr &validator);

static const char *s_http_addr = "http://localhost:8000";  // HTTP port

// Dynamic RESTful server
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
        mg_http_reply(c, 200, "Content-Type: application/json\r\n" 
                            "Access-Control-Allow-Headers: content-type\r\n"
                            "Access-Control-Request-Method: OPTIONS, POST\r\n"
                            "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                            "{\"OPTIONS Request\": %d}", 0);
    }
    else if (mg_http_match_uri(hm, "/api/validate")) {
     
      // Attempt to fetch a JSON string from the body, hm->body
      struct mg_str file = hm->body;
      char *file_str = mg_json_get_str(file, "$.file");
      std::cout << "Extracted string is...\n" << file_str << std::endl;
      
      // Create model
      auto parser = libcellml::Parser::create();
      auto model = parser->parseModel(file_str);

      // Create validator
      auto validator = libcellml::Validator::create();
      validator->validateModel(model);

      // Retrieve number of issues from validator
      size_t numIssues = validator->issueCount();
      std::string issueList = create_issue_list(validator);

      // get component count
      size_t numComponents = model->componentCount();
      std::cout << "Model has " << numComponents << " components.\n";            
      // List components
      for (size_t i = 0; i < numComponents; ++i) {
          auto component = model->component(i);
          std::cout << "extracting component... " << component->name() << std::endl;

      }

      mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                        "Access-Control-Allow-Headers: content-type\r\n"
                        "Access-Control-Request-Method: POST\r\n"
                        "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                        "{\"Number of issues\": %d, \"Issue List\": %s}", numIssues, issueList.c_str());
    } else if (mg_http_match_uri(hm, "/api/create")) {
          // Extract model name and id
          struct mg_str file = hm->body;
          char *model_name = mg_json_get_str(file, "$.model_name");
          char *model_id = mg_json_get_str(file, "$.model_id");
          std::cout << "Extracted name is... " << model_name << std::endl;
          std::cout << "Extracted id is... " << model_id << std::endl;

          // Create new model
          auto model = libcellml::Model::create(model_name);
          model->setId(model_id);

          // Create serialised model string
          auto printer = libcellml::Printer::create();
          std::string serialisedModelString = printer->printModel(model);

          // Send JSON response
          mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Headers: content-type\r\n"
                                "Access-Control-Request-Method: POST\r\n"
                                "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                                "{\"New model\": %s}", serialisedModelString.c_str());
    } else if (mg_http_match_uri(hm, "/api/edit")) {
          // Extract model data and editing info
          //    Edit options:
              //    1) Add/remove units
              //    2) Add/remove component
          //    If unit already exits or invalid name ==> error
          struct mg_str file = hm->body;
          std::string edit_type = mg_json_get_str(file, "$.edit_type");
          std::string item_name = mg_json_get_str(file, "$.item_name");
          std::string file_str = mg_json_get_str(file, "$.file");
          std::string model_name = mg_json_get_str(file, "$.model_name");
          std::string model_id = mg_json_get_str(file, "$.model_id");
          std::cout << "Extracted edit is... " << edit_type << std::endl;
          std::cout << "Extracted item name is... " << item_name << std::endl;
          std::cout << "Extracted file is... " << file_str << std::endl;
          std::cout << "Extracted model name is... " << model_name << std::endl;
          std::cout << "Extracted model id is... " << model_id << std::endl;
          
          // Create model
          auto parser = libcellml::Parser::create();
          auto model = parser->parseModel(file_str);
          model->setName(model_name);
          model->setId(model_id);

          if (edit_type.compare("add_units") == 0) {
              std::cout << "HERE\n";
              auto new_unit = libcellml::Units::create(item_name);
              model->addUnits(new_unit);
          } else if (edit_type.compare("remove_unit")) {
          } else if (edit_type.compare("add_component")) {
          } else if (edit_type.compare("remove_component")) {
          } else {
              // Unknown command
          }

          // get component count
          size_t numComponents = model->componentCount();
          std::cout << "Model has " << numComponents << " components.\n";            
          // List components
          for (size_t i = 0; i < numComponents; ++i) {
              auto component = model->component(i);
              std::cout << "extracting component...\n";
          }


          // Create serialised model string
          auto printer = libcellml::Printer::create();
          std::string serialisedModelString = printer->printModel(model);

          // Send JSON response
          mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Headers: content-type\r\n"
                                "Access-Control-Request-Method: POST\r\n"
                                "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                                "{\"Updated model\": %s}", serialisedModelString.c_str());
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

std::string create_issue_list(libcellml::ValidatorPtr &validator) {
    std::string issueList;
    for (size_t e = 0; e < validator->issueCount(); ++e) {
        libcellml::IssuePtr validatorIssue = validator->issue(e);
        std::string issueSpecificationReference = validatorIssue->referenceHeading();
        issueList+=" Validator issue[";
        issueList+= std::to_string(e);
        issueList+= "]:\n";
        issueList+="   Description: ";
        issueList+= validatorIssue->description();
        issueList+= "\n";
        issueList+="   Type of item stored: ";
        issueList+= cellmlElementTypeAsString(validatorIssue->item()->type());
        issueList+="\n";
        issueList+="   URL: ";
        issueList+=validatorIssue->url();
        issueList+="\n";
        if (issueSpecificationReference != "") {
            issueList += "     See section ";
            issueList += issueSpecificationReference;
            issueList += " in the CellML specification.\n";
        }
    }
    return issueList;
}


//enum class CellmlElementType
//{
//    COMPONENT,
//    COMPONENT_REF,
//    CONNECTION,
//    ENCAPSULATION,
//    IMPORT,
//    MAP_VARIABLES,
//    MATH,
//    MODEL,
//    RESET,
//    RESET_VALUE,
//    TEST_VALUE,
//    UNDEFINED,
//    UNIT,
//    UNITS,
//    VARIABLE,
//};

// Curl command to hit server: 
// curl -X POST -d '[69, 420]' -H "Content-Type: application/json" http://localhost:8000/api/sum
