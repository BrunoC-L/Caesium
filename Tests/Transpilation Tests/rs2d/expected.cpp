#include "defaults.hpp"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

using sf__Time = sf::Time;
using sf__Clock = sf::Clock;
using sf__TcpSocket = sf::TcpSocket;
using sf__TcpListener = sf::TcpListener;
using sf__SocketSelector = sf::SocketSelector;
using sf__Socket = sf::Socket;
using sf__Socket__Status = sf::Socket::Status;
struct reference_wrapper__DB;
struct ServerServiceProvider__DB;
struct DB;

struct ServerServiceProvider__DB {
	Tuple<Optional<reference_wrapper__DB>> services;
};

struct DB {
ServerServiceProvider__DB provider;
};


Void provide__DB__DB(ServerServiceProvider__DB& provider, const DB& service);
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& args);

Void provide__DB__DB(ServerServiceProvider__DB& provider, const DB& service) {
							provider.services[0] = { service, };
	};
Int _redirect_main(const Vector_String_& args) {
	ServerServiceProvider__DB provider = ServerServiceProvider__DB{{ None, }};
	DB db = DB{provider};
	provide__DB__DB(provider, db);
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
