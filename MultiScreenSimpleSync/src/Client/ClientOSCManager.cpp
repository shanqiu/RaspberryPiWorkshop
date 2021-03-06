//
//  ClientOSCManager.cpp
//  MultiScreenCommonTiming
//
//  Created by Andreas Müller on 25/12/2012.
//
//

#include "ClientOSCManager.h"

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ClientOSCManager::ClientOSCManager()
{
	haveSetupSender = false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ClientOSCManager::~ClientOSCManager()
{
	ofRemoveListener(ofEvents().update, this, &ClientOSCManager::_update);	
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ClientOSCManager::init( int _uniqueComputerID )
{
	uniqueComputerID = _uniqueComputerID;
	
	receiver.setup( 7778 );
	
	commonTimeOsc.init( &sender, uniqueComputerID );
	
	ofAddListener(ofEvents().update, this, &ClientOSCManager::_update );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ClientOSCManager::_update(ofEventArgs &e)
{
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		
		//cout << "Got message: " << m.getAddress() << endl;
		
		if( m.getAddress() == "/hello" )
		{
			// if we get a hello message and we haven't set up our sender, get the Ip and port from the hello message
			if( !haveSetupSender )
			{
				int sendPort = m.getArgAsInt32(0);
				sender.setup( m.getRemoteIp(), sendPort );
				commonTimeOsc.senderIsSetup( true );
				haveSetupSender = true;
			}
		}
		else if( m.getAddress() == "/pong" )
		{
			// pong messages come in as: /pong, receiving computer ID (int), server timestamp (int), timestamp when remote computer sent (int)
			
			if( m.getArgAsInt32(0) == uniqueComputerID )  	// if the message is for us
			{
				int serverTime = m.getArgAsInt32(1);
				int originalTimeStamp = m.getArgAsInt32(2);
				
				commonTimeOsc.newReading( serverTime, commonTimeOsc.getInternalTimeMillis() - originalTimeStamp );
			}
		}
		else if( m.getAddress() == "/change_scene" )
		{
			int sceneIndex = m.getArgAsInt32(0);
			ofSendMessage("change_scene " + ofToString(sceneIndex) );
		}
	}
	
}