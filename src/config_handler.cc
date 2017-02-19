#include "config_handler.h"

//Sets up config based on config file. Returns false
//if there are any errors
bool
ConfigHandler::handle_config(const char* filename)
{
  // parse the config file
  if (config_parser.Parse(filename, &config)) {
    return handle_statements(config.statements_);
  }
  else {
    std::cerr << "Error: Could not parse config file.\n";
    return false;
  }
}

ConfigHandler::StartTokenType
ConfigHandler::to_token_type(std::string token) {
  if (token == "port") 
    return TOKEN_PORT;
  else if (token == "path")
    return TOKEN_PATH;
  else if (token == "server")
    return TOKEN_SERVER;
  else if (token == "root")
    return TOKEN_ROOT;
  else if (token == "default")
    return TOKEN_DEFAULT;
  else return TOKEN_INVALID;
}

bool
ConfigHandler::handle_statements(const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_) {
  for (unsigned int i = 0; i < statements_.size(); i++) {
    NginxConfigStatement* statementPtr = statements_[i].get();
    std::string token_start = statementPtr->tokens_[0];

    ConfigHandler::StartTokenType start_token_type = to_token_type(token_start);
    switch (start_token_type) 
    {
      case TOKEN_PORT:
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
        if (statementPtr->tokens_.size() != 3) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }

        std::string uri_prefix = statementPtr->tokens_[1];
        std::string handler_name = statementPtr->tokens_[2];
        if (check_duplicate_path.find(uri_prefix) != check_duplicate_path.end()) {
          std::cerr << "Error: Illigal duplicate paths\n";
          return false;
        }
        if (uri_prefix.back() == '/') {
          std::cerr << "Error: Invalid path name terminated with /\n";
          return false;
        }
        check_duplicate_path[uri_prefix] = true;
        if (handler_name == "EchoHandler") {
          config_opt.echo_uri_prefixes.emplace_back(uri_prefix);
          if (statementPtr->child_block_ != nullptr) {
            std::cerr << "Error: Invalid child block for EchoHandler\n";
            return false;
          }
        }
        else if (handler_name == "StaticHandler") {
          config_opt.static_file_uri_prefixes.emplace_back(uri_prefix);
          if (statementPtr->child_block_ == nullptr) {
            std::cerr << "Error: Missing child block for StaticHandler\n";
            return false;
          }
          config_opt.static_file_config.emplace_back(*statementPtr->child_block_);
        }
        continue;
      }
      case TOKEN_SERVER:
      {
        if (statementPtr->child_block_ == nullptr) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }
        bool child_return = handle_statements(statementPtr->child_block_->statements_);
        if (!child_return) {
          std::cerr << "Error: Invalid config file format for" << token_start << "\n";
          return false;
        }
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



