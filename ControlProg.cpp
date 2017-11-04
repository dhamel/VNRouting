#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <map>
#include <vector>

#include "Common.hpp"

using namespace std;

map<int, int>ports;
map<int, string> hostnames;


int main(int argc, char **argv)
{
  if(argc != 2)
  {
    cout << "USAGE: " << argv[0] << " <input file path>" << endl;
    return -1;
  }
  /* TODO Read in port/address info for each Node in network */

  string filename = argv[1];
  
  vector<pair<int, int> > control_ports = getAllContPorts(filename);
  vector<pair<int, string> > node_hostnames = getAllHostnames(filename);
  
  for(int i = 0; i < control_ports.size(); ++i)
  {
    ports.insert(control_ports[i]);
    hostnames.insert(node_hostnames[i]);
    cout << control_ports[i].first << " " << control_ports[i].second << endl;
    cout << node_hostnames[i].first << " " << node_hostnames[i].second << endl;
  }

  struct sockaddr_in send_sock, send_data;
  struct sockaddr_in cont_sockaddr;

  memset(&cont_sockaddr, 0, sizeof(cont_sockaddr));
  cont_sockaddr.sin_family = AF_INET;
  cont_sockaddr.sin_port = htons(0);
  cont_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  int sd = createSock((void*)&cont_sockaddr);
  
  ControlHeader header;
  ControlData data;
  
  char data_packet[PACKET_SIZE_BYTES];

  while(1)
  {
    string input, command, unused_data;
    int node1, node2;
  
    cout << "Enter Command: ";
    getline(cin, input);
    cout << input << endl;

    istringstream iss(input);
    if(!(iss >> command >> node1 >> node2))    /* needs to be in the right format */
    {
      cout << "Command should be given as: <command> <node #1> <node #2>" << endl;
    }
    else if(iss >> unused_data)    /* there shouldn't be more arguments */
    {
      cout << "Command should be given as: <command> <node #1> <node #2>" << endl;
    }
    else
    {
      if(command == "generate-packet")
      {
	memset(data_packet, 0, PACKET_SIZE_BYTES);
	header.pkt_type = CTRL_MSG_SEND_PCKT;
	data.node_ids[0] = node2;
	
	send_sock.sin_family = AF_INET;
	send_sock.sin_port = htons(ports.at(node1));

	struct hostent* hInfo = gethostbyname(hostnames.at(node1).c_str());

	memcpy(&send_sock.sin_addr, hInfo->h_addr, hInfo->h_length);

	cout << "heyo from controlprog" << endl;
	sendto(sd, (const void*)data_packet, PACKET_SIZE_BYTES, 0, (struct sockaddr*)&send_sock, sizeof(sockaddr));

      }
      else if(command == "create-link")
      {

      }
      else if(command == "remove-link")
      {

      }
      else /* needs to give a valid request */
      {
	cout << "Please give a valid request: generate-packet, create-link, or remove-link" << endl;
      }
    } 
  }
}
