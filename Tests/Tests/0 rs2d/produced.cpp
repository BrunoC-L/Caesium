#include "defaults.hpp"

using sf__sf_time = sf::sf_time;
using sf__clock = sf::clock;
using sf__TcpSocket = sf::TcpSocket;
using sf__TcpListener = sf::TcpListener;
using sf__SocketSelector = sf::SocketSelector;
using sf__Socket = sf::Socket;
using sf__SocketStatus = sf::SocketStatus;
using std__reference_wrapper = std::reference_wrapper<Ts...>;
struct reference_wrapper__DB;
struct ServerServiceProvider__DB;
struct DB;

struct reference_wrapper__DB {
DB _;
};

struct ServerServiceProvider__DB {
Tuple__Optional<std__reference_wrapper__DB> services;
};

struct DB {
ServerServiceProvider__DB provider;
};


Int _redirect_main(const Vector<String>& args);

Int _redirect_main(const Vector<String>& args) {
	ServerServiceProvider__DB provider = ServerServiceProvider__DB{{ None, }};
	DB db = DB{provider};
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
