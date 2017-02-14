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
  else if (token == "path")
    return TOKEN_PATH;
  else if (token == "server")
    return TOKEN_SERVER;
  else if (token == "root")
    return TOKEN_ROOT;
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
      case TOKEN_PATH:
      {
        if (statementPtr->tokens_.size() < 3) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }

        unsigned int tokens_length = statementPtr->tokens_.size();
        std::string token_handler_name = statementPtr->tokens_[tokens_length - 1];
        if (token_handler_name == "EchoHandler") {
          for (unsigned int j = 1; j < tokens_length - 1; ++j) {
            config_opt.echo_handler.paths.emplace_back(statementPtr->tokens_[j]);
          }
          if (statementPtr->child_block_ != nullptr) {
            if (!setup_handler_roots(config_opt.echo_handler, statementPtr->child_block_->statements_))
              return false;
          }
        }
        else if (token_handler_name == "StaticHandler") {
          for (unsigned int j = 1; j < tokens_length - 1; ++j) {
            config_opt.static_handler.paths.emplace_back(statementPtr->tokens_[j]);
          }
          if (statementPtr->child_block_ != nullptr) {
            if (!setup_handler_roots(config_opt.static_handler, statementPtr->child_block_->statements_))
              return false;
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
        if (!child_return) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }
        continue;
      }
      default:
      {
        std::cerr << "Error: Invalid config file format for 233" << token_start << "\n";
        return false;
      }
    } 
  }
  return true;
}

bool
ConfigHandler::setup_handler_roots(handler_opts& handler,
                                   const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_) {
  for (unsigned int i = 0; i < statements_.size(); i++) {
    std::string token_start = statements_[i]->tokens_[0];
    ConfigHandler::StartTokenType start_token_type = to_token_type(token_start);
    switch (start_token_type)
    {
      case TOKEN_ROOT:
      {
        if (statements_[i]->tokens_.size() != 3) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }

        std::string uri_root = statements_[i]->tokens_[1];
        std::string base_dir = statements_[i]->tokens_[2];
        handler.uri_root2base_dir[uri_root] = base_dir;
        continue;
      }
      default: 
      {
        std::cerr << "Error: Invalid config file format for" << token_start << "\n";
        return false;
      }
    }
  }
  return true;
}


