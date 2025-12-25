#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "networklib/protocols/grpc/grpc_client.h"
#include "networklib/core/event/event_loop.h"

namespace py = pybind11;
using namespace networklib;

PYBIND11_MODULE(networklib_py, m) {
    m.doc() = "Python bindings for Network Library";

    py::class_<core::event::EventLoop, std::shared_ptr<core::event::EventLoop>>(m, "EventLoop")
        .def(py::init<>())
        .def("Run", &core::event::EventLoop::Run)
        .def("Stop", &core::event::EventLoop::Stop);

    py::class_<protocols::grpc::GrpcClient>(m, "GrpcClient")
        .def(py::init<std::shared_ptr<core::event::EventLoop>>())
        .def("Connect", &protocols::grpc::GrpcClient::Connect)
        .def("SendUnary", &protocols::grpc::GrpcClient::SendUnary);
}
