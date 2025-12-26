#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "networklib/network_lib.h"
#include "stream_envelope.pb.h"

namespace py = pybind11;
using namespace networklib;

PYBIND11_MODULE(networklib_py, m) {
    m.doc() = "Python bindings for Network Library";

    // Forward declarations
    py::class_<IServer, std::shared_ptr<IServer>>(m, "IServer")
        .def("start", &IServer::Start)
        .def("stop", &IServer::Stop)
        .def("wait", &IServer::Wait)
        .def("register_stream_handler", [](IServer& self, py::function callback) {
            self.RegisterStreamHandler([callback](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> /*ctx*/) {
                // Acquire GIL for Python call
                py::gil_scoped_acquire gil;

                // Serialize Request
                std::string req_str = req.SerializeAsString();
                py::bytes req_bytes(req_str);

                // Call Python: resp_bytes = callback(req_bytes)
                // TODO: Pass context to Python!
                // For now, simplify: python callback returns response bytes.
                try {
                    py::object result = callback(req_bytes);

                    if (!result.is_none()) {
                        std::string resp_str = py::cast<std::string>(result);
                        bool parsed = resp.ParseFromString(resp_str);
                        if (!parsed) {
                            throw std::runtime_error("Failed to parse response from Python");
                        }
                    }
                } catch (const std::exception& e) {
                    // Log or handle error?
                    // For now, we propagate to C++?
                    // ProtocolHandler catches exceptions and sends 500.
                    throw;
                }
            });
        });

    py::class_<NetworkLib>(m, "NetworkLib")
        .def_static("create_server", [](const std::string& config_path) {
            return NetworkLib::CreateServer(config_path);
        });
}
