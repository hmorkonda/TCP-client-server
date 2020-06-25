#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
struct cityWeather {
	string cityName, feel, temp;
} city_weathers[20];


int city_count = 0;

int readWeatherFromFile(string filename) {
	string line;

	ifstream readfile;
	readfile.open(filename.c_str());

	if (!readfile.is_open()) {
		cout << "didnt open input file " << endl;
		return -1;
	}

	cout << "input is open" << endl;

	while (getline(readfile, line)) {
		istringstream ss(line);
		string token;

		getline(ss, token, ',');
		cout << "city name: " << token << endl;
		city_weathers[city_count].cityName = token;

		getline(ss, token, ',');
		cout << "temp: " << token << endl;
		city_weathers[city_count].temp = token;

		getline(ss, token, ',');
		cout << "weather: " << token << endl;
		city_weathers[city_count].feel = token;

		city_count++;
	}

	return 0;
}

int findCity(string cityname) {
	int i;

	for (i = 0; i < city_count; i++) {
		if (city_weathers[i].cityName == cityname) {
			return i;
		}
	}

	return -1;
}


string getResponse(int index) {
	if (index == -1) {
		string response = "No data";
		return response;
	}

	cityWeather city = city_weathers[index];

	string response = "Tomorrow's maximum temperature is: " + city.temp + "\n" +
						"Tomorrow's sky condition is: " + city.feel + "\n" ;
	return response;
}
int main() {

	//source for most of the socket functions: https://www.youtube.com/watch?v=cNdlrbZSkyQ

	int read_result = readWeatherFromFile("weather20.txt"); // dont take it this way

	int port;

	cout << "server program.." << endl;
	cout << "enter port number: ";
	cin >> port;

	//create a socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		cerr << "cant create a socket..!" << endl;
		return -1;
	}

	//bind the socket to an IP / sort
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	//hint.sin_port = htons(54000); //needs to be port
	hint.sin_port = htons(port); 

	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); 

	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
		cerr << "cant bind to ip/port";
		return -2;
	}

	//mark the socket for listening in 
	if (listen(listening, SOMAXCONN) == -1) {
		cerr << "cant listen" << endl;
		return -3;
	}

	//accept a call
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if (clientSocket == -1) {
		cerr << "problem with client connecting";
		//return -4;
	}

	//close the listening port
	//close(listening);

	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	if (result) {
		cout << host << " !!connected on " << svc << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on!! " << ntohs(client.sin_port) << endl;
	}

	//while receiving - display message, echo message
	char buf[4096];
	while (true){

		//clear the buffer;
		memset(buf, 0, 4096);

		//wait for message
		int bytesRecv = recv(clientSocket, buf, 4096, 0);
		if (bytesRecv == -1) {
			cerr << "there was a connection issue.." << endl;
			break;
		}
		if (bytesRecv == 0) {
			cout << "The client disconnected.. " << endl;
			break;
		}

		//display message
		cout << "recieved: " << string(buf, 0, bytesRecv) << endl;

		//----
		string cityname(buf);
		int index = findCity(cityname);
		string response = getResponse(index);
		cout << "Weather report for " << cityname << endl;
		cout << response << endl;
		//----

		//resend message;
		send(clientSocket, response.c_str(), response.size(), 0);

		//send(clientSocket, buf, bytesRecv + 1, 0);

		//so that server still runs even after client is disconnected..
		clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	}

	//close socket
	close(clientSocket);
	return 0;
}