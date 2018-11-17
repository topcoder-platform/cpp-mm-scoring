
/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */
#include <nan.h>
#include <unistd.h>
#include "Runner.h"
#include "MemoryMonitor.h"
using namespace std;
using namespace nlohmann;
using v8::Local;
using v8::Value;
using v8::Number;
using v8::String;
using v8::Function;
using v8::FunctionTemplate;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::HandleScope;
using Nan::ThrowTypeError;
using Nan::ThrowError;
using Nan::MaybeLocal;
using Nan::Callback;
using Nan::Utf8String;
using Nan::JSON;
using Nan::New;
using Nan::Call;
using Nan::Null;
using Nan::To;

// Async worker to run submission codes
class RubSubmissionWorker : public AsyncWorker {
public:
  RubSubmissionWorker(string signature, string input, string submissionCode,
                      Callback *callback)
      : AsyncWorker(callback), signature(signature), input(input),
        submissionCode(submissionCode) {}
  ~RubSubmissionWorker() {}

  // Executed inside the worker-thread.
  void Execute() {
    try {
      Runner runner(json::parse(signature)); // build runner class
      pid_t pid = getpid();
      MemoryMonitor memoryMonitor(100, (int)pid); // build memory monitor
      memoryMonitor.start();
      runner.setMemoryMonitor(&memoryMonitor);
      result = runner.runSubmission(json::parse(input), submissionCode);
      memoryMonitor.terminate();
      return;
    } catch (const std::runtime_error &e) {
      this->SetErrorMessage(e.what());
      return;
    } catch (...) {
      this->SetErrorMessage("Unknown error occurred!");
      return;
    }
  }

  // Executed when the async work is complete
  void HandleOKCallback() {
    HandleScope scope;
    JSON NanJSON;
    Local<Value> argv[] = {
        Null(), // no error occurred
        NanJSON.Parse(New(result.dump()).ToLocalChecked()).ToLocalChecked()};
    Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }

  // Executed when error occurred
  void HandleErrorCallback() {
    HandleScope scope;
    Local<Value> argv[] = {
        New(this->ErrorMessage()).ToLocalChecked(), // return error message
        Null()};
    Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }

private:
  string signature;
  string input;
  string submissionCode;
  json result;
};


// Async worker to checkSignature codes
class CheckSignatureWorker : public AsyncWorker {
public:
  CheckSignatureWorker(string signature, string submissionCode,
                      Callback *callback)
      : AsyncWorker(callback), signature(signature),
        submissionCode(submissionCode) {}
  ~CheckSignatureWorker() {}

  // Executed inside the worker-thread.
  void Execute() {
    try {
      /*Runner* runner = new Runner(json::parse(signature)); // build runner class
      result = runner->checkSignatureExists(submissionCode);
      delete runner; */
      Runner runner(json::parse(signature)); // build runner class
            result = runner.checkSignatureExists(submissionCode);
      return;
    } catch (const std::runtime_error &e) {
      this->SetErrorMessage(e.what());
      return;
    } catch (...) {
      this->SetErrorMessage("Unknown error occurred!");
      return;
    }
  }

  // Executed when the async work is complete
  void HandleOKCallback() {
    HandleScope scope;
    JSON NanJSON;
    Local<Value> argv[] = {
        Null(), // no error occurred
        NanJSON.Parse(New(result.dump()).ToLocalChecked()).ToLocalChecked()};
    Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }

  // Executed when error occurred
  void HandleErrorCallback() {
    HandleScope scope;
    Local<Value> argv[] = {
        New(this->ErrorMessage()).ToLocalChecked(), // return error message
        Null()};
    Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }

private:
  string signature;
  string submissionCode;
  json result;
};
NAN_METHOD(runSubmissionAsync) {
  // check arguments
  if (info.Length() != 4) {
    return ThrowTypeError(New("expected length of arguments=4").ToLocalChecked());
  }
  if (!info[0]->IsObject()) {
    return ThrowTypeError(New("expected arg 0: object signature").ToLocalChecked());
  }
  if (!info[1]->IsObject()) {
    return ThrowTypeError(New("expected arg 1: object input").ToLocalChecked());
  }
  if (!info[2]->IsString()) {
    return ThrowTypeError(
        New("expected arg 2: string submissionCode").ToLocalChecked());
  }
  if (!info[3]->IsFunction()) {
    return ThrowTypeError(
        New("expected arg 3: function callback").ToLocalChecked());
  }
  // load json objects to json string and convert to nlohmann json object later
  JSON NanJSON;
  MaybeLocal<String> signature_json = NanJSON.Stringify(info[0]->ToObject());
  if (signature_json.IsEmpty()) {
    return ThrowTypeError(
        New("expected arg 0: object signature is valid json object")
            .ToLocalChecked());
  }
  MaybeLocal<String> input_json = NanJSON.Stringify(info[1]->ToObject());
  if (input_json.IsEmpty()) {
    return ThrowTypeError(New("expected arg 1: object input is valid json object")
                          .ToLocalChecked());
  }
  string signature =
      string(*Utf8String(signature_json.ToLocalChecked()->ToString()));
  string input = string(*Utf8String(input_json.ToLocalChecked()->ToString()));
  string submissionCode = string(*Utf8String(info[2]->ToString()));
  Callback *callback = new Callback(To<Function>(info[3]).ToLocalChecked());
  // use async callback function with error and result
  AsyncQueueWorker(
      new RubSubmissionWorker(signature, input, submissionCode, callback));
}

NAN_METHOD(checkSignatureAsync) {
  // check arguments
  if (info.Length() != 3) {
    return ThrowTypeError(New("expected length of arguments=4").ToLocalChecked());
  }
  if (!info[0]->IsObject()) {
    return ThrowTypeError(New("expected arg 0: object signature").ToLocalChecked());
  }
  if (!info[1]->IsString()) {
    return ThrowTypeError(
        New("expected arg 1: string submissionCode").ToLocalChecked());
  }
  if (!info[2]->IsFunction()) {
    return ThrowTypeError(
        New("expected arg 2: function callback").ToLocalChecked());
  }
  // load json objects to json string and convert to nlohmann json object later
  JSON NanJSON;
  MaybeLocal<String> signature_json = NanJSON.Stringify(info[0]->ToObject());
  if (signature_json.IsEmpty()) {
    return ThrowTypeError(
        New("expected arg 0: object signature is valid json object")
            .ToLocalChecked());
  }
  string signature =
      string(*Utf8String(signature_json.ToLocalChecked()->ToString()));
  string submissionCode = string(*Utf8String(info[1]->ToString()));
  Callback *callback = new Callback(To<Function>(info[2]).ToLocalChecked());
  // use async callback function with error and result
  AsyncQueueWorker(
      new CheckSignatureWorker(signature, submissionCode, callback));
}
// init nan module
NAN_MODULE_INIT(init) {
  Nan::Set(target, New<String>("runSubmissionAsync").ToLocalChecked(),
           New<FunctionTemplate>(runSubmissionAsync)->GetFunction());
  Nan::Set(target, New<String>("checkSignatureAsync").ToLocalChecked(),
              New<FunctionTemplate>(checkSignatureAsync)->GetFunction());
}

NODE_MODULE(addon, init)
