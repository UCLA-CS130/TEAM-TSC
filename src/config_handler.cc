#include "config_handler.h"
#include "config_parser.h"

//Sets up config based on config file. Returns false
//if there are any errors
bool ConfigHandler::setup_config(const char* filename)
{
  // parse the config file
  if (config_parser->Parse(filename, config)) {
    return setup_config(config->statements_);
  }
  else {
    std::cerr << "Error: Could not parse config file.\n";
    return false;
  }
}

bool ConfigHandler::setup_config(std::vector<std::shared_ptr<NginxConfigStatement>>& statements_) {
  for (unsigned int i = 0; i < statements_.size(); i++) {
    NginxConfigStatement* statementPtr = statements_[i].get();

    for(unsigned int j = 0; j < statementPtr->tokens_.size(); j++) { 
      std::string cur_token = statementPtr->tokens_[j];

      if(cur_token == TOKEN_BEFORE_PORT) {
        // should conform to 'port %d format'
        if (statementPtr->tokens_.size() != 2) {
          std::cerr<<"Error: Invalid config file format.\n";
          return false;
        }

        int portNum = 0;
        // invalid number
        try {
          portNum = std::stoi(statementPtr->tokens_[j + 1]);
        }
        catch (const std::invalid_argument& e) {
          std::cerr << "Error: Invalid port number(not a number)\n";
          return false;
        }
    
        // invalid port
        if(portNum <= MAX_PORT && portNum >= 0) {
          config_opt.port = statementPtr->tokens_[j+1];
        }
        else {
          std::cerr<<"Error: Invalid port number(not in [0, 65535]).\n";
          return false;
        }

        continue;
      }
      // TODO
      // else if (cur_token == TOKEN_BEFORE_LOCATION) .....
      // else if (cur_token == TOKEN_BEFORE_XXX)

      if (statementPtr->child_block_ != nullptr){
        bool child_return = setup_config(statementPtr->child_block_->statements_);
        if (!child_return) return false;
      }
    }
  }
  return true;
}


