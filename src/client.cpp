#include <thread>
#include <udp_connection.h>

#include <flexpoint.h>
#include <flexrpc_proto/common.pb.h>

int main() {
    std::array<uint8_t, sizeof(Header)> buffer{};
    flexrpc::utilities::UdpEndpoint cli{7777};
    if (!cli.open_socket()) { return -1; }

    for (;;) {
        auto srv = cli.poll_from(buffer);
        if (srv.size <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }

        Header incoming;
        auto res = incoming.ParseFromArray(buffer.data(), srv.size);
        if (!res) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }

        switch(incoming.cmd()) {
        case Command::DISCOVER:
        {
            Header out;
            out.set_id(12);
            out.set_cmd(Command::HEARTBEAT);
            printf("Discover from %s of size: %zu\n", srv.src.ip().c_str(), srv.size);
            if (out.SerializeToArray(buffer.data(), sizeof(Header))) {
                cli.send_to(srv.src, std::span<const uint8_t>{buffer.data(), out.ByteSizeLong()});
            }
            break;
        }
        default:
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    return 0;
}
