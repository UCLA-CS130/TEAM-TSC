#include <python2.7/Python.h>
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/regex.hpp>
#include "python_handler.h"

namespace http {
namespace server {

RequestHandler::Status
PythonHandler::Init(const std::string& uri_prefix, const NginxConfig& config)
{
  uri_prefix_ = uri_prefix;
  int root_num = 0;
  int func_num = 0;
  for (auto statement: config.statements_) {
    std::string start_token = statement->tokens_[0];
    if (start_token == "root") {
      if (root_num > 0 || 
          statement->tokens_.size() != 2 ||
          statement->tokens_[1].back() == '/') 
      {
        return RequestHandler::invalid_config;
      }
      base_dir = statement->tokens_[1];
      // otherwise the filepath is //...
      if (base_dir == "/") base_dir += ".";
      root_num++;
    }
    else if (start_token == "func") {
      if (func_num > 0 ||
          statement->tokens_.size() != 2) 
      {
        return RequestHandler::invalid_config;
      }
      entry_func = statement->tokens_[1];
      func_num++;
    }
    // other statements
  }
  if (base_dir.empty() || entry_func.empty()) 
    return RequestHandler::invalid_config;
  return RequestHandler::ok;
}

RequestHandler::Status
PythonHandler::HandleRequest(const Request& request, Response* response)
{
  std::string request_uri;

  if (!Request::uri_decode(request.uri(), request_uri)) {
    BOOST_LOG_TRIVIAL(trace) << "PYTHON_HANDLER: URI decode error";
    response->SetStatus(Response::bad_request);
    return RequestHandler::handle_fail;
  }

  std::size_t start = request_uri.find_last_of("/");
  std::size_t end = request_uri.find_last_of(".");
  if (start == std::string::npos || end == std::string::npos) {
    BOOST_LOG_TRIVIAL(trace) << "PYTHON_HANDLER: bad request";
    response->SetStatus(Response::bad_request);
    return RequestHandler::handle_fail;
  }
  std::string python_script = request_uri.substr(start + 1, end - start - 1);

  // parse the body to pass to python script
  std::string req_body = request.body();
  DecodedBody decoded_body;
  if (!ParseBody(req_body, decoded_body)) {
    BOOST_LOG_TRIVIAL(info) << "PYTHON_HANDLER: Invalid request body format: ";
    response->SetStatus(Response::bad_request);
    return RequestHandler::handle_fail;
  }

  Py_Initialize();
  PyObject* sysPath = PySys_GetObject((char*)"path");
  PyObject* programName = PyString_FromString(base_dir.c_str());
  PyList_Append(sysPath, programName);
  Py_DECREF(programName);

  PyObject *pName = NULL, *pModule = NULL, *pDict = NULL, *pFunc = NULL;
  PyObject *pArgs = NULL, *pValue = NULL;
  Py_Initialize();

  pName = PyString_FromString(python_script.c_str());
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  bool handle = false;

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, entry_func.c_str());
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(1);
      pDict = PyDict_New();
      if (!pDict) {
        BOOST_LOG_TRIVIAL(trace) << "PYTHON_HANDLER: cannot convert argument";
      }
      else {
        for (auto sub_body: decoded_body) {
          PyObject* key = PyString_FromString(sub_body.first.c_str());
          PyObject* val = PyString_FromString(sub_body.second.c_str());
          PyDict_SetItem(pDict, key, val);
          Py_DECREF(key);
          Py_DECREF(val);
        }
        /* pValue reference stolen here: */
        PyTuple_SetItem(pArgs, 0, pDict);

        pValue = PyObject_CallObject(pFunc, pArgs);
        if (pValue != NULL) {
            std::string response_body = std::string(PyString_AsString(pValue));
            response->SetStatus(Response::ok);
            response->SetBody(response_body);
            response->AddHeader("Content-Length", std::to_string(response_body.size()));
            response->AddHeader("Content-Type", "text/plain");
            handle = true;
        }
        else {
          BOOST_LOG_TRIVIAL(trace) << "PYTHON_HANDLER: call failed";
        }
      }
    }
    else {
      if (PyErr_Occurred()) PyErr_Print();
      BOOST_LOG_TRIVIAL(trace) << "PYTHON_HANDLER: Cannot find function test";
    }
  }
  else {
      if (PyErr_Occurred()) PyErr_Print();
      BOOST_LOG_TRIVIAL(trace) << "PYTHON-HANDLER: failed to load python script " << python_script;
  }
  Py_XDECREF(pValue);
  Py_XDECREF(pArgs);
  Py_XDECREF(pFunc);
  Py_XDECREF(pDict);
  Py_XDECREF(pModule);
  Py_XDECREF(sysPath);
  Py_Finalize();

  if (handle) return RequestHandler::ok;
  else return RequestHandler::handle_fail;
}

bool 
PythonHandler::ParseBody(const std::string& body, DecodedBody& decoded_body)
{
  std::string sub_body = "";
  for (std::size_t i = 0; i <= body.size(); ++i) {
    if (body[i] == '&' || i == body.size()) {
    boost::cmatch sub_body_mat;
    boost::regex sub_body_expression ("(\\S+)=(\\S*)");
    if (!boost::regex_match(sub_body.c_str(), sub_body_mat, sub_body_expression)) {
      return false;
    }
    std::string name = std::string(sub_body_mat[1]);
    std::string value;
    if (!Request::uri_decode(std::string(sub_body_mat[2]), value)) {
      return false;
    }
    decoded_body.push_back(std::pair<std::string, std::string>(name, value));
      sub_body = "";
      continue;
    }
    else sub_body.push_back(body[i]);
  }
  return true;
}

} // namespace server
} // namespace http