#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
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
    BOOST_LOG_TRIVIAL(error) << "Could not parse config file";
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
  else if (token == "thread")
    return TOKEN_THREAD;
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
          BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
          return false;
        }

        // invalid number
        int portNum = 0;
        try {
          portNum = std::stoi(statementPtr->tokens_[1]);
        }
        catch (const std::invalid_argument& e) {
          BOOST_LOG_TRIVIAL(error) << "Invalid port number(not a number)";
          return false;
        }
    
        // invalid port
        if(portNum <= MAX_PORT && portNum >= 0) {
          config_opt.port = statementPtr->tokens_[1];
        }
        else {
          BOOST_LOG_TRIVIAL(error) << "Invalid port number(not in [0, 65535]).";
          return false;
        }
        continue;
      }
      case TOKEN_PATH:
      {
        if (statementPtr->tokens_.size() != 3) {
          BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
          return false;
        }

        std::string uri_prefix = statementPtr->tokens_[1];
        std::string handler_name = statementPtr->tokens_[2];
        if (check_duplicate_path.find(uri_prefix) != check_duplicate_path.end()) {
          BOOST_LOG_TRIVIAL(error) << "Illigal duplicate paths";
          return false;
        }
        // if (uri_prefix.back() == '/') {
        //   BOOST_LOG_TRIVIAL(error) << "Invalid path name terminated with /";
        //   return false;
        // }
        check_duplicate_path[uri_prefix] = true;
        if (handler_name == "EchoHandler") {
          config_opt.echo_uri_prefixes.emplace_back(uri_prefix);
          if (statementPtr->child_block_ != nullptr) {
            BOOST_LOG_TRIVIAL(error) << "Invalid child block for EchoHandler";
            return false;
          }
        }
        else if (handler_name == "StatusHandler") {
          config_opt.status_uri_prefixes.emplace_back(uri_prefix);
          if(statementPtr->child_block_!=nullptr) {
            BOOST_LOG_TRIVIAL(error) << "Invalid child block for StatusHandler";
          }
        }
        else if (handler_name == "StaticHandler") {
          config_opt.static_file_uri_prefixes.emplace_back(uri_prefix);
          if (statementPtr->child_block_ == nullptr) {
            BOOST_LOG_TRIVIAL(error) << "Missing child block for StaticHandler";
            return false;
          }
          config_opt.static_file_config.emplace_back(*statementPtr->child_block_);
        }
	else if (handler_name == "ProxyHandler") {
	  std::cout << uri_prefix << std::endl;
	  config_opt.proxy_uri_prefixes.emplace_back(uri_prefix);
          if (statementPtr->child_block_ == nullptr) {
            BOOST_LOG_TRIVIAL(error) << "Missing child block for ProxyHandler";
            return false;
          }
          config_opt.proxy_config.emplace_back(*statementPtr->child_block_);
        }
        continue;
      }
      case TOKEN_SERVER:
      {
        if (statementPtr->child_block_ == nullptr) {
          BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
          return false;
        }
        bool child_return = handle_statements(statementPtr->child_block_->statements_);
        if (!child_return) {
          BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
          return false;
        }
        continue;
      }
      case TOKEN_THREAD:
      {
        if (statementPtr->tokens_.size() != 2) {
          BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
          return false;
        }

        // invalid number
        int threadNum = 0;
        try {
          threadNum = boost::lexical_cast<int>(statementPtr->tokens_[1]);
        }
        catch(boost::bad_lexical_cast &e)
        {
          BOOST_LOG_TRIVIAL(error) << "Invalid thread number(not a number)";
          return false;
        }
    
        // invalid port
        if(threadNum > 0) {
          config_opt.threadCount = threadNum;
        }
        else {
          BOOST_LOG_TRIVIAL(error) << "Invalid thread number(not positive).";
          return false;
        }
        continue;
      }
      default:
      {
        BOOST_LOG_TRIVIAL(error) << "Invalid config file format for" << token_start;
        return false;
      }
    } 
  }
  return true;
}



