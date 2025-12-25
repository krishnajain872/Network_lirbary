#include <napi.h>
#include "networklib/protocols/grpc/grpc_client.h"
#include "networklib/core/event/event_loop.h"

// Wrapper for EventLoop
class NodeEventLoop : public Napi::ObjectWrap<NodeEventLoop> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "EventLoop", {
            InstanceMethod("run", &NodeEventLoop::Run),
            InstanceMethod("stop", &NodeEventLoop::Stop)
        });
        Napi::FunctionReference* constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData(constructor);
        exports.Set("EventLoop", func);
        return exports;
    }

    NodeEventLoop(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodeEventLoop>(info) {
        loop_ = std::make_shared<networklib::core::event::EventLoop>();
        loop_->Init();
    }

    Napi::Value Run(const Napi::CallbackInfo& info) {
        // Run in a separate thread because it blocks? 
        // For N-API, usually we want non-blocking integration. 
        // This is a simplified binding.
        loop_->Run();
        return info.Env().Undefined();
    }

    Napi::Value Stop(const Napi::CallbackInfo& info) {
        loop_->Stop();
        return info.Env().Undefined();
    }

    std::shared_ptr<networklib::core::event::EventLoop> GetInternal() { return loop_; }

private:
    std::shared_ptr<networklib::core::event::EventLoop> loop_;
};

// Wrapper for GrpcClient
class NodeGrpcClient : public Napi::ObjectWrap<NodeGrpcClient> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "GrpcClient", {
            InstanceMethod("connect", &NodeGrpcClient::Connect),
            InstanceMethod("sendUnary", &NodeGrpcClient::SendUnary)
        });
        exports.Set("GrpcClient", func);
        return exports;
    }

    NodeGrpcClient(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodeGrpcClient>(info) {
        if (info.Length() < 1 || !info[0].IsObject()) {
            Napi::TypeError::New(info.Env(), "EventLoop expected").ThrowAsJavaScriptException();
            return;
        }
        
        Napi::Object loopObj = info[0].As<Napi::Object>();
        NodeEventLoop* nodeLoop = Napi::ObjectWrap<NodeEventLoop>::Unwrap(loopObj);
        client_ = std::make_unique<networklib::protocols::grpc::GrpcClient>(nodeLoop->GetInternal());
    }

    Napi::Value Connect(const Napi::CallbackInfo& info) {
        std::string host = info[0].As<Napi::String>();
        int port = info[1].As<Napi::Number>().Int32Value();
        bool success = client_->Connect(host, port);
        return Napi::Boolean::New(info.Env(), success);
    }

    Napi::Value SendUnary(const Napi::CallbackInfo& info) {
        std::string path = info[0].As<Napi::String>();
        std::string payload = info[1].As<Napi::String>();
        client_->SendUnary(path, payload);
        return info.Env().Undefined();
    }

private:
    std::unique_ptr<networklib::protocols::grpc::GrpcClient> client_;
};

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    NodeEventLoop::Init(env, exports);
    return NodeGrpcClient::Init(env, exports);
}

NODE_API_MODULE(networklib_node, InitAll)
