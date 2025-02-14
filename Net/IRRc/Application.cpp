//author randomMesh
#define IRRc
#ifdef IRRc
#include "Application.h"
#include <enet/enet.h>

//!TODO started porting process to enet, need to setup receive still possibly try threading aswell.


//#define __Linux  // for boost
#ifdef __Linux
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <iostream>
#include <algorithm>
#include <fstream>

#define test
#ifdef testf
	const std::string serverName("127.0.0.1");
	const unsigned short serverPort = 6668;
	const std::string channelName("#luna2");
	int aha = rand();
	const std::string botName("aBot");
#else
    const std::string serverName("irc.starchat.net");
	const unsigned short serverPort = 6667;
	const std::string channelName("#luna2");
	int aha = rand();
	const std::string botName("aevBot");
#endif

    ENetEvent event;
    ENetHost * host=0;
    ENetPeer * peer=0;
    ENetAddress address;
void ServerLoop();

std::vector<std::string> tokens2;

std::vector<std::string> tokenize(const std::string & delim , const std::string & str ){
  using namespace std;
  vector<string> tokens;

  size_t p0 = 0, p1 = string::npos;
  while(p0 != string::npos)
  {
    p1 = str.find_first_of(delim, p0);
    if(p1 != p0)
    {
      string token = str.substr(p0, p1 - p0);
      tokens.push_back(token);
    }
    p0 = str.find_first_not_of(delim, p1);
  }

  return tokens;
}

Application::Application(irr::IrrlichtDevice *device2 ) :
  device(0), gui(0), running(false)
   // device(0), gui(0), socket(0), running(false)
   {	device = device2;

   }

Application::~Application(){
	//delete all topics
	std::vector<Topic*>::iterator start = topics.begin();
	const std::vector<Topic*>::const_iterator end = topics.end();
	for (; start != end; ++start)
	{
		delete (*start);
	}
	sendMessage("QUIT :Bye\r\n");
//	delete socket;
	delete gui;
	device->drop();
}

bool Application::init(){
    #ifdef independent
    this->device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2du(800, 600), 16, false);
    #endif
	if (!this->device)
		return false;
	this->device->setEventReceiver(this);
	this->device->setWindowCaption(L"Irrlicht Help Bot");

	//todo: read configuration

	gui = new Gui(device->getGUIEnvironment());
	if (!readTopics())
		return false;
	ServerLoop();
	//create socket

//	try
//	{
//		socket = new TCPSocket(serverName, serverPort);
//		running = true;
//	}
//	catch (SocketException& e)
//	{
//		std::cout << "Fatal error: Could not create Socket." << std::endl;
//		return false;
//	}

address.port = 6667;

        printf("I am client...\n");
        fflush(stdout);

        host = enet_host_create (0, // create a client host
                1, // allow only 1 outgoing connection
                0, // use 57600 / 8 for 56K modem with 56 Kbps downstream bandwidth
                0);// use 14400 / 8 for 56K modem with 14 Kbps upstream bandwidth

        if (!host) {
            fprintf(stderr,"An error occurred while trying to create an ENet client host.\n");
            exit (EXIT_FAILURE);
        }

        // connect to server:
        enet_address_set_host (&address, "irc.starchat.net");
        peer = enet_host_connect (host, &address, 2);
        peer->data=0; // use this as mark that connection is not yet acknowledged
        if (!peer) {
           fprintf(stderr,"No available peers for initiating an ENet connection.\n");
           exit (EXIT_FAILURE);
        }

	#ifdef __Linux  /// change this for pthreads or something
	//start listen tread
	//boost::thread thrd1(boost::bind(&receive, this));
	#endif
	//connect to irc channel
	sendMessage("NICK " + botName + "\r\n");
	sendMessage("USER Username hostname servername :" + botName + "\r\n");
	#ifdef test
        device->sleep(6220,0); // allow server to auth non ident'd client
    #endif
	sendMessage("JOIN " + channelName + "\r\n");
	return true;
}

void Application::ServerLoop(){  //newEnet
address.host = serverName.c_str();
address.port = serverPort;
        // processing incoming events:
        while (enet_host_service (host, &event, 1000) > 0) {
            printf("Checking Incoming");
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                printf("  A new connected has been established to %u:%u\n",
                    event.peer -> address.host,event.peer-> address.port);
                fflush(stdout);

                char buf[64];
                sprintf(buf,"%u:%u",event.peer -> address.host, event.peer -> address.port);
                int buflen=strlen(buf);
                event.peer -> data=malloc(buflen+1);
                strncpy((char*)event.peer -> data,buf,buflen);
                peer=event.peer;
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:

                    printf("%s says %s on channel %u\n",
                        (char*)event.peer -> data,event.packet -> data,event.channelID);
                    fflush(stdout);

                enet_packet_destroy (event.packet); // clean up the packet now that we're done using it

                   //  SendPacket(0,"Data Received");
                     sendMessage ("Data Received") ;
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("  host disconnected.\n");
                fflush(stdout);
                free(event.peer -> data);
                event.peer -> data = 0; // reset the peer's client information.
                peer=0;
            default:
                break;
            }
        }
}



bool Application::readTopics(){
	std::ifstream a_file("./media/answers.txt", std::ios::in);
	if (!a_file.is_open())
	{
		std::cout << "Fatal error: Could not read answer list." << std::endl;
		return false;
	}
	else
	{
		std::string line;
		while(!a_file.eof())
		{
			getline(a_file, line);

			const std::string::size_type loc = line.find(':');
			if (!line.compare("") || loc == std::string::npos) { /*empty line or missing ':' */	}
			else
			{
				std::string question(line.substr(0, line.find(':')));
				std::transform(question.begin(), question.end(), question.begin(), (int(*)(int))std::tolower);
				const std::string answer(line.substr(line.find(':')+1));

				createTopic(question, answer);
			}
		}
	}

	a_file.close();

	return true;
}

bool Application::writeTopics() const {
	std::ofstream a_file("./media/answers.txt", std::ios::out);
	if (!a_file.is_open())
	{
		std::cout << "Fatal error: Could not write answer list." << std::endl;
		return false;
	}
	else
	{
		unsigned int i;
		for (i = 0; i < topics.size(); ++i)
		{
			a_file << topics[i]->getQuestion() << ":" << topics[i]->getAnswer() << "\n";
		}
	}
	a_file.close();
	return true;
}

void Application::registerIrrDevice(irr::IrrlichtDevice *device2){
	device = device2;
    guienv = device->getGUIEnvironment();
    driver = device->getVideoDriver();
}

void Application::run() const{
#ifdef independents
	   irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
 irr::video::IVideoDriver* driver = device->getVideoDriver();


	while (device->run())
	{
	//	ServerLoop();
		receive();
	//	if (device->isWindowActive())
	//	{
			driver->beginScene(true, false, irr::video::SColor());
			guienv->drawAll();
			driver->endScene();
		//}
//		device->sleep(2,0);
	}
#endif

receive();

}

bool Application::sendMessage(const std::string& message) const{

	int Channel=0;
	//char pack[11] = message.c_str();;


//	try
//	{
//			    ENetPacket * packet = enet_packet_create (pack,
//                                              strlen (pack) + 1,
//                                              ENET_PACKET_FLAG_RELIABLE);

			    ENetPacket * packet = enet_packet_create (message.c_str(),
                                              message.size(),
                                              ENET_PACKET_FLAG_RELIABLE);

    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    //enet_peer_send (peer, Channel, packet);
        enet_peer_send (peer, Channel, packet);

    /* One could just use enet_host_service() instead. */
    enet_host_flush (host);

		//socket->send(message.c_str(), message.size());
		return true;
//	}
//	catch (SocketException& e)
//	{
//		std::cout << "Warning: Could not send message: " << e.what() << std::endl;
//
//		return false;
//	}


}

void Application::receive(){

			char currentByte = '\0';
			std::string message;

			//read a line, byte by byte...
//			while(currentByte != '\r' && app->socket->recv(&currentByte, 1) > 0)
//			{
//				message += currentByte;
//			}

			//remove ' ', '\r' and '\n'
			std::string trimmedMessage(trimString(message));

//			handleIncommingMessages(trimmedMessage);


}

void Application::handleIncommingMessages(std::string& message){
	std::string transformedIRCMessage(message);
	std::transform(transformedIRCMessage.begin(), transformedIRCMessage.end(), transformedIRCMessage.begin(), (int(*)(int))std::tolower);

	if (transformedIRCMessage.substr(0, 4).compare("ping"))
	{

		std::string prefix = "";
		std::string transformedIrcCommand = "";
		std::string parameters = "";

		if(!transformedIRCMessage.substr(0, 1).compare(":"))
		{
			//we have a prefix
			prefix = message.substr(1, message.find(' '));

			//truncate message
			message = message.substr(message.find(' ') + 1);
		}

		// extract the command and make it lowercase
		transformedIrcCommand = message.substr(0, message.find(' '));
		std::transform(transformedIrcCommand.begin(), transformedIrcCommand.end(), transformedIrcCommand.begin(), (int(*)(int))std::tolower);

		// extract the parameter
		parameters = message.substr(message.find(' ') + 1);

		//std::cout << "Prefix: " << prefix << std::endl;
		//std::cout << "Command: " << command << std::endl;
		std::cout << "Parameters: " << parameters << std::endl;

		if (!transformedIrcCommand.compare("001")) {
		    /* This is the first point where a script or client can be sure that the user's
		    connection has been accepted without errors or problems */
		    }

		else if (!transformedIrcCommand.compare("002")) { /* name and version of the server */ }
		else if (!transformedIrcCommand.compare("003")) { /* The date and time the server was created or last restarted. */ }

		else if (!transformedIrcCommand.compare("004")) {
                /* All user modes and channel modes (including o for IRC operator, etc.)
              are listed, however not all may be accessable to a given user. */
              //  gui->addLine(irr::core::stringw(L"channelmodes"), message.c_str());
              /*
               aBot WALLCHOPS WALLVOICES MODES=19 CHANTYPES=# PREFIX=(ohv)@%+ MAP
               MAXCHANNELS=20 MAXBANS=60 VBANLIST NICKLEN=31 CASEMAPPING=rfc1459 STATUSMSG=@%+
               CHARSET=ascii :are supported by this server
            */
             //   std::cout << message.c_str();
                }

		else if (!transformedIrcCommand.compare("005")) { }
		else if (!transformedIrcCommand.compare("250")) { }
		else if (!transformedIrcCommand.compare("251")) { /* Number of users (visible and invisible) */ }
		else if (!transformedIrcCommand.compare("252")) { }
		else if (!transformedIrcCommand.compare("254")) { }
		else if (!transformedIrcCommand.compare("255")) { }
		else if (!transformedIrcCommand.compare("265")) { }
		else if (!transformedIrcCommand.compare("266")) { }
		else if (!transformedIrcCommand.compare("331"))
            { /* Channel has no topic */
                gui->addLine(irr::core::stringw(L"notopicFlag Here"), message.c_str());
            }

		else if (!transformedIrcCommand.compare("332"))
            { /* Topic */
                message = message.substr(message.find(':') + 1);
                gui->addLine(irr::core::stringw(L"Topic :"), message.c_str());
            }

		else if (!transformedIrcCommand.compare("333"))
            { /* If channel has a topic, show user who set topic and when*/
         //       gui->addLine(irr::core::stringw(L"User Set Topic :"), message.c_str());
            }

		else if (!transformedIrcCommand.compare("353"))
            {  /* names list */
            //refresh the whole list every 10 min otherwise just user join part msgs
            tokens2 = tokenize(" " , message.c_str());
            for( int x=4;x<tokens2.size();x++)
                {
                const irr::core::stringw message3(tokens2[x].c_str());
                gui->list->addItem((wchar_t*) message3.c_str());
                }
            }

		else if (!transformedIrcCommand.compare("366")) { /* end of names list */ }

		else if (!transformedIrcCommand.compare("375")) { /* RPL_MOTD START */ }
		else if (!transformedIrcCommand.compare("372")) { /* RPL_MOTD */ }
		else if (!transformedIrcCommand.compare("376")) { /* RPL_MOTD END */ }
		else if (!transformedIrcCommand.compare("privmsg"))
		{
			std::string username;

			const std::string messageText = parameters.substr(parameters.find(':')+1);

			const bool publicMessage = (parameters[0] == '#');
			if (publicMessage) //message to channel
			{
				username = parameters.substr(0, parameters.find(' ')); //channel name

				//update gui
				const irr::core::stringw who(prefix.substr(0, prefix.find("!")).c_str());
				this->gui->addLine(who + " : ", irr::core::stringw(messageText.c_str()));
			}
			else //private message
			{
				if(prefix.find('!') != std::string::npos)
					username = prefix.substr(0, prefix.find("!"));
				else username = prefix;
			}


            #include "botFuncts.h"  // just temporary to cleanup code

}




void Application::createTopic(const std::string& question, const std::string& answer)
{
	Topic* const topic = new Topic(question, answer);
	topics.push_back(topic);
}

void Application::deleteTopic(const std::string& question)
{
	std::vector<Topic*>::iterator start = topics.begin();
	const std::vector<Topic*>::const_iterator end = topics.end();
	for (; start != end; ++start)
	{
		if (!((*start))->getQuestion().compare(question))
		{
			delete (*start);
			topics.erase(start);

			return;
		}
	}
}

Application::Topic* const Application::findTopicInList(const std::string& question) const
{
	unsigned int i;
	for (i = 0; i< topics.size(); ++i) //should use regex here and return a vector of iterators
	{
		if (!topics[i]->getQuestion().compare(question))
			return topics[i];
	}

	return 0;
}

bool Application::OnEvent(const irr::SEvent& event)
{
	switch(event.EventType)
	{
	case irr::EET_KEY_INPUT_EVENT:
	{
		if (!event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{

			case irr::KEY_ESCAPE: running = false; device->closeDevice(); return true;

			case irr::KEY_RETURN:
			{
				const std::string message(irr::core::stringc(this->gui->getBox()->getText()).c_str());
				if (message.compare("") != 0)
				{
					sendMessage("PRIVMSG " + channelName + " :" + message + "\r\n");
					gui->addLine(irr::core::stringw( botName.c_str() ) + " : ", message.c_str());
				}
				this->gui->getBox()->setText(L"");

			}
			return true;

			default: return false;
			}
		}
	}
	break;

	default: return false;
	}

	return false;
}
#endif
