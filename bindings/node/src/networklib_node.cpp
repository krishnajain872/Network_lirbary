#include <napi.h>
#include "networklib/network_lib.h"
#include "stream_envelope.pb.h"
#include <iostream>

class ServerWrapper : public Napi::ObjectWrap<ServerWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "Server", {
            InstanceMethod("start", &ServerWrapper::Start),
            InstanceMethod("stop", &ServerWrapper::Stop),
            InstanceMethod("wait", &ServerWrapper::Wait),
            InstanceMethod("registerStreamHandler", &ServerWrapper::RegisterStreamHandler)
        });

        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();

        exports.Set("Server", func);
        return exports;
    }

    ServerWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<ServerWrapper>(info) {
        Napi::Env env = info.Env();
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
            return;
        }
        std::string path = info[0].As<Napi::String>();
        try {
            server_ = networklib::NetworkLib::CreateServer(path);
        } catch (const std::exception& e) {
            Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        }
    }

private:
    static Napi::FunctionReference constructor;
    std::shared_ptr<networklib::IServer> server_;

    Napi::Value Start(const Napi::CallbackInfo& info) {
        bool res = server_->Start();
        return Napi::Boolean::New(info.Env(), res);
    }

    Napi::Value Stop(const Napi::CallbackInfo& info) {
        server_->Stop();
        return info.Env().Undefined();
    }

    Napi::Value Wait(const Napi::CallbackInfo& info) {
        server_->Wait();
        return info.Env().Undefined();
    }

    Napi::Value RegisterStreamHandler(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        if (info.Length() < 1 || !info[0].IsFunction()) {
            Napi::TypeError::New(env, "Function expected").ThrowAsJavaScriptException();
            return env.Undefined();
        }

        auto tsfn = Napi::ThreadSafeFunction::New(
            env,
            info[0].As<Napi::Function>(),
            "StreamHandler",
            0,
            1
        );

        server_->RegisterStreamHandler([tsfn](const networklib::StreamEnvelope& req, networklib::StreamEnvelope& resp, std::shared_ptr<networklib::IStreamContext> ctx) mutable {
            std::string req_data = req.SerializeAsString();

            auto callback = [req_data](Napi::Env env, Napi::Function jsCallback) {
                Napi::Buffer<char> buf = Napi::Buffer<char>::Copy(env, req_data.data(), req_data.size());
                jsCallback.Call({buf});
            };

            tsfn.BlockingCall(callback);
        });

        return env.Undefined();
    }
};

Napi::FunctionReference ServerWrapper::constructor;

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return ServerWrapper::Init(env, exports);
}

NODE_API_MODULE(networklib_node, Init)
