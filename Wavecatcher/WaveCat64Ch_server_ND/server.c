/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    server.c                                                         */
/*                                                                           */
/* PURPOSE: This is a skeleton program to demonstrate how you would write a  */
/*          a TCP Server application. This sample registers a server on a    */
/*          speciable port number and waits for connections.  It will allow  */
/*          a connection from one client, and let the user send data back    */
/*          and forth.  For simplicity, the server refuses more than 1       */
/*          simultaneous client connection.                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <cvirte.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcpsupp.h>
#include <string.h>
#include <utility.h>
#include <userint.h>
#include "server.h"
#include "WaveCat64Ch_Sample.h"

/*---------------------------------------------------------------------------*/
/* Macros						                                             */
/*---------------------------------------------------------------------------*/
#define tcpChk(f) if ((g_TCPError=(f)) < 0) {ReportTCPError(); goto Done;} else

/*---------------------------------------------------------------------------*/
/* Internal function prototypes                                              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error,
							 void *callbackData);
static void ReportTCPError (void);

/*---------------------------------------------------------------------------*/
/* Module-globals                                                            */
/*---------------------------------------------------------------------------*/
static unsigned int g_hconversation;
static int          g_hmainPanel = 0;
static int			g_TCPError = 0;
static int		 	registered = 0;
static int      portNum;

/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
int Start_server ()
{
	char     tempBuf[256] = {0};
	lastEventSentOverTCP=-1;
	if ((g_hmainPanel = LoadPanel(0, "server.uir", MAINPNL)) < 0)
		return -1;
	DisableBreakOnLibraryErrors();

	/* Prompt for the port number on which to receive connections */
	/*
	PromptPopup ("Port Number?",
				 "Type the port number on which you would like to register "
				 "this server application.\n\n(example: 10000)",
				 tempBuf, 31);
	portNum = atoi (tempBuf);
	*/
	portNum = 21000;
	/* Attempt to register as a TCP server... */
	SetWaitCursor (1);

	if (RegisterTCPServer (portNum, ServerTCPCB, 0) < 0)
		MessagePopup("TCP Server", "Server registration failed!");
	else
	{
		registered = 1;
		SetWaitCursor (0);

		/* We are successfully registered -- gather info */
		SetCtrlVal (g_hmainPanel, MAINPNL_ONLINE, 1);
		if (GetTCPHostAddr (tempBuf, 256) >= 0)
			SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_IP, tempBuf);
		if (GetTCPHostName (tempBuf, 256) >= 0)
			SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_NAME, tempBuf);
		SetCtrlAttribute (g_hmainPanel, MAINPNL_STRING, ATTR_DIMMED, 1);
		SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0);

		/* Display the panel and run the UI */
		DisplayPanel (g_hmainPanel);
		SetActiveCtrl (g_hmainPanel, MAINPNL_STRING);
		//RunUserInterface ();
	}
	return 0;
} // Start_server

int Stop_server()
{
	if (registered)
		UnregisterTCPServer (portNum);

	/* Free resources and return */
	DiscardPanel (g_hmainPanel);
	CloseCVIRTE ();
	return 0;
} //Stop_server

/*---------------------------------------------------------------------------*/
/* When the user hits ENTER after typing some text, send it to the client... */
/* Note that the string control will be dimmed unless there is a client      */
/* connected.                                                                */
/*---------------------------------------------------------------------------*/
int CVICALLBACK TransmitCB (int panelHandle, int controlID, int event,
							void *callbackData, int eventData1, int eventData2)
{
	char transmitBuf[512] = {0};

	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, MAINPNL_STRING, transmitBuf);
			strcat (transmitBuf, "\n");
			SetCtrlVal (panelHandle, MAINPNL_TRANSMIT, transmitBuf);
			SetCtrlVal (panelHandle, MAINPNL_STRING, "");
			if (ServerTCPWrite (g_hconversation, transmitBuf,
								strlen (transmitBuf), 1000) < 0)
				SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,
							"Transmit Error\n");
			break;
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Sends a channel data over TCP              */
/*---------------------------------------------------------------------------*/
int transmitChannel(int channel, int firstSample, int lastSample)
{
#define CHARS_PER_RECORD 12
	char transmitBuf[512] = {0};
	char *longBuf;
	if (channel>=0)
	{
		int channelNum=CurrentEvent.ChannelData[channel].Channel;
		int channelDataSize=CurrentEvent.ChannelData[channel].WaveformDataSize;
		longBuf=malloc(CHARS_PER_RECORD*(channelDataSize+1));
		sprintf(transmitBuf,"Channel: %d\nSize: %d\n",channelNum,channelDataSize);
		if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
		{
			//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
		}
		if ((lastSample==-1)||(lastSample>channelDataSize))
		{
			lastSample=channelDataSize;
		}
		for (int idata=firstSample; idata < lastSample; idata++)
		{
			sprintf(longBuf+(idata-firstSample)*CHARS_PER_RECORD,"%6.4e  ",CurrentEvent.ChannelData[channel].WaveformData[idata]);
		} // for each data event
		sprintf(longBuf+(lastSample-firstSample)*CHARS_PER_RECORD,"\n");
		if (ServerTCPWrite (g_hconversation, longBuf, strlen (longBuf), 1000) < 0)
		{
			//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
		}
		free(longBuf);
		return 0;
	}
	else
	{
		return -1;
	}
} //transmitChannel

/*---------------------------------------------------------------------------*/
/* Saves channels data to file              */
/*---------------------------------------------------------------------------*/
int saveChannels(short int channels[8], char* filename)
{
	FILE* fid=fopen("test.txt","w+");
	if (fid!=NULL)
	{
		for (int ichan=0; ichan<8; ichan++)
		{
			if (channels[ichan]==1)
			{
				int channelNum=CurrentEvent.ChannelData[ichan].Channel;
				int channelDataSize=CurrentEvent.ChannelData[ichan].WaveformDataSize;
				fprintf(fid,"Channel: %d\nSize: %d\n",channelNum,channelDataSize);
				for (int idata=0; idata < channelDataSize; idata++)
				{
					fprintf(fid,"%6.4e ",CurrentEvent.ChannelData[ichan].WaveformData[idata]);
				} // for each data event
				fprintf(fid,"\n");
			} // if channel on
			else
			{
				//MessagePopup("Saving", " channel skipped");
			}
		} // for each channel
		fclose(fid);
		return 0;
	}
	else
	{
		//MessagePopup("Saving", " fopen failed");
		return -1;
	}
} //saveChannels




/*---------------------------------------------------------------------------*/
/* This is the TCP server's TCP callback.  This function will receive event  */
/* notification, similar to a UI callback, whenever a TCP event occurs.      */
/* We'll respond to CONNECT and DISCONNECT messages and indicate to the user */
/* when a client connects to or disconnects from us.  when we have a client  */
/* connected, we'll respond to the DATAREADY event and read in the available */
/* data from the client and display it.                                      */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error,
							 void *callbackData)
{
	char receiveBuf[256] = {0};
	char transmitBuf[512] = {0};
	ssize_t dataSize        = sizeof (receiveBuf) - 1;
	char addrBuf[31];
	int msg_value=MSG_UNKNOWN;

	switch (event)
	{
		case TCP_CONNECT:
			if (g_hconversation)
			{
				/* We already have one client, don't accept another... */
				tcpChk (GetTCPPeerAddr (handle, addrBuf, 31));
				sprintf (receiveBuf, "-- Refusing conection request from "
						 "%s --\n", addrBuf);
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				tcpChk (DisconnectTCPClient (handle));
			}
			else
			{
				/* Handle this new client connection */
				g_hconversation = handle;
				SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 1);
				tcpChk (GetTCPPeerAddr (g_hconversation, addrBuf, 31));
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_IP, addrBuf);
				//tcpChk (GetTCPPeerName (g_hconversation, receiveBuf, 256));
				//SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_NAME, receiveBuf);
				sprintf (receiveBuf, "-- New connection from %s --\n",
						 addrBuf);
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				SetCtrlAttribute (g_hmainPanel, MAINPNL_STRING, ATTR_DIMMED,
								  0);

				/* Set the disconect mode so we do not need to terminate */
				/* connections ourselves. */
				tcpChk (SetTCPDisconnectMode (g_hconversation,
											  TCP_DISCONNECT_AUTO));
			}
			break;
		case TCP_DATAREADY:
			if ((dataSize = ServerTCPRead (g_hconversation, receiveBuf,
										   dataSize, 1000))
					< 0)
			{
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "Receive Error\n");
			}
			else
			{
				int error=0;
				int posSpace=0;
				int errCode=0;
				char *token;
				receiveBuf[dataSize] = '\0';
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				sprintf(transmitBuf,"Not found\n");
				msg_value=MSG_UNKNOWN;
				if (strncmp(receiveBuf,"start",4)==0)  msg_value=MSG_START;
				if (strncmp(receiveBuf,"stop",4)==0)  msg_value=MSG_STOP;
				if (strncmp(receiveBuf,"update",4)==0)  msg_value=MSG_UPDATE;
				if (strncmp(receiveBuf,"display",4)==0)  msg_value=MSG_DISPLAY;
				if (strncmp(receiveBuf,"save",4)==0)  msg_value=MSG_SAVE;
				if (strncmp(receiveBuf,"trigger",4)==0)  msg_value=MSG_TRIG;
				if (strncmp(receiveBuf,"threshold",4)==0)  msg_value=MSG_THRESHOLD;
				if (strncmp(receiveBuf,"delay",4)==0)  msg_value=MSG_DELAY;
				if (strncmp(receiveBuf,"sampling",4)==0)  msg_value=MSG_SAMPLING;
				if (strncmp(receiveBuf,"data",4)==0)  msg_value=MSG_DATA;
				if (strncmp(receiveBuf,"channel",4)==0)  msg_value=MSG_CHAN;
				if (strncmp(receiveBuf,"range",4)==0)  msg_value=MSG_RANGE;
				if (strncmp(receiveBuf,"event",4)==0)  msg_value=MSG_EVT;
				if (strncmp(receiveBuf,"integrate",4)==0)  msg_value=MSG_INT;
				if (strncmp(receiveBuf,"substrate",4)==0)  msg_value=MSG_SUBSTRACT;
				if (strncmp(receiveBuf,"quit",4)==0)  msg_value=MSG_QUIT;
				switch(msg_value)
				{
					case MSG_START:
						//start
						//MessagePopup ("START!",receiveBuf);
						sprintf(transmitBuf,"Starting...\n");
						Start_Acquisition_TCP();
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received MSG START.");
						break;
					case MSG_STOP:
						sprintf(transmitBuf,"Stopping\n");
						StopAcquisition = TRUE;
						while(AcquisitionRunning == TRUE);
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received MSG STOP.");
						break;
					case MSG_UPDATE:
						sprintf(transmitBuf,"Updated\n");
						Update_Panel();
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
						break;
					case MSG_DATA:
						if (CurrentEvent.EventID!=lastEventSentOverTCP)
						{
							int channel;
							sprintf(transmitBuf,"Event ID %d\n",CurrentEvent.EventID);
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
							{
								//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
							}
							for(channel = 0; channel < CurrentEvent.NbOfSAMBlocksInEvent * 2; channel++) transmitChannel(channel,0,-1);
							sprintf(transmitBuf,"Data sent, event %d.\n",CurrentEvent.EventID);


						}
						else
						{
							sprintf(transmitBuf,"No new event %d\n",CurrentEvent.EventID);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
						break;
					case MSG_DELAY:
						posSpace=(int)(strchr(receiveBuf,' ')-receiveBuf);
						short int delay=(short) atoi(&(receiveBuf[posSpace]));
						StopAcquisition = TRUE;
						while(AcquisitionRunning == TRUE);
						errCode = WAVECAT64CH_SetTriggerDelay((unsigned char)delay);
						sprintf(transmitBuf,"Delay changed. %d\n",errCode);
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
						break;
					case MSG_DISPLAY:
						posSpace=(int)(strchr(receiveBuf,' ')-receiveBuf);
						short int display=(short) atoi(&(receiveBuf[posSpace]));
						DisplayON=(display==1);
						sprintf(transmitBuf,"Display %d\n",display);
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_SAMPLING:
						posSpace=(int)(strchr(receiveBuf,' ')-receiveBuf)+1;
						WAVECAT64CH_SamplingFrequencyType frequency=-1;
						if (strncmp(&(receiveBuf[posSpace]),"400",2)==0) frequency=WAVECAT64CH_400MHZ;
						if (strncmp(&(receiveBuf[posSpace]),"533",2)==0) frequency=WAVECAT64CH_533MHZ;
						if (strncmp(&(receiveBuf[posSpace]),"800",2)==0) frequency=WAVECAT64CH_800MHZ;
						if (strncmp(&(receiveBuf[posSpace]),"1.07",2)==0) frequency=WAVECAT64CH_1_07GHZ;
						if (strncmp(&(receiveBuf[posSpace]),"1.28",2)==0) frequency=WAVECAT64CH_1_28GHZ;
						if (strncmp(&(receiveBuf[posSpace]),"1.6",2)==0) frequency=WAVECAT64CH_1_6GHZ;
						if (strncmp(&(receiveBuf[posSpace]),"2.13",2)==0) frequency=WAVECAT64CH_2_13GHZ;
						if (strncmp(&(receiveBuf[posSpace]),"3.2",2)==0) frequency=WAVECAT64CH_3_2GHZ;
						StopAcquisition = TRUE;
						while(AcquisitionRunning == TRUE);
						errCode = WAVECAT64CH_SetSamplingFrequency(frequency);
						sprintf(transmitBuf,"Sampling changed. %d\n",errCode);
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_THRESHOLD:
						int channel=-1;
						float threshold=-1;
						int error=0;
						char *token;
						receiveBuf[dataSize] = ' ';
						receiveBuf[dataSize+1] = '\0';
						token=strtok(receiveBuf," ");
						//first token should be threshold - We skip it...

						// second token should be channel
						token=strtok(NULL," ");
						if (!(strcmp(token,"channel")==0)) error=1;

						// third token should be the channel number
						if (token!=NULL)
						{
							token=strtok(NULL," ");
							channel=atoi(token);
						}
						else
						{
							error=error+16;
						}

						// 4th token should be "value"
						if (token!=NULL)
						{
							token=strtok(NULL," ");
							if (!(strcmp(token,"value")==0)) error=error+2;
						}
						else
						{
							error=error+32;
						}

						// 5th token should be the threshold
						if (token!=NULL)
						{
							token=strtok(NULL," ");
							threshold=atof(token);
						}
						else
						{
							error=error+64;
						}
						if (error==0)
						{
							StopAcquisition = TRUE;
							while(AcquisitionRunning == TRUE);
							int errCode = WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_FRONT_CHANNEL,channel,threshold);
							sprintf(transmitBuf,"Threshold set. %d\n",errCode);
						}
						else
						{
							sprintf(transmitBuf,"Threshold error %d\n",error);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_SAVE:
						short int channels[8];
						for (int iloop=0; iloop<8; iloop++)
						{
							channels[iloop]=0;
						};
						error=0;
						receiveBuf[dataSize] = ' ';
						receiveBuf[dataSize+1] = '\0';
						token=strtok(receiveBuf," ");
						//first token should be save - We skip it...

						// second token should be channel
						token=strtok(NULL," ");
						if (!(strncmp(token,"channel",4)==0)) error=1;

						// next token should be the channel number
						token=strtok(NULL," ");
						while((token!=NULL)&&(!(strncmp(token,"filename",4)==0)))
						{
							int channelON=atoi(token);
							if ((channelON>=1)&&(channelON<=8))
							{
								channels[channelON-1]=1;
							}
							else
							{
								error=error+16;
							}
							token=strtok(NULL," ");
						}

						// next token should be "filename"
						if (token!=NULL)
						{
							if (!(strncmp(token,"filename",4)==0)) error=error+2;
						}
						else
						{
							error=error+32;
						}

						// 5th token should be the filename
						if ((token!=NULL)&&(error==0))
						{
							token=strtok(NULL," ");
							saveChannels(channels,token);
							sprintf(transmitBuf,"Event ID %d\n",CurrentEvent.EventID);
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error reecived.");
							sprintf(transmitBuf,"Channels saved\n");
						}
						else
						{
							sprintf(transmitBuf,"Save error %d\n",error);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_TRIG:
						posSpace=(int)(strchr(receiveBuf,' ')-receiveBuf)+1;
						WAVECAT64CH_TriggerType trigmode=WAVECAT64CH_TRIGGER_NORMAL;
						if (strncmp(&(receiveBuf[posSpace]),"auto",2)==0) trigmode=WAVECAT64CH_TRIGGER_INTERNAL;
						if (strncmp(&(receiveBuf[posSpace]),"intern",2)==0) trigmode=WAVECAT64CH_TRIGGER_INTERNAL;
						if (strncmp(&(receiveBuf[posSpace]),"extern",2)==0) trigmode=WAVECAT64CH_TRIGGER_EXTERNAL;
						if (strncmp(&(receiveBuf[posSpace]),"normal",2)==0) trigmode=WAVECAT64CH_TRIGGER_NORMAL;
						if (strncmp(&(receiveBuf[posSpace]),"soft",2)==0) trigmode=WAVECAT64CH_TRIGGER_SOFT;
						StopAcquisition = TRUE;
						while(AcquisitionRunning == TRUE);
						// WAVECAT64CH_TriggerType triggerBefore;
						//   WAVECAT64CH_GetTriggerMode(&triggerBefore);
						errCode = WAVECAT64CH_SetTriggerMode(trigmode);
						WAVECAT64CH_TriggerType triggerAfter;
						WAVECAT64CH_GetTriggerMode(&triggerAfter);
						sprintf(transmitBuf,"Trigger changed. %d %d %d \n",errCode,(int)trigmode,(int)triggerAfter,&(receiveBuf[posSpace]));
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;


					case MSG_CHAN:
						if (CurrentEvent.EventID!=lastEventSentOverTCP)
						{
							int channel;
							posSpace=(int)(strchr(receiveBuf,' ')-receiveBuf);
							channel=atoi(&(receiveBuf[posSpace]));
							sprintf(transmitBuf,"Event ID %d (%d)\n",CurrentEvent.EventID,channel);
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
							{
								//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
							}
							transmitChannel(channel,0,-1);
							sprintf(transmitBuf,"Data sent, event %d.\n",CurrentEvent.EventID);


						}
						else
						{
							sprintf(transmitBuf,"No new event %d\n",CurrentEvent.EventID);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_RANGE:
						if (CurrentEvent.EventID!=lastEventSentOverTCP)
						{
							int channel=-1;
							int firstSample=-1;
							int lastSample=-1;
							error=0;
							receiveBuf[dataSize] = ' ';
							receiveBuf[dataSize+1] = '\0';
							token=strtok(receiveBuf," ");
							//first token should be range - We skip it...

							// second token should be channel
							token=strtok(NULL," ");
							if (!(strcmp(token,"channel")==0)) error=1;

							// third token should be the channel number
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								channel=atoi(token);
							}
							else
							{
								error=error+16;
							}

							// 4th token should be "from"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"from")==0)) error=error+2;
							}
							else
							{
								error=error+32;
							}

							// 5th token should be the firstSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								firstSample=atoi(token);
							}
							else
							{
								error=error+64;
							}

							// 6th token should be "to"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"to")==0)) error=error+4;
							}
							else
							{
								error=error+128;
							}

							// 7th token should be the lastSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								lastSample=atoi(token);
							}
							else
							{
								error=error+256;
							}

							if (error==0)
							{
								sprintf(transmitBuf,"Event ID %d (%d,%d:%d)\n",CurrentEvent.EventID,channel,firstSample,lastSample);
							}
							else
							{
								sprintf(transmitBuf,"Range error %d\n",error);
							}
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
							{
								//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
							}
							if (error==0)
							{
								transmitChannel(channel,firstSample,lastSample);
								sprintf(transmitBuf,"Data sent, event %d.\n",CurrentEvent.EventID);
							}


						}
						else
						{
							sprintf(transmitBuf,"No new event %d\n",CurrentEvent.EventID);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_INT:
						if (CurrentEvent.EventID!=lastEventSentOverTCP)
						{
							int channel=-1;
							int firstSample=-1;
							int lastSample=-1;
							int error=0;
							char *token;
							receiveBuf[dataSize] = ' ';
							receiveBuf[dataSize+1] = '\0';
							token=strtok(receiveBuf," ");
							//first token should be integrate - We skip it...

							// second token should be channel
							token=strtok(NULL," ");
							if (!(strcmp(token,"channel")==0)) error=1;

							// third token should be the channel number
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								channel=atoi(token);
							}
							else
							{
								error=error+16;
							}

							// 4th token should be "from"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"from")==0)) error=error+2;
							}
							else
							{
								error=error+32;
							}

							// 5th token should be the firstSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								firstSample=atoi(token);
							}
							else
							{
								error=error+64;
							}

							// 6th token should be "to"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"to")==0)) error=error+4;
							}
							else
							{
								error=error+128;
							}

							// 7th token should be the lastSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								lastSample=atoi(token);
							}
							else
							{
								error=error+256;
							}

							if (error==0)
							{
								sprintf(transmitBuf,"Event ID %d (%d,%d:%d)\n",CurrentEvent.EventID,channel,firstSample,lastSample);
							}
							else
							{
								sprintf(transmitBuf,"Range error %d\n",error);
							}
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
							{
								//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
							}
							if (error==0)
							{
								long int sum=0;
								int channelDataSize=CurrentEvent.ChannelData[channel].WaveformDataSize;
								if ((lastSample==-1)||(lastSample>channelDataSize))
								{
									lastSample=channelDataSize;
								}
								for (int idata=firstSample; idata < lastSample; idata++)
								{
									sum=sum+CurrentEvent.ChannelData[channel].WaveformData[idata];
								} // for each data event
								sprintf(transmitBuf,"Sum (%d,%d:%d) %d\n",channel,firstSample,lastSample,sum);
							}
							else
							{
								sprintf(transmitBuf,"Range error %d\n",error);
							}


						}
						else
						{
							sprintf(transmitBuf,"No new event %d\n",CurrentEvent.EventID);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_SUBSTRACT:
						if (CurrentEvent.EventID!=lastEventSentOverTCP)
						{
							int channel=-1;
							int firstSample=-1;
							int lastSample=-1;
							int firstSampleMinus=-1;
							int lastSampleMinus=-1;
							int error=0;
							char *token;
							receiveBuf[dataSize] = ' ';
							receiveBuf[dataSize+1] = '\0';
							token=strtok(receiveBuf," ");
							//first token should be substract - We skip it...

							// second token should be channel
							token=strtok(NULL," ");
							if (!(strcmp(token,"channel")==0)) error=1;

							// third token should be the channel number
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								channel=atoi(token);
							}
							else
							{
								error=error+16;
							}

							// 4th token should be "from"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"from")==0)) error=error+2;
							}
							else
							{
								error=error+32;
							}

							// 5th token should be the firstSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								firstSample=atoi(token);
							}
							else
							{
								error=error+64;
							}

							// 6th token should be "to"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"to")==0)) error=error+4;
							}
							else
							{
								error=error+128;
							}

							// 7th token should be the lastSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								lastSample=atoi(token);
							}
							else
							{
								error=error+256;
							}

							// 8th token should be "minus"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"minus")==0)) error=error+512;
							}
							else
							{
								error=error+1024;
							}

							// 9th token should be the firstSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								firstSampleMinus=atoi(token);
							}
							else
							{
								error=error+2048;
							}

							// 10th token should be "to"
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								if (!(strcmp(token,"to")==0)) error=error+4096;
							}
							else
							{
								error=error+8192;
							}

							// 11th token should be the lastSample
							if (token!=NULL)
							{
								token=strtok(NULL," ");
								lastSampleMinus=atoi(token);
							}
							else
							{
								error=error+16384;
							}

							if (error==0)
							{
								sprintf(transmitBuf,"Event ID %d (%d,%d:%d-%d:%d)\n",CurrentEvent.EventID,channel,firstSample,lastSample,firstSampleMinus,lastSampleMinus);
							}
							else
							{
								sprintf(transmitBuf,"Range error %d\n",error);
							}
							if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0)
							{
								//SetCtrlVal (panelHandle, MAINPNL_TRANSMIT,"Transmit Error\n");
							}
							if (error==0)
							{
								long int sum=0;
								long int sumMinus=0;
								float substraction=0;
								int channelDataSize=CurrentEvent.ChannelData[channel].WaveformDataSize;
								if ((lastSample==-1)||(lastSample>channelDataSize))
								{
									lastSample=channelDataSize;
								}
								for (int idata=firstSample; idata < lastSample; idata++)
								{
									sum=sum+CurrentEvent.ChannelData[channel].WaveformData[idata];
								} // for each data event
								for (int idata=firstSampleMinus; idata < lastSampleMinus; idata++)
								{
									sumMinus=sumMinus+CurrentEvent.ChannelData[channel].WaveformData[idata];
								} // for each data event
								substraction=((float)sum/(lastSample-firstSample))-(((float)sumMinus)/(lastSampleMinus-firstSampleMinus));
								sum=sum-sumMinus;
								sprintf(transmitBuf,"Sum (%d,%d:%d-%d:%d) %5.3f\n",channel,firstSample,lastSample,firstSampleMinus,lastSampleMinus,substraction);
							}
							else
							{
								sprintf(transmitBuf,"Range error %d\n",error);
							}


						}
						else
						{
							sprintf(transmitBuf,"No new event %d\n",CurrentEvent.EventID);
						}
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received.");
							break;
					case MSG_EVT:
					{
						//MessagePopup ("NOT Understood",receiveBuf);
						sprintf(transmitBuf,"Event ID %d\n",CurrentEvent.EventID);
						//if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error reecived.");
					}
					break;
					case MSG_QUIT:
					{
						sprintf(transmitBuf,"Bye\n");
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received MSG QUIT.");
						tcpChk (DisconnectTCPClient (handle));
					}
					break;
					case MSG_UNKNOWN:
					default:
					{
						//MessagePopup ("NOT Understood",receiveBuf);
						sprintf(transmitBuf,"Not understood %d\nCommand: >%s<\n",msg_value,receiveBuf);
						if (ServerTCPWrite (g_hconversation, transmitBuf, strlen (transmitBuf), 1000) < 0) MessagePopup("Transmission error!","Transmission error received MSG UNKNOWN.");
					}
				}
				break;

			}
			break;
		case TCP_DISCONNECT:
			if (handle == g_hconversation)
			{
				/* The client we were talking to has disconnected... */
				SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0);
				g_hconversation = 0;
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_IP, "");
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_NAME, "");
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE,
							"-- Client disconnected --\n");
				SetCtrlAttribute (g_hmainPanel, MAINPNL_STRING, ATTR_DIMMED,
								  1);

				/* Note that we do not need to do any more because we set the*/
				/* disconnect mode to AUTO. */
			}
			break;
	}

Done:
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Respond to the UI and clear the receive screen for the user.              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ClearScreenCB (int panel, int control, int event,
							   void *callbackData, int eventData1,
							   int eventData2)
{
	if (event == EVENT_COMMIT)
		ResetTextBox (panel, MAINPNL_RECEIVE, "");
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Respond to the panel closure to quit the UI loop.                         */
/*---------------------------------------------------------------------------*/
int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		QuitUserInterface (0);
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Report TCP Errors if any                         						 */
/*---------------------------------------------------------------------------*/
static void ReportTCPError (void)
{
	if (g_TCPError < 0)
	{
		char	messageBuffer[1024];
		sprintf(messageBuffer,
				"TCP library error message: %s\nSystem error message: %s",
				GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
		MessagePopup ("Error", messageBuffer);
	}
}
