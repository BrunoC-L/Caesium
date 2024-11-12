#include "defaults.hpp"

using sf__sf_time = sf::sf_time;
using sf__clock = sf::clock;
using sf__TcpSocket = sf::TcpSocket;
using sf__TcpListener = sf::TcpListener;
using sf__SocketSelector = sf::SocketSelector;
using sf__Socket = sf::Socket;
using sf__SocketStatus = sf::SocketStatus;
struct A;
struct B;
struct a__b__c;

struct A {
};

struct B {
Int a;
};

struct a__b__c {
};


Int _redirect_main(const Vector<String>& args);

Int _redirect_main(const Vector<String>& args) {
	B x = B{0};
	A y = A{};
	B z = B{0};
	sf__clock clock = sf__clock{};
	a__b__c d = a__b__c{};
	return z.a;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
