#include <unistd.h>
#include <cstdio>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

//#define headlessGUI //define for console mode only

//#include "logging/logging.h"
//#define SQUIRREL
#ifdef SQUIRREL
#include "sqlCon.h"
#include "includes/squirrel.h"
#endif


#define headlessGUI
//#define COMPILE_WITH_IRRLICHT
#ifndef headlessGUI
#include <irrlicht.h>





using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
vector3df tmpvect;
#endif

#include <irrNet.h>
using namespace irr;

//#define SQL

void handleConnection();
bool doit;
bool connected,authorized;


//	u32 address = netManager->getClientAddress(playerId);

class ServerNetCallback : public net::INetCallback
{
 public:
	ServerNetCallback(net::INetManager* netManagerIn) : netManager(netManagerIn) {}

	// Override the "onConnect" function, don't forget to get the method
	// signature correct. Whenever a fresh client connects, their brand
	// spanking new player id will be passed to this function. A "u16"
	// is a typedef for an unsigned short, incase you were wondering.
	virtual void onConnect(const u16 playerId)
	{
	    connected=1;
        printf("connected - sending handshake \n");
			net::SOutPacket packet;
		//	core::stringc message;
		//	message = "first";
			//message += playerId;
			//message += " has just connected.";
            packet << "first";
            netManager->sendOutPacket(packet);
		//	netManager->sendOutPacket(packet);
	}

	virtual void onDisconnect(const u16 playerId)
	{
	    doit=0;
	    connected=0;
		net::SOutPacket packet;
		core::stringc message;
		message = "Client number ";
		message += playerId;
		message += " has just left the building.";
		packet << message;
		netManager->sendOutPacket(packet);
		doit,connected = false;
	}

	virtual void handlePacket(net::SInPacket& packet)
	{
        core::stringc str;
		packet >> str;
        printf("handling packet \n");
     //   core::vector3df yes;
	//	packet >> yes;

            if (str == "first"){
                connected=true; // do stuff were authed
                std::cout << "CONNECTED "  << str.c_str() <<std::endl;
            }

            if (str == "second"){
                connected=true; // do stuff were authed
                 doit=true; // do stuff were authed
                std::cout << "second2connECTED "  << str.c_str() <<std::endl;
            }
            if (str == "userLogin"){
                doit=true; // do stuff were authed

                std::cout << "AUTHORIZED with " << str.c_str() <<std::endl;
            }
            if (str.c_str() == "actPack2"){
                               doit=true;
                std::cout <<"Auth Communications 2" << str.c_str() <<std::endl;
            }else{
                std::cout <<"random string" << str.c_str() <<std::endl;
            }

        //    tmpvect = vector3df(yes);

	}
private:
	core::array<u32> banList;
	net::INetManager* netManager;
};



int main()
{
#ifndef headlessGUI
                    std::cout << "Create IrrDevice " ;
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
                                            false, false, false, 0);

                                            if (!device)return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
	guienv->addStaticText(L"Server!",
		rect<s32>(10,10,260,22), true);

	IAnimatedMesh* mesh = smgr->getMesh("./media/sydney.md2");
	if (!mesh)return 1;

	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );
	if (node)
	{   node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture("./media/sydney.bmp") );
	}
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
#endif
    //log << std::string("loop");
		//  The default port that clients can connect to is set to 45000.
		net::INetManager* netManager = net::createIrrNetServer(0);
		ServerNetCallback* serverCallback = new ServerNetCallback(netManager);
		netManager->setNetCallback(serverCallback);
          netManager->setVerbose(true);


    //! TEST AREA FOR SCRIPTING AND SQL
 //   Squirrel *SQ;
 //   SQ->do_main();
 //   SQ->LoadFile("test.nut");
//#define noSQL
#ifdef SQL
std::string a;
    sqlCon *sq =new sqlCon("ha.db");
    //  sq->execute(".dump");
          //  sq->execute("PRAGMA table_info(Coins);"); //testing

		int createcoins=0;
		if ( createcoins==1 ){
			sq->execute("CREATE TABLE Coins (coin integer, wallet string);");
			for (int coin=1;coin < 100 ;++coin){
				std::string test = "insert into Coins (coin,wallet) VALUES(" + std::to_string(coin) + " ,'ssssnipples');";
				//printf("%s",(char*)chr);
				const char *chr = test.c_str();
				sq->execute((char*)chr);
			}
		}

		// either scan through db to build coin list or remember what coins belong to your wallet
		// search several dabatase ranges
		  sq->execute("SELECT * FROM Coins where wallet like '%ssssnipples%';");
#endif


#ifndef headlessGUI

    while(device->run())
    {
        // std::cout << "Sleeping for 150 units\n " ;
        // device->sleep(150,0);


#else
while(1){
    if(netManager->getConnectionStatus() != net::EICS_FAILED)
    {
                net::SOutPacket packet;// To send a packet, first you create an SOutPacket object.
        if (connected==true)
        {
            if (!authorized){
                if (!doit)
                {
                    std::cout << "tell client to authorize \n " ;
                    packet << "second";
                       //    doit = false; // send only one
                     //    netManager->sendOutPacket(packet);
                }
                else
                {
                    std::cout << "handshake \n " ;
                    packet << "actPack2";
                                     doit = true;
    ////tmpvect.X = 5;
    ////            packet << vector3df(tmpvect);
    ////                      std::cout << "sending X=" << tmpvect.X << "Y=" << tmpvect.Y << "Z=" << tmpvect.Z ;
    ////
    ////            packet << vector3df(tmpvect);
    ////                      std::cout << "sending X=" << tmpvect.X << "Y=" << tmpvect.Y << "Z=" << tmpvect.Z ;
                }

    ////doit = false;
            }else{
            // take some authenticated data
            }
                   netManager->sendOutPacket(packet);
        } else {
           sleep(5);    //   device->sleep(50,0);
        };

            netManager->update(1000); //update(delay);
    }
}
#endif
#ifndef headlessGUI
        driver->beginScene(true, true, SColor(255,100,101,140));
        smgr->drawAll();
        guienv->drawAll();
        driver->endScene();


	device->drop();
    }
#endif
    delete netManager;
    delete serverCallback;
    return 0;
}
