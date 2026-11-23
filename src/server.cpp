#include <thread>
#include <udp_connection.h>

#include <flexmethod.h>
#include <flexpoint.h>
#include <flexservice.h>
#include <flexregistry.h>
#include <flexrpc_proto/common.pb.h>

int main() {
    flexrpc::Registry registry{};
    flexrpc::SystemService sys{registry};
    sys.init();

    registry.invoke(0, {}, {});
    registry.invoke(1, {}, {});

    std::array<uint8_t, sizeof(Header)> buffer{};

    flexrpc::utilities::UdpEndpoint srv{7778};
    if (!srv.open_socket()) { return -1; }
    srv.enable_broadcast();
    flexrpc::utilities::UdpEndpoint::Endpoint bc_dst{"10.35.255.255", 7777};

    for (;;) {
        Header out;
        out.set_id(32);
        out.set_cmd(Command::DISCOVER);
        const size_t byte_size = out.ByteSizeLong();
        auto res = out.SerializeToArray(buffer.data(), byte_size);
        if (!res) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }

        srv.send_to(bc_dst, std::span<const uint8_t>{buffer.data(), byte_size});
        auto recv = srv.poll_from(buffer);
        if (recv.size <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }

        Header in;
        auto resp = in.ParseFromArray(buffer.data(), recv.size);
        if (!resp) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }
        if (in.cmd() == Command::HEARTBEAT) {
            printf("Discovered %s\n", recv.src.ip().c_str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    return 0;
}
