#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <string> // required for std::string
#include <sys/types.h> // required for stat.h
#include <sys/stat.h> 

#include "libcellml/analyser.h"
#include "libcellml/annotator.h"
#include "libcellml/model.h"
#include "libcellml/printer.h"
#include "libcellml/validator.h"
#include "libcellml/variable.h"
#include "libcellml/parser.h"
#include "libcellml/issue.h"
#include "libcellml/units.h"
#include "libcellml/component.h"
#include "libcellml/types.h"
#include "libcellml/importsource.h"
#include "mongoose.h"

// Forward declare helper functions
void printModel(libcellml::ModelPtr &model);
void printModel(libcellml::ModelPtr &model, bool includeMaths);
void printIssues(const libcellml::LoggerPtr &item);
void printComponentToTerminal(const libcellml::ComponentPtr &component, size_t const c, std::string const spacer);
void printComponentToTerminal(const libcellml::ComponentPtr &component, size_t const c, std::string const spacer, bool includeMaths);
void printComponentOnlyToTerminal(libcellml::ComponentPtr &component, std::string spacer);
void printEncapsulation(libcellml::ModelPtr &model);
void printImportDependencies(const libcellml::ModelPtr &model);
std::string fileContents(const std::string &fileName);
std::string getIssueLevelFromEnum(libcellml::Issue::Level myLevel);
void solveWithEuler(double stepSize, int stepCount, std::string outFileName);
bool makeDirectory(const std::string &sPath);
std::string create_issue_list(libcellml::ValidatorPtr &validator);
std::string create_visualised_string(libcellml::ModelPtr &model);

static const char *s_http_addr = "http://localhost:8000";  // HTTP port

// Local copy of model
auto local_model = libcellml::Model::create();

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

      // Print model to terminal, include the MathML
      printModel(model, true);

	  // http reply
      mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                        "Access-Control-Allow-Headers: content-type\r\n"
                        "Access-Control-Request-Method: POST\r\n"
                        "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                        "{\"Number of issues\": %d, \"Issue List\": %s}", numIssues, issueList.c_str());
    } else if (mg_http_match_uri(hm, "/api/visualise")) {
     
      // Attempt to fetch a JSON string from the body, hm->body
      struct mg_str file = hm->body;
      char *file_str = mg_json_get_str(file, "$.file");
      std::cout << "Extracted string is...\n" << file_str << std::endl;
      
      // Create model
      auto parser = libcellml::Parser::create();
      auto model = parser->parseModel(file_str);

      std::string text = create_visualised_string(model);

	  // http reply
      mg_http_reply(c, 200, "Content-Type: application/json\r\n"
                        "Access-Control-Allow-Headers: content-type\r\n"
                        "Access-Control-Request-Method: POST\r\n"
                        "Access-Control-Allow-Origin: http://localhost:5173\r\n", 
                        "%s", text.c_str());
    } else if (mg_http_match_uri(hm, "/api/create")) {

          // string to store result
          std::string result;

          // Extract model name and id
          struct mg_str file = hm->body;
          char *model_name = mg_json_get_str(file, "$.model_name");
          char *model_id = mg_json_get_str(file, "$.model_id");
          std::cout << "Extracted name is... " << model_name << std::endl;
          std::cout << "Extracted id is... " << model_id << std::endl;

          // Reset local model
          local_model = libcellml::Model::create(model_name);
          local_model->setId(model_id);

          // Create serialised model string of local
          auto printer = libcellml::Printer::create();
          result = printer->printModel(local_model);

          // Create visualised string of local
          std::string visualised_str = create_visualised_string(local_model);

          // Send chunked JSON response
          mg_printf(c, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Headers: content-type\r\nAccess-Control-Request-Method: POST\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n\r\n");
          mg_http_printf_chunk(c, "%s", result.c_str());
          mg_http_printf_chunk(c, "%s", "*separator*");
          mg_http_printf_chunk(c, "%s", visualised_str.c_str());
          mg_http_printf_chunk(c, "");  // Don't forget the last empty chunk

    } else if (mg_http_match_uri(hm, "/api/edit")) {

          // Extract model data and editing info
          //    Edit options:
              //    1) Add/remove units
              //    2) Add/remove component
          //    If unit already exits or invalid name ==> error

          // string to store result
          std::string result;

          struct mg_str file = hm->body;
          std::string edit_type = mg_json_get_str(file, "$.edit_type");
          std::string file_str = mg_json_get_str(file, "$.file");
          std::string model_name = mg_json_get_str(file, "$.model_name");
          std::string model_id = mg_json_get_str(file, "$.model_id");
          std::cout << "Extracted edit is... " << edit_type << std::endl;
          std::cout << "Extracted file is... " << file_str << std::endl;
          std::cout << "Extracted model name is... " << model_name << std::endl;
          std::cout << "Extracted model id is... " << model_id << std::endl;

          // ADD UNITS
          if (edit_type.compare("add_units") == 0) {
              std::string units_name = mg_json_get_str(file, "$.units_name");
              std::cout << "Extracted units name is... " << units_name << std::endl;
              auto new_unit = libcellml::Units::create(units_name);
              local_model->addUnits(new_unit);
          } 
          // REMOVE UNITS          
          else if (edit_type.compare("remove_unit") == 0) {
          } 
          // ADD COMPONENT 
          else if (edit_type.compare("add_component") == 0) {
              std::cout << "HERE\n";
              std::string component_name = mg_json_get_str(file, "$.component_name");
              std::cout << "Extracted component name is... " << component_name << std::endl;
              auto new_component = libcellml::Component::create(component_name);
              local_model->addComponent(new_component);
          } 
          // REMOVE COMPONENT
          else if (edit_type.compare("remove_component") == 0) {
          } else {
              std::cout << "UNKNOWN\n";
              // Unknown command
          }

          auto printer = libcellml::Printer::create();
          result = printer->printModel(local_model);

          // Create visualised string of local
          std::string visualised_str = create_visualised_string(local_model);

          // Send chunked JSON response
          mg_printf(c, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Headers: content-type\r\nAccess-Control-Request-Method: POST\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n\r\n");
          mg_http_printf_chunk(c, "%s", result.c_str());
          mg_http_printf_chunk(c, "%s", "*separator*");
          mg_http_printf_chunk(c, "%s", visualised_str.c_str());
          mg_http_printf_chunk(c, "");  // Don't forget the last empty chunk
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

// Redirect printModel output from std::cout to stringstream
// Automatically includes the MathML
std::string create_visualised_string(libcellml::ModelPtr &model)
{
      std::stringstream buffer;
      std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
      printModel(model, true);
      std::string text = buffer.str();
      std::cout.rdbuf( old );
      return text;
}


// HELPER FUNCTIONS:

/**
 *  TUTORIAL UTILITIES
 */



void printModel(libcellml::ModelPtr &model)
{
    printModel(model, false);
}

void printModel(libcellml::ModelPtr &model, bool includeMaths)
{
    if(model == nullptr) {
        std::cout << "No model passed to this function." << std::endl;
        return;
    }
    std::string spacer = "    ";

    std::cout << "MODEL: '" << model->name() << "'";
    if (model->id() != "") {
        std::cout << ", id: '" << model->id() << "'";
    }
    std::cout << std::endl;

    std::cout << spacer << "UNITS: " << model->unitsCount() << " custom units" << std::endl;
    for (size_t u = 0; u < model->unitsCount(); ++u) {
        std::cout << spacer << spacer << "[" << u << "]: " << model->units(u)->name();
        if(model->units(u)->isImport()) {
            std::cout << ", imported from: '";
            std::cout << model->units(u)->importReference();
            std::cout << "' in '"<<model->units(u)->importSource()->url() << "'";
        }
        std::cout << std::endl;
    }

    std::cout << spacer << "COMPONENTS: " << model->componentCount() << " components" << std::endl;
    for (size_t c = 0; c < model->componentCount(); ++c) {
        auto component = model->component(c);
        printComponentToTerminal(component, c, spacer + spacer, includeMaths);
    }
}


void printComponentToTerminal(const libcellml::ComponentPtr &component, size_t const c, std::string const spacer)
{
    printComponentToTerminal(component, c, spacer, true);
}

void printComponentToTerminal(const libcellml::ComponentPtr &component, size_t const c, std::string const spacer, bool includeMaths)
{
    std::string local = "    ";

    std::cout << spacer << "[" << c << "]: '" << component->name() << "'";
    if (component->id() != "") {
        std::cout << ", id: '" << component->id() << "'";
    }
    if(component->isImport()) {
            std::cout << " <--- imported from: '";
            std::cout << component->importReference();
            std::cout << "' in '" << component->importSource()->url() << "'";
    }
    std::cout << std::endl;

    std::cout << spacer << local << "VARIABLES: " << component->variableCount() << " variables" << std::endl;

    // Printing the variables within the component.
    for (size_t v = 0; v < component->variableCount(); v++) {
        std::cout << spacer << local << local;
        std::cout << "[" << v << "]: " << component->variable(v)->name();
        if (component->variable(v)->units() != nullptr) {
            std::cout << " [" << component->variable(v)->units()->name() << "]";
        }
        if (component->variable(v)->initialValue() != "") {
            std::cout << ", initial = " << component->variable(v)->initialValue();
        }
        std::cout << std::endl;
        if (component->variable(v)->equivalentVariableCount() > 0) {
            std::cout << spacer << local << local << local;
            std::string con = "  └──> ";
            for (size_t e = 0; e < component->variable(v)->equivalentVariableCount(); ++e) {
                auto ev = component->variable(v)->equivalentVariable(e);
                if (ev == nullptr) {
                    std::cout << "WHOOPS! Null equivalent variable!";
                    continue;
                }
                libcellml::ComponentPtr ev_parent = std::dynamic_pointer_cast<libcellml::Component>(ev->parent());
                if (ev_parent == nullptr) {
                    std::cout << "WHOOPS! Null parent component for equivalent variable!";
                    continue;
                }
                std::cout << con << ev_parent->name() << ":" << ev->name();
                if (ev->units() != nullptr) {
                    std::cout << " [" << ev->units()->name() << "]";
                }
                con = ", ";
            }
            std::cout << std::endl;
        }
    }

    // Print the maths within the component.
    if (includeMaths) {
        if (component->math() != "") {
            std::cout << spacer << "  Maths in the component is:" << std::endl;
            std::cout << component->math() << std::endl;
        }
    }

    // Print the encapsulated components
    if (component->componentCount() > 0) {
        std::cout << spacer << local << "COMPONENT " << component->name() << " has "
                  << component->componentCount()
                  << " child components:" << std::endl;

        for (size_t c2 = 0; c2 < component->componentCount(); c2++) {
            auto child = component->component(c2);
            std::string oneMoreSpacer = spacer + local + local;
            printComponentToTerminal(child, c2, oneMoreSpacer, includeMaths);
        }
    }
}

// START printIssues
void printIssues(const libcellml::LoggerPtr &item) {

    // Get the number of issues attached to the logger item.  Note that this will 
    // return issues of all levels.  To retrieve the total number of a specific level
    // of issues, use the errorCount(), warningCount(), hintCount(), or messageCount() functions. 
    size_t num = item->issueCount();
    std::cout << "Recorded " << num << " issues";

    if (num != 0) {
        std::cout << ":" << std::endl;
        for (size_t i = 0; i < num; ++i) {

            // Retrieve the issue at index i.  Note that this is agnostic as to the level of issue.
            // Specific issue levels can be retrieved using the functions item->error(i), item->warning(i) 
            // etc, where the index must be within appropriate limits.
            libcellml::IssuePtr issue = item->issue(i);

            // Issues created by the Validator class contain a reference heading number, which indicates
            // the section reference within the normative specification relevant to the issue.
            std::string errorReference = issue->referenceHeading();

            // The level of an issue is retrieved using the level() function as an enum value.  
            std::cout << "Issue " << i << " is " << getIssueLevelFromEnum(issue->level()) << ":" << std::endl;

            // Each issue has a descriptive text field, accessible through the description() function.
            std::cout << "    Description: " << issue->description() << std::endl;
            if (errorReference != "") {
                std::cout << "    See section " << errorReference
                          << " in the CellML specification." << std::endl;
            }

            // An optional URL is given for some issues which directs the user to more detailed information.
            if(!issue->url().empty()){
                std::cout << "    More information is available at: " <<issue->url() << std::endl;
            }

            // Each issue is associated with an item.  In order to properly deal with the item stored, its type is 
            // recorded too in an enumeration.
            std::cout << "    Stored item type: " << cellmlElementTypeAsString(issue->item()->type()) << std::endl;
        }
        std::cout << std::endl;
    }
    else {
        std::cout << "!" << std::endl << std::endl;
    }
}
// END printIssues

std::string fileContents(const std::string &fileName)
{
    std::ifstream file(fileName);
    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

// START getIssueLevelFromEnum
std::string getIssueLevelFromEnum(libcellml::Issue::Level myLevel)
{
    std::string myTypeAsString = "dunno";

    switch (myLevel) {
    case libcellml::Issue::Level::ERROR:
        myTypeAsString = "an ERROR";
        break;
    case libcellml::Issue::Level::WARNING:
        myTypeAsString = "a WARNING";
        break;
    case libcellml::Issue::Level::MESSAGE:
        myTypeAsString = "a MESSAGE";
        break;
    }
    return myTypeAsString;
}
// END getIssueLevelFromEnum

void printComponentOnlyToTerminal(libcellml::ComponentPtr &component, std::string spacer)
{
    std::cout << spacer << "Component '" << component->name() << "' has " << component->componentCount() << " child components" << std::endl;
    for (size_t c = 0; c < component->componentCount(); c++) {
        std::string anotherSpacer = "    " + spacer;
        auto child = component->component(c);
        printComponentOnlyToTerminal(child, anotherSpacer);
    }
}

void printEncapsulation(libcellml::ModelPtr &model)
{
    // Prints the encapsulation structure of the model to the terminal
    std::string spacer = "  - ";

    std::cout << "Model '" << model->name() << "' has " << model->componentCount()
              << " components" << std::endl;

    for (size_t c = 0; c < model->componentCount(); ++c) {
        auto child = model->component(c);
        printComponentOnlyToTerminal(child, spacer);
    }
}

// START printEquivalentVariableSet
void listEquivalentVariables(const libcellml::VariablePtr &variable, std::vector<libcellml::VariablePtr> &variableList)
{
    if (variable == nullptr) {
        return;
    }

    for (size_t i = 0; i < variable->equivalentVariableCount(); ++i) {
        auto equivalentVariable = variable->equivalentVariable(i);
        if (std::find(variableList.begin(), variableList.end(), equivalentVariable) == variableList.end()) {
            variableList.push_back(equivalentVariable);
            listEquivalentVariables(equivalentVariable, variableList);
        }
    }
}

void printEquivalentVariableSet(const libcellml::VariablePtr &variable)
{
    if (variable == nullptr) {
        std::cout << "NULL variable submitted to printEquivalentVariableSet." << std::endl;
        return;
    }
    std::vector<libcellml::VariablePtr> variableList;
    variableList.push_back(variable);
    listEquivalentVariables(variable, variableList);

    libcellml::ComponentPtr component = std::dynamic_pointer_cast<libcellml::Component>(variable->parent());

    if (component != nullptr) {
        std::cout << "Tracing: " << component->name() << " -> "
                  << variable->name();
        if (variable->units() != nullptr) {
            std::cout << " [" << variable->units()->name() << "]";
        }
        if (variable->initialValue() != "") {
            std::cout << ", initial = " << variable->initialValue();
        }
        std::cout << std::endl;
    }

    if (variableList.size() > 1) {
        for (auto &e : variableList) {
            component = std::dynamic_pointer_cast<libcellml::Component>(e->parent());
            if (component != nullptr) {
                std::cout << "    - " << component->name() << " -> " << e->name();
                if (e->units() != nullptr) {
                    std::cout << " [" << e->units()->name() << "]";
                }
                if (e->initialValue() != "") {
                    std::cout << ", initial = " << e->initialValue();
                }
                std::cout << std::endl;
            } else {
                std::cout << "Variable " << e->name() << " does not have a parent component." << std::endl;
            }
        }
    } else {
        std::cout << "    - Not connected to any equivalent variables." << std::endl;
    }
}
// END printEquivalentVariableSet

// START printImportDependencies
void doPrintImportDependencies(const libcellml::ModelPtr &model, std::string &spacer) {
    // Function to recursively iterate through the import dependencies in this model, and 
    // print their URL and what they require to the terminal.
    if(model->hasUnresolvedImports()) {
        return;
    }
    std::cout << spacer << "Model '" << model->name() << "' imports:" << std::endl;

    const std::vector<std::string> imports = model->importRequirements();
    for (const auto &i : imports) {
        std::cout << "    - import: " << i << std::endl;
    }
}

void printImportDependencies(const libcellml::ModelPtr &model){
    std::string spacer = " ";
    doPrintImportDependencies(model, spacer);
}
// END printImportDependencies

bool makeDirectory(const std::string &sPath) {
    // std::string sPath = "/tmp/test";
    mode_t nMode = 0733; // UNIX style permissions
    int nError = 0;
    #if defined(_WIN32)
    nError = _mkdir(sPath.c_str()); // can be used on Windows
    #else 
    nError = mkdir(sPath.c_str(), nMode); // can be used on non-Windows
    #endif
    if (nError != 0) {
        // std::cout << "OOPS! Something went wrong trying to create directory: " << sPath << std::endl;
        return false;
    }
    return true;
}
