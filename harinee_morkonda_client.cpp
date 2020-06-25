#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

int main() {

	//source for some socket functions: https://www.youtube.com/watch?v=cNdlrbZSkyQ //thank you
	cout << "client running.." << endl;
	string hostname;
	int portnum;
	cout << "enter the hostname: ";
	cin >> hostname;

	struct hostent *hp;

	hp = gethostbyname(hostname.c_str());
	if (hp == NULL) {
		cout << "bad hostname.. did you mean 'localhost'? " << endl;
		return -1;
	}

	cout << "enter port num: ";
	cin >> portnum;

	//create a socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return -1;
	}
	//--




	//--
	//create a hint structure for the server we are connecting with
	int port = portnum;// change to the cin value portnum;
	//int port = 54000;// change to the cin value portnum;

	//string ipaddress = "127.0.0.1"; //change to hostname we take as well;

	sockaddr_in hint;

	//hint.sin_family = AF_INET;
	hint.sin_family = hp->h_addrtype;

	hint.sin_port = htons(port);

	//inet_pton(AF_INET, ipaddress.c_str(), &hint.sin_addr);
	inet_pton(AF_INET, hostname.c_str(), &hint.sin_addr);

	//connect to the server on the socket
	int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connectRes == -1) {
		return 1;
	}

	char buf[4096];
	string userInput;

	//while loop
	//cout << "Enter city name: "; //fix this
	getline(cin, userInput);
	//do {
	//while (true) {
		//enter lines of text
		cout << "> ";
		cout << "Enter a city name: "; //fix this

		getline(cin, userInput);

		//send to server
		int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
		//to do: check if that failed
		if (sendRes == -1) {
			cout << "could not send to server!" << endl;
			//continue;
		}
		//wait for response
		memset(buf, 0, 4096); //zeroing the buffer 
		int bytesReceived = recv(sock, buf, 4096, 0);

		//display response
		cout << "Weather Report for " <<userInput << endl << string(buf, bytesReceived) << "\r\n";
		close(sock);

	//}
	//} while (true);
	
	//close the socket
	close(sock);
	return 0;
}