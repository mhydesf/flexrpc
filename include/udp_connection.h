#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <span>
#include <string>
#include <string_view>

namespace flexrpc::utilities {

typedef uint16_t port_t;
typedef uint32_t timeout_ms_t;

class UdpEndpoint final {
public:
    struct Endpoint {
        sockaddr_in addr_{};

        Endpoint(port_t port) {
            addr_.sin_family = AF_INET;
            addr_.sin_port = htons(port);
            addr_.sin_addr.s_addr = INADDR_ANY;
        }

        Endpoint(std::string_view iface, port_t port) {
            addr_.sin_family = AF_INET;
            addr_.sin_port = htons(port);
            inet_pton(AF_INET, iface.data(), &addr_.sin_addr);
        }

        sockaddr_in& sockaddr() { return addr_; }
        const sockaddr_in& sockaddr() const { return addr_; }

        std::string ip() const {
            std::array<char, INET_ADDRSTRLEN> buf;
            inet_ntop(AF_INET, &addr_.sin_addr, buf.data(), buf.size());
            return std::string{buf.data()};
        }

        port_t port() const { return ntohs(addr_.sin_port); }
    };

    struct UdpPacketInfo {
        ssize_t size{0};
        Endpoint src{0};
    };

    explicit UdpEndpoint(std::string_view iface, port_t port = 0) : iface_{iface, port} {}
    explicit UdpEndpoint(port_t port = 0) : iface_{port} {}
    ~UdpEndpoint() { close_socket(); }

    UdpEndpoint(const UdpEndpoint&) = delete;
    UdpEndpoint& operator=(const UdpEndpoint&) = delete;
    UdpEndpoint(UdpEndpoint&& other) = default;
    UdpEndpoint& operator=(UdpEndpoint&& other) = default;

    void enable_broadcast(bool enable = true) {
        if (!is_open()) { return; }
        int opt = enable ? 1 : 0;
        ::setsockopt(fd_, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    }

    void set_receive_timeout(timeout_ms_t timeout_ms) {}

    void send_to(const Endpoint& dst, std::span<const uint8_t> data) {
        if (!is_open()) { return; }
        ::sendto(fd_, data.data(), data.size(), 0, to_sockaddr(&dst.sockaddr()), sizeof(dst.sockaddr()));
    }

    UdpPacketInfo recv_from(std::span<uint8_t> buffer) {
        UdpPacketInfo pkt{};
        if (!is_open()) { return pkt; }
        socklen_t addr_len = sizeof(pkt.src.sockaddr());
        pkt.size = ::recvfrom(fd_, buffer.data(), buffer.size(), 0, to_sockaddr(&pkt.src.sockaddr()), &addr_len);
        return pkt;
    }

    UdpPacketInfo poll_from(std::span<uint8_t> buffer) {
        UdpPacketInfo pkt{};
        if (!is_open()) {  return pkt; }
        socklen_t addr_len = sizeof(pkt.src.sockaddr());
        pkt.size = ::recvfrom(fd_, buffer.data(), buffer.size(), MSG_DONTWAIT, to_sockaddr(&pkt.src.sockaddr()), &addr_len);
        return pkt;
    }

    bool open_socket() {
        // guard against socket already open
        if (is_open()) { return false; }

        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (!is_open()) { return false; }

        // TODO: enable sock opt to avoid EADDRINUSE for devel testing
        const int opt = 1;
        ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (::bind(fd_, to_sockaddr(&iface_.sockaddr()), sizeof(iface_.sockaddr())) < 0) {
            close_socket();
            return false;
        }

        return true;
    }

    void close_socket() { 
        if (!is_open()) { return; }
        ::close(fd_);
        fd_ = -1;
    }

private:
    bool is_open() { return fd_ >= 0; }
    static sockaddr* to_sockaddr(sockaddr_in* in) { return reinterpret_cast<sockaddr*>(in); }
    static const sockaddr* to_sockaddr(const sockaddr_in* in) { return reinterpret_cast<const sockaddr*>(in); }

private:
    Endpoint iface_;
    int fd_{-1};
};

}  // namespace flexrpc::utilities
