#include "config_handler.h"
#include "config_parser.h"

//Sets up config based on config file. Returns false
//if there are any errors
bool
ConfigHandler::setup_config(const char* filename)
{
  // parse the config file
  if (config_parser.Parse(filename, &config)) {
    return setup_config(config.statements_);
  }
  else {
    std::cerr << "Error: Could not parse config file.\n";
    return false;
  }
}

ConfigHandler::StartTokenType
ConfigHandler::to_token_type(std::string token) {
  if (token == "listen") 
    return TOKEN_BEFORE_PORT;
  else if (token == "handler_map")
    return TOKEN_BEFORE_HANDLER;
  else if (token == "server")
    return TOKEN_SERVER;
  else if (token == "base_path")
    return TOKEN_BASE_PATH;
  else return TOKEN_INVALID;
}

bool
ConfigHandler::setup_config(const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_) {
  for (unsigned int i = 0; i < statements_.size(); i++) {
    NginxConfigStatement* statementPtr = statements_[i].get();
    std::string token_start = statementPtr->tokens_[0];

    ConfigHandler::StartTokenType start_token_type = to_token_type(token_start);
    switch (start_token_type) 
    {
      case TOKEN_BEFORE_PORT:
      { 
        if (statementPtr->tokens_.size() != 2) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }

        // invalid number
        int portNum = 0;
        try {
          portNum = std::stoi(statementPtr->tokens_[1]);
        }
        catch (const std::invalid_argument& e) {
          std::cerr << "Error: Invalid port number(not a number)\n";
          return false;
        }
    
        // invalid port
        if(portNum <= MAX_PORT && portNum >= 0) {
          config_opt.port = statementPtr->tokens_[1];
        }
        else {
          std::cerr<<"Error: Invalid port number(not in [0, 65535]).\n";
          return false;
        }
        continue;
      }
      case TOKEN_BEFORE_HANDLER:
      {
        if (statementPtr->tokens_.size() < 3) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }

        std::string token_handler_name = statementPtr->tokens_[1];
        if (token_handler_name == "echo_handler") {
          for (unsigned int j = 2; j < statementPtr->tokens_.size(); ++j) {
            config_opt.echo_paths.emplace_back(statementPtr->tokens_[j]);
          }
        }
        else if (token_handler_name == "static_handler") {
          for (unsigned int j = 2; j < statementPtr->tokens_.size(); ++j) {
            config_opt.static_paths.emplace_back(statementPtr->tokens_[j]);
          }
        }
        continue;
      }
      case TOKEN_SERVER:
      {
        if (statementPtr->child_block_ == nullptr) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }
        bool child_return = setup_config(statementPtr->child_block_->statements_);
        if (!child_return) return false;
        continue;
      }
      case TOKEN_BASE_PATH:
      {
        if (statementPtr->tokens_.size() != 3) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }
        std::string url_root = statementPtr->tokens_[1];
        std::string base_dir = statementPtr->tokens_[2];
        config_opt.url_root2base_dir[url_root] = base_dir;
        continue;
      }
      case TOKEN_INVALID:
        return false;
    } 
  }
  return true;
}


