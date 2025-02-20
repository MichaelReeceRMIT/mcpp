#pragma once

#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdexcept>
#include <initializer_list>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

namespace mcpp {

    class SocketConnection {
    private:
        boost::asio::ip::tcp::socket *socket;
        const std::string FailedCommandResponse = "Fail";
        std::string lastSent;

    public:
        explicit SocketConnection(const std::string& address_str,
                                  uint16_t port);

        void send(const std::string& dataString);

        [[nodiscard]] std::string recv() const;

        /**
         * Takes in parameters supporting std::stringstream conversion and a string prefix and transforms them into
         * format "prefix(arg1,arg2,arg3)\n" (e.g. "chat.post(test)\n") and sends command to the server.
         * @tparam Types
         * @param prefix
         * @param args
         */
        template<typename... Types>
        void sendCommand(const std::string& prefix, const Types& ...args) {
            std::stringstream ss;

            ss << prefix << "(";

            // iterate over args pack
            ((ss << args << ','), ...);
            // Remove trailing comma
            ss.seekp(-1, std::ios_base::end);

            ss << ")\n";

            send(ss.str());
        }

        /**
         * Sends via sendCommand() and returns the result from endpoint
         * @tparam T
         * @tparam Types
         * @param prefix
         * @param args
         * @return
         */
        template<typename T, typename... Types>
        std::string sendReceiveCommand(const T& prefix, const Types& ...args) {
            sendCommand(prefix, args...);
            auto result = recv();
            return result;
        }

        SocketConnection& operator=(const SocketConnection& other);

    private:
        [[nodiscard]] bool checkCommandFailed(const std::string& result) const;
    };
}