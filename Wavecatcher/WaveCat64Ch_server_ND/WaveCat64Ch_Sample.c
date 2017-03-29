 /***************************************************************
 *	File	: 	WaveCat64ch_Sample.c						    *
 *															    *
 *	Date	:	July 2014   								    *
 *															    *
 *	Authors	:	Dominique Breton	LAL Orsay					*
 *	            Jihane    Maalmi	LAL Orsay					*
 *															    *
 ***************************************************************/

#include <ansi_c.h>
#include "windows.h" 
#include <analysis.h>
#include "WaveCat64Ch_Lib.h"
#include "WaveCat64Ch_Sample.h"
#include "WaveCat64Ch_RatePanel.h"
#include "server.h"
 

#include <utility.h>
#include <cvirte.h>		
#include <userint.h>

/* ============================================================================ */ 
/* ============================ Constantes ==================================== */
/* ============================================================================ */ 

#define TRUE 1
#define FALSE 0

#define MAX_NB_OF_CHANNELS 	72
#define NB_OF_CHANNELS_IN_FE_FPGA 4
#define MAX_NB_OF_FE_FPGAS_IN_SYSTEM 16
#define MIN_DAC_RAW_VALUE	-1.25
#define MAX_DAC_RAW_VALUE	1.25

#define	 WAVECAT64CH_ADCTOVOLTS			 0.00061
#define  VOLTSTOADC			 1638

#define SOFTWARE_VERSION "V1.1.2"

#define MAX_NB_OF_CHANNELS_TO_PLOT 16

typedef int Boolean;


//* ============================================================================= */ 
//* ============================   GLOBAL VARIABLES ============================= */
//* ============================================================================= */

/* ==================  local global variables ================== */		


static int MainPanelHandle;
static int RateStatisticsPanelHandle;

int DeviceHandle = -1;
WAVECAT64CH_DeviceInfoStruct DeviceInfo; 
StopAcquisition = TRUE;
int NbOfChannels = 0;

int VerticalScale  = 500; //mV/Div
AcquisitionRunning = FALSE;

float DCOffset[MAX_NB_OF_CHANNELS];
float TriggerThreshold[MAX_NB_OF_CHANNELS];
Boolean TriggerPolarity[MAX_NB_OF_CHANNELS];
unsigned char TriggerDelay;

WAVECAT64CH_SamplingFrequencyType SamplingFrequency;

WAVECAT64CH_TriggerType TriggerType;

int ChannelTriggerEnable[MAX_NB_OF_CHANNELS];

WAVECAT64CH_TriggerEdgeType TriggerEdge[MAX_NB_OF_CHANNELS];

unsigned short PulsePattern[MAX_NB_OF_CHANNELS];
int EnablePulseChannels[MAX_NB_OF_CHANNELS];

unsigned int *ReadoutBuffer; 
int ReadoutBufferSize;

DisplayON = TRUE;
Boolean EnableRateStats = FALSE;

double CurrentTimer =0;
double FormerTimer =0;
int FormerEventNumber = 0;

int FirstChannelToDisplay = 0;

int ChannelInCoincidenceForRateHandle[MAX_NB_OF_FE_FPGAS_IN_SYSTEM]={
				PANEL_RATE_COINCBOX0_0,PANEL_RATE_COINCBOX0_1,PANEL_RATE_COINCBOX0_2,PANEL_RATE_COINCBOX0_3,
				PANEL_RATE_COINCBOX1_0,PANEL_RATE_COINCBOX1_1,PANEL_RATE_COINCBOX1_2,PANEL_RATE_COINCBOX1_3,
				PANEL_RATE_COINCBOX2_0,PANEL_RATE_COINCBOX2_1,PANEL_RATE_COINCBOX2_2,PANEL_RATE_COINCBOX2_3,
				PANEL_RATE_COINCBOX3_0,PANEL_RATE_COINCBOX3_1,PANEL_RATE_COINCBOX3_2,PANEL_RATE_COINCBOX3_3}; 

int LabelInCoincidenceForRateHandle[NB_OF_CHANNELS_IN_FE_FPGA]={
				PANEL_RATE_TEXTMSG_41,PANEL_RATE_TEXTMSG_42,PANEL_RATE_TEXTMSG_43,PANEL_RATE_TEXTMSG_44};

unsigned short CoincidenceMask[MAX_NB_OF_FE_FPGAS_IN_SYSTEM];
int ChannelForRateStats;
int ChannelForRatePlot;
float PlotIntervalForRateStripchart;
float ChannelHitRate[MAX_NB_OF_CHANNELS];
Boolean RateRunStopRequested = FALSE;
					

/* ============================================================================ */ 
/* ============================== Main ======================================= */ 
/* ============================================================================ */ 

int main (int argc, char *argv[])
{
	int errCode;
	char title[120];
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	//Start the TCP server
	Start_server();
	if ((MainPanelHandle = LoadPanel (0, "WaveCat64Ch_Sample.uir", MAINPANEL)) < 0)
		return -1;
	if ((RateStatisticsPanelHandle = LoadPanel (0, "WaveCat64Ch_RatePanel.uir", PANEL_RATE)) < 0)
		return -1; 
	
	
	
    errCode = WAVECAT64CH_OpenDevice(&DeviceHandle); 
	
/*		MeasurementParams.ForceBaseline = FALSE;	
		MeasurementParams.EnforcedBaselineValue = 0.0;
		MeasurementParams.FixedThresholdOrCFD = CFD_MODE;
		MeasurementParams.FixedThresholdValue = 0.0;
		MeasurementParams.PeakPolarity = NEGATIVE;
		MeasurementParams.PeakRatio = 8;	   // Entre 1 et 16 (en seizièmes)
		MeasurementParams.EnableDynamicCharge = FALSE;
		MeasurementParams.PreCharge = 32;	   // Entre 1 et 99 (en samples)
		MeasurementParams.RefCellForCharge = 800;  // En samples
		MeasurementParams.ChargeLength = 8; // Entre 4 et 63 (en colonnes => multiplier par 16 pour samples)
		WAVECAT64CH_SetFirmwareMeasurementParameters(MeasurementParams);   */
	
	WAVECAT64CH_GetDeviceInfo(&DeviceInfo);                       
	
	if(errCode == WAVECAT64CH_Success)
	{  
	
		
		errCode = WAVECAT64CH_ResetDevice();
		
		Init_LocalVariables();
	    
		Update_Panel();
		
		sprintf(title, "WAVECAT_64CH SAMPLE %s", SOFTWARE_VERSION);
		SetPanelAttribute(MainPanelHandle, ATTR_TITLE, title);
		SetPanelAttribute(MainPanelHandle, ATTR_FLOATING, VAL_FLOAT_APP_ACTIVE);
		
		DisplayPanel(MainPanelHandle);
	
		ClearStripChart(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART);
		
		errCode = WAVECAT64CH_StopRun();
		
		RunUserInterface();
		
// -----------------------------------------------------------------
// On quitte l'appli ...
		
		Stop_run();
	    errCode = WAVECAT64CH_CloseDevice();
	}
	else
	{
		MessagePopup("Error!", "No WaveCatcher system found! \n");
	}
	
	Stop_server(); // stop the TCP server
	DiscardPanel(MainPanelHandle);
	DiscardPanel(RateStatisticsPanelHandle);
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
	return 0;
}

/* ============================================================================ */ 
/* ================== Start_Acquisition Callback ============================== */
/* ============================================================================ */ 
 
int Start_Acquisition_TCP()
{
	if(AcquisitionRunning == TRUE)
					 {  
						 StopAcquisition = TRUE;
						 while(AcquisitionRunning);
					 }
			   
					CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, Start_Acquisition, ThreadData, &ThreadID);
	return 0;
}
/* ============================================================================ */ 
static int CVICALLBACK Start_Acquisition (void* unused)
/* ============================================================================ */ 
{
int channel, channelToPlot;
double yOffset;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success;
double waveformArray[WAVECAT64CH_MAX_DATA_SIZE];
double mean, rms;
int i;

	StopAcquisition = FALSE;
	
	EventNumber = 0; 
	SetCtrlVal(MainPanelHandle, MAINPANEL_EVENTNUMBER, EventNumber);
	SetCtrlAttribute (MainPanelHandle, MAINPANEL_GRAPH, ATTR_REFRESH_GRAPH, 0);
	SetCtrlAttribute(MainPanelHandle, MAINPANEL_START, ATTR_DIMMED, 1);
	
	Start_run();
	
	AcquisitionRunning = TRUE;
	
	//SetAxisScalingMode (MainPanelHandle, MAINPANEL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, -5, 5);
	SetAxisScalingMode (MainPanelHandle, MAINPANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 1024);

	for(;;)
	{
		Prepare_Event();
	    errCode = -1;
		
		for(;;)
		{
			if(StopAcquisition == TRUE)
				break;
			
			errCode = WAVECAT64CH_ReadEventBuffer();
			if((errCode == WAVECAT64CH_Success) || (errCode == WAVECAT64CH_ReadoutError))
				break;
			
			if((errCode == WAVECAT64CH_CommError) || (errCode == WAVECAT64CH_ExtendedReadoutError))
			{
				printf("Error while reading event!\n");
				break;	
			}
			Delay(0.001);
		}
			
		if(errCode  < 0) // erreur
		{   
		 	StopAcquisition = TRUE; 
			printf("Error while reading board: errCode = %d\n", errCode);
			break;
		}
			
		errCode = WAVECAT64CH_DecodeEvent(&CurrentEvent);
		EventNumber++;
		
		if(errCode < 0)
		{
			printf("Error while decoding event!\n");
			break;
		}
	
		if(DisplayON == TRUE)
		{
			SetCtrlVal(MainPanelHandle, MAINPANEL_EVENTNUMBER, EventNumber);  
			SetCtrlVal(MainPanelHandle, MAINPANEL_EVENTID, CurrentEvent.EventID);  

			DeleteGraphPlot (MainPanelHandle, MAINPANEL_GRAPH, -1, VAL_DELAYED_DRAW);
		
			for(channel = 0; channel < CurrentEvent.NbOfSAMBlocksInEvent * 2; channel++)
			{
				if(channel >= MAX_NB_OF_CHANNELS_TO_PLOT) break;
				
				channelToPlot = FirstChannelToDisplay + channel;
				
				if(channelToPlot >= NbOfChannels) break; 
				
				GetAxisItem (MainPanelHandle, MAINPANEL_GRAPH, VAL_LEFT_YAXIS, channel, NULL, &yOffset);
			
				PlotWaveform (MainPanelHandle, MAINPANEL_GRAPH, CurrentEvent.ChannelData[channelToPlot].WaveformData, CurrentEvent.ChannelData[channelToPlot].WaveformDataSize, VAL_FLOAT,
								  (double)1000*WAVECAT64CH_ADCTOVOLTS/VerticalScale, yOffset, 0, 1, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, Set_Color(channel));
				
/*				for(i = 0; i<CurrentEvent.ChannelData[channelToPlot].WaveformDataSize; i++)
					waveformArray[i]=(double)(CurrentEvent.ChannelData[channelToPlot].WaveformData[i]*WAVECAT64CH_ADCTOVOLTS*1000);
				
				StdDev (waveformArray, CurrentEvent.ChannelData[channelToPlot].WaveformDataSize, &mean, &rms);
				printf("ch%d mean= %f mV rms =%f mV \n", channel, mean , rms);
*/			
			}
			
		    Delay(0.3);
			
			RefreshGraph (MainPanelHandle,MAINPANEL_GRAPH);
		
			//SetCtrlAttribute (MainPanelHandle, MAINPANEL_GRAPH, ATTR_REFRESH_GRAPH, 1);
		}
		else
		{
			if(EventNumber%100 == 0)
			 SetCtrlVal(MainPanelHandle, MAINPANEL_EVENTNUMBER, EventNumber);  
		}
		
//		printf("Falling = %f, peak = %d, rising = %f\n", CurrentEvent.ChannelData[0].CFDFallingEdgeTime, CurrentEvent.ChannelData[0].PeakCell, CurrentEvent.ChannelData[0].CFDRisingEdgeTime); 
		
	  	if(StopAcquisition == TRUE)
	     	break;
	}

	Stop_run();
	
	AcquisitionRunning = FALSE; 
	
	SetCtrlAttribute(MainPanelHandle, MAINPANEL_START, ATTR_DIMMED, 0); 
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
	return 0;
}

/* ============================================================================ */ 
/* ===================== Main Panel Callback   ================================= */
/* ============================================================================ */ 
 				    
/* ============================================================================ */ 
int CVICALLBACK MainpanelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
/* ============================================================================ */ 
{
int err;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success;

 
	switch (event)
	{
		case EVENT_CLOSE:
			
			errCode = WAVECAT64CH_CloseDevice();
			QuitUserInterface (0);
		if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   		
		break;
	}
	
	    
	return 0;
}

/* ============================================================================ */ 
/* ====================== Callbacks inside main panel ========================= */
/* ============================================================================ */ 

/* ============================================================================ */ 
int CVICALLBACK HorizontalCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
int intVal;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success;

	switch (event)
	{
		case EVENT_COMMIT:
			
			if(AcquisitionRunning == TRUE)
			 {  
				 StopAcquisition = TRUE;
				 while(AcquisitionRunning);
			 }

			switch(control)
			{
				
				case MAINPANEL_DELAY :
					
					GetCtrlVal(MainPanelHandle,MAINPANEL_DELAY, &TriggerDelay); 
					errCode = WAVECAT64CH_SetTriggerDelay(TriggerDelay);
					 
					break;
					
				case MAINPANEL_HORIZONTALSCALE:
					
					GetCtrlVal(MainPanelHandle,MAINPANEL_HORIZONTALSCALE, &intVal); 
					SamplingFrequency = (WAVECAT64CH_SamplingFrequencyType)intVal;
					errCode = WAVECAT64CH_SetSamplingFrequency(SamplingFrequency);
					
					break;
			}
		  
			
		if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");             
		break;
	}
	
	return 0;
}

/* ============================================================================ */ 
int CVICALLBACK VerticalCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
float offset;
int selCh, channel;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 

	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case MAINPANEL_SELCH_FOR_OFFSET :
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_OFFSET, &selCh); 
					SetCtrlVal(MainPanelHandle, MAINPANEL_OFFSET, DCOffset[selCh]); 
		
					break;
					
				case MAINPANEL_OFFSET:
					
					 if(AcquisitionRunning == TRUE)
		  			 {  
						 StopAcquisition = TRUE;
						 while(AcquisitionRunning);
					 }
  					
					GetCtrlVal(MainPanelHandle, MAINPANEL_OFFSET, &offset); 
				    GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_OFFSET, &selCh);         
	
					DCOffset[selCh] = offset;
					
					if(selCh < DeviceInfo.NbOfFeChannels)
	 					errCode = WAVECAT64CH_SetChannelDCOffset(WAVECAT64CH_FRONT_CHANNEL, selCh, DCOffset[selCh]);
					else
	 					errCode = WAVECAT64CH_SetChannelDCOffset(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, DCOffset[selCh]);
		
					break;
			  
			   case MAINPANEL_APPLYTOALL_DCOFFSET:
				
					 if(AcquisitionRunning == TRUE)
		  			 {  
						 StopAcquisition = TRUE;
						 while(AcquisitionRunning);
					 }
  					
				   GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_OFFSET, &selCh);         
	
				   for(channel = 0; channel < NbOfChannels; channel++)
				   {
					  	DCOffset[channel] =  DCOffset[selCh];
						if(channel < DeviceInfo.NbOfFeChannels)
		 					errCode = WAVECAT64CH_SetChannelDCOffset(WAVECAT64CH_FRONT_CHANNEL, channel, DCOffset[channel]);
						else
		 					errCode = WAVECAT64CH_SetChannelDCOffset(WAVECAT64CH_BACK_EXTRA_CHANNEL, channel - DeviceInfo.NbOfFeChannels, DCOffset[channel]);
				   }
				
				break;
					  
			  case MAINPANEL_VERTICALSCALE :
				  
				  GetCtrlVal(MainPanelHandle, MAINPANEL_VERTICALSCALE, &VerticalScale);
				  
				  break;
			}
		    if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   
			break;
	}
	
	
	return 0;
}

/* ============================================================================ */ 
int CVICALLBACK SAMCorrectionsCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
Boolean pedestalCorrect, inlCorrect;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEPEDCORRECTION, &pedestalCorrect);
			
			GetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEINLCORRECTION, &inlCorrect);
			
			if(pedestalCorrect == FALSE)
			{
				if(inlCorrect == TRUE)
					SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEINLCORRECTION, FALSE);
				
				errCode = WAVECAT64CH_SetCorrectionMode(WAVECAT64CH_NO_CORRECTION); 
			}
			else
			{
				if(inlCorrect == FALSE)
					errCode = WAVECAT64CH_SetCorrectionMode(WAVECAT64CH_PEDESTAL_CORRECTION_ONLY); 
			
				else
					errCode = WAVECAT64CH_SetCorrectionMode(WAVECAT64CH_ALL_CORRECTIONS);
			}
			if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   
			break;
	}
	
	return 0;
}

/* ============================================================================ */ 
int CVICALLBACK DisplayCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
char channelNumber[8];
int channel;
float yPosition;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case MAINPANEL_DISPLAYON :
					
				GetCtrlVal(MainPanelHandle, MAINPANEL_DISPLAYON, &DisplayON);

				break;
			
				case MAINPANEL_SELCH_FOR_DISPLAY :
					
				GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_DISPLAY, &FirstChannelToDisplay);
				
				for(channel = 0; channel < MAX_NB_OF_CHANNELS_TO_PLOT; channel ++)
				{
					sprintf(channelNumber, "Ch%d", FirstChannelToDisplay + channel);
					yPosition = -5.0 + channel * 10.0 / (MAX_NB_OF_CHANNELS_TO_PLOT - 1);
					ReplaceAxisItem(MainPanelHandle, MAINPANEL_GRAPH, VAL_LEFT_YAXIS, channel, channelNumber, yPosition);
				}
				
				break;
				
				case  MAINPANEL_OPENRATEPANEL :
			
				DisplayPanel(RateStatisticsPanelHandle);

				break;
			}
	
	
		
	    break;
	}
	return 0;
}


/* ============================================================================ */ 
int CVICALLBACK TriggerCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
int selCh,channel, intVal;
int triggerMode, triggerPolarity;
Boolean boolVal;
float threshold;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 	
	 
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(AcquisitionRunning == TRUE)
			 {  
				 StopAcquisition = TRUE;
				 while(AcquisitionRunning);
			 }
			
			switch(control)
			{
				case MAINPANEL_TRIGGERTYPE :
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_TRIGGERTYPE, &intVal);
					TriggerType = (WAVECAT64CH_TriggerType)intVal;
					errCode = WAVECAT64CH_SetTriggerMode(TriggerType);
					
					break;
					
				case MAINPANEL_THRESHOLD:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_THRESHOLD, &threshold);
					GetCtrlVal(MainPanelHandle,MAINPANEL_SELCH_FOR_TRIG, &selCh);
					TriggerThreshold[selCh] = threshold;
					if(selCh < DeviceInfo.NbOfFeChannels)
							errCode = WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_FRONT_CHANNEL, selCh, TriggerThreshold[selCh]); 
					else
							errCode = WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, TriggerThreshold[selCh]); 
					
					break;
					
				case MAINPANEL_APPLYTOALL_THRESHOLD:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_THRESHOLD, &threshold); 
					
					for(channel = 0; channel < NbOfChannels; channel++)
					{	
						TriggerThreshold[channel] =  threshold;
						if(channel < DeviceInfo.NbOfFeChannels)
								errCode = WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_FRONT_CHANNEL, channel, TriggerThreshold[channel]); 
						else
								errCode = WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_BACK_EXTRA_CHANNEL, channel - DeviceInfo.NbOfFeChannels, TriggerThreshold[channel]); 
					}
					break;
					
				case MAINPANEL_SELCH_FOR_TRIG:
					
					GetCtrlVal(MainPanelHandle,MAINPANEL_SELCH_FOR_TRIG, &selCh);
					SetCtrlVal(MainPanelHandle, MAINPANEL_ENTRIGCH, ChannelTriggerEnable[selCh]);   
					SetCtrlVal(MainPanelHandle, MAINPANEL_TRIGEDGE, TriggerPolarity[selCh]);   
					SetCtrlVal(MainPanelHandle, MAINPANEL_THRESHOLD, TriggerThreshold[selCh]);   
					
				 break;
					
				case MAINPANEL_ENTRIGCH:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_ENTRIGCH, &boolVal);
					GetCtrlVal(MainPanelHandle,MAINPANEL_SELCH_FOR_TRIG, &selCh);
					ChannelTriggerEnable[selCh] = (WAVECAT64CH_StateType)boolVal;
					if(selCh < DeviceInfo.NbOfFeChannels)
							errCode = WAVECAT64CH_SetTriggerSourceState(WAVECAT64CH_FRONT_CHANNEL, selCh, ChannelTriggerEnable[selCh]);
					else
							errCode = WAVECAT64CH_SetTriggerSourceState(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, ChannelTriggerEnable[selCh]);
					
					break;
					
				case MAINPANEL_APPLYTOALL_ENTRIGCH:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_ENTRIGCH, &boolVal); 
					
					for(channel = 0; channel < NbOfChannels; channel++)
					{	
						ChannelTriggerEnable[channel] =  boolVal;
						if(channel < DeviceInfo.NbOfFeChannels)
								errCode = WAVECAT64CH_SetTriggerSourceState(WAVECAT64CH_FRONT_CHANNEL, channel, ChannelTriggerEnable[channel]); 
						else
								errCode = WAVECAT64CH_SetTriggerSourceState(WAVECAT64CH_BACK_EXTRA_CHANNEL, channel - DeviceInfo.NbOfFeChannels, ChannelTriggerEnable[channel]); 
					}
					break;
					
				case MAINPANEL_TRIGEDGE:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_TRIGEDGE, &boolVal);
					GetCtrlVal(MainPanelHandle,MAINPANEL_SELCH_FOR_TRIG, &selCh);
					TriggerPolarity[selCh] = (WAVECAT64CH_StateType)boolVal;
					if(selCh < DeviceInfo.NbOfFeChannels)
							errCode = WAVECAT64CH_SetTriggerEdge(WAVECAT64CH_FRONT_CHANNEL, selCh, TriggerPolarity[selCh]);
					else
							errCode = WAVECAT64CH_SetTriggerEdge(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, TriggerPolarity[selCh]);
					
					break;
					
				case MAINPANEL_APPLYTOALL_TRIGEDGE:
					
					GetCtrlVal(MainPanelHandle, MAINPANEL_TRIGEDGE, &boolVal); 
					
					for(channel = 0; channel < NbOfChannels; channel++)
					{	
						TriggerPolarity[channel] =  boolVal;
						if(channel < DeviceInfo.NbOfFeChannels)
							errCode =	WAVECAT64CH_SetTriggerEdge(WAVECAT64CH_FRONT_CHANNEL, channel, TriggerPolarity[channel]); 
						else
							errCode =	WAVECAT64CH_SetTriggerEdge(WAVECAT64CH_BACK_EXTRA_CHANNEL, channel - DeviceInfo.NbOfFeChannels, TriggerPolarity[channel]); 
					}
					break;
			}
		    if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   
			break;
	}

	
	return 0;
}

/* ============================================================================ */ 
int CVICALLBACK PulserCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
int enablePulsers, selCh, channel;
unsigned short pulsePattern;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 

	switch (event)
	{
		case EVENT_COMMIT:
			
		if(AcquisitionRunning == TRUE)
		 {  
			 StopAcquisition = TRUE;
			 while(AcquisitionRunning);
		 }  
	 
		switch(control)
		{
			case MAINPANEL_ENABLEPULSERS:
				
				GetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEPULSERS, &enablePulsers);
				
				if(enablePulsers == TRUE) 
					errCode =	WAVECAT64CH_EnablePulsers();
				else
					errCode =	WAVECAT64CH_DisablePulsers();
				
				break;
			
			case MAINPANEL_SELCH_FOR_PATTERN:
				
				GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_PATTERN, &selCh);  
				if(selCh < DeviceInfo.NbOfFeChannels)
					errCode = WAVECAT64CH_GetPulsePattern(WAVECAT64CH_FRONT_CHANNEL, selCh, &pulsePattern);
				else
					WAVECAT64CH_GetPulsePattern(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, &pulsePattern);

				SetCtrlVal(MainPanelHandle, MAINPANEL_PULSEPATTERN, pulsePattern);         
				
				break;
			
			case MAINPANEL_PULSEPATTERN:
				
				GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_PATTERN, &selCh);         
				GetCtrlVal(MainPanelHandle, MAINPANEL_PULSEPATTERN, &PulsePattern[selCh]);         

				if(selCh < DeviceInfo.NbOfFeChannels)
					errCode = WAVECAT64CH_SetPulsePattern(WAVECAT64CH_FRONT_CHANNEL, selCh, PulsePattern[selCh]); 
				else
					errCode = WAVECAT64CH_SetPulsePattern(WAVECAT64CH_BACK_EXTRA_CHANNEL, selCh - DeviceInfo.NbOfFeChannels, PulsePattern[selCh]); 
			
				break;

			case MAINPANEL_APPLYTOALL_PATTERN:
					
				GetCtrlVal(MainPanelHandle, MAINPANEL_PULSEPATTERN, &pulsePattern); 
				
				for(channel = 0; channel < NbOfChannels; channel++)
				{	
					PulsePattern[channel] = pulsePattern;
					if(channel < DeviceInfo.NbOfFeChannels)
						errCode = WAVECAT64CH_SetPulsePattern(WAVECAT64CH_FRONT_CHANNEL, channel, PulsePattern[channel]); 
					else
						errCode = WAVECAT64CH_SetPulsePattern(WAVECAT64CH_BACK_EXTRA_CHANNEL, channel - DeviceInfo.NbOfFeChannels, PulsePattern[channel]); 
				}
				
				break;
		}
		
		
		if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   
		break;
		
	}
	
	return 0;
}

/* ============================================================================ */ 
int CVICALLBACK RunCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case MAINPANEL_START:
					
					if(AcquisitionRunning == TRUE)
					 {  
						 StopAcquisition = TRUE;
						 while(AcquisitionRunning);
					 }
			   
					CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, Start_Acquisition, ThreadData, &ThreadID);

					break;
					
				case MAINPANEL_STOP:
					
					StopAcquisition = TRUE;
					while(AcquisitionRunning == TRUE);
					
					break;
			}

			break;
	}

	return 0;
}

/* ============================================================================ */ 
/* ===================== Rate Panel Callback  ================================= */
/* ============================================================================ */ 
 				    
/* ============================================================================ */ 
int CVICALLBACK PanelRateStatCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
/* ============================================================================ */ 
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(RateStatisticsPanelHandle);
			break;
	}
	return 0;
}

/* ============================================================================ */ 
/* ====================== Callbacks inside main panel ========================= */
/* ============================================================================ */ 

/* =========================================================================== */							
int CVICALLBACK RateStatCB(int panel, int control, int event, void *callbackData, 
		int eventData1, int eventData2)
/* =========================================================================== */							
{
int intVal, i, j, fourChannelGroup, channel, channelInGroup; 
unsigned char coincidenceMask;
float floatVal;
Boolean boolVal;
char label[16], xAxisTitle[128];
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 
	switch (event)
	{
	 	case EVENT_COMMIT:

			if(AcquisitionRunning == TRUE)
			 {  
				 StopAcquisition = TRUE;
				 while(AcquisitionRunning);
			 }

			switch(control) 
			 {
				 case PANEL_RATE_ENABLERATESTAT:
					 
					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_ENABLERATESTAT, &boolVal);
					 EnableRateStats = boolVal;
					 if(boolVal == TRUE)
					 {
						 ClearStripChart(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART);
					 	 errCode = WAVECAT64CH_EnableRateCounters();
						 CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, Run_WithRateCounters, RunRateThreadData, &RunRateThreadID);
						 RateRunStopRequested = FALSE;
						 CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, RunRateThreadID);
					 }
					 else
					 {
					 	 RateRunStopRequested = TRUE;
					 }

				  break;
				  
				case PANEL_RATE_SELFEFPGAFORCOINC:
					 
					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_SELFEFPGAFORCOINC, &fourChannelGroup);
					 CoincidenceMask[fourChannelGroup] = 0;
					 
					 for(i = 0; i < NB_OF_CHANNELS_IN_FE_FPGA; i++)
					 {
						 sprintf(label, "%d", fourChannelGroup * NB_OF_CHANNELS_IN_FE_FPGA + i);
						 SetCtrlVal (RateStatisticsPanelHandle, LabelInCoincidenceForRateHandle[i], label);
						 sprintf(label, "Ch%d", fourChannelGroup * NB_OF_CHANNELS_IN_FE_FPGA + i);
						 SetCtrlAttribute (RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i], ATTR_LABEL_TEXT, label);
					 
				  		 errCode = WAVECAT64CH_GetRateCounterCoincidenceMask(fourChannelGroup, i, &coincidenceMask);
						 CoincidenceMask[fourChannelGroup] += (coincidenceMask << (i * NB_OF_CHANNELS_IN_FE_FPGA));
						 
						 for(j = 0; j < NB_OF_CHANNELS_IN_FE_FPGA; j++)
						 {
							 boolVal = (coincidenceMask & (0x1 << j)) >> j;

						 	 SetCtrlVal(RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i * NB_OF_CHANNELS_IN_FE_FPGA + j], boolVal);
						 }
					}

				  break;
				  
				case PANEL_RATE_APPLYCOINCTOALL:
					 
					for(fourChannelGroup = 0; fourChannelGroup < DeviceInfo.NbOfFeFPGAs; fourChannelGroup++)
					{
						for(i = 0; i < NB_OF_CHANNELS_IN_FE_FPGA; i++)
						{
							coincidenceMask = 0;
							
							for(j = 0; j < NB_OF_CHANNELS_IN_FE_FPGA; i++)
							 {
							 	 GetCtrlVal(RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i * NB_OF_CHANNELS_IN_FE_FPGA + j], &boolVal);
								 coincidenceMask |= (boolVal >> j);
							 }
							
							 errCode = WAVECAT64CH_SetRateCounterCoincidenceMask(fourChannelGroup, i, coincidenceMask);
						}
					}
					
				  break;
				  
				case PANEL_RATE_CH_FOR_RATE_STAT:
					 
					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_CH_FOR_RATE_STAT, &intVal);
					 ChannelForRateStats = intVal;

					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_CH_FOR_RATE_PLOT, &boolVal);

					 if(boolVal == TRUE) // Si on plotte en single channel, on le met à jour
					 {
						 ChannelForRatePlot = ChannelForRateStats;
						 SetTraceAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, 1, ATTR_TRACE_COLOR, Set_Color(ChannelForRatePlot));
					 	 ClearStripChart (RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART);
					 }
						 
				  break;
				  
				case PANEL_RATE_CH_FOR_RATE_PLOT:
					 
					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_CH_FOR_RATE_PLOT, &boolVal);
					 
					 if(boolVal == FALSE)
						 ChannelForRatePlot = -1; // All channels
					 else
						 ChannelForRatePlot = ChannelForRateStats;

					 if(ChannelForRatePlot == -1) // All channels
					 {
						 SetCtrlAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ATTR_NUM_TRACES, NbOfChannels);
						 for(channel = 0; channel < NbOfChannels; channel++)
							SetTraceAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, channel + 1, ATTR_TRACE_COLOR, Set_Color(channel));  // Attention : le numéro de la trace part de 1 dans le stripchart !!!
					 }
					 else
					 {
						 SetCtrlAttribute (RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ATTR_NUM_TRACES, 1);  // Une seule voie
						 SetTraceAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, 1, ATTR_TRACE_COLOR, Set_Color(ChannelForRatePlot));
					 }
					 	
					 ClearStripChart(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART);

				break; 
				
				case PANEL_RATE_PLOTINTERVAL:
					 
					 GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_PLOTINTERVAL, &floatVal); // en s
					 if(floatVal < 0.1)
					 { 
						 floatVal = 0.1;
						 SetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_PLOTINTERVAL, floatVal);
					 }
					 if(floatVal > 16.0) 
					 { 
						 floatVal = 16.0;
						 SetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_PLOTINTERVAL, floatVal);
					 }

					 PlotIntervalForRateStripchart = (int)(floatVal * 1000); // en ms

					 if(floatVal != 1.0)
						sprintf(xAxisTitle, "Time intervals");
					 else
						sprintf(xAxisTitle, "Time [s]");

					 SetCtrlAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ATTR_XNAME, xAxisTitle);
					 
				break;
					 
				default : // Cas des masques de coincidence traité ci-dessous
				 
					for(i = 0; i < NB_OF_CHANNELS_IN_FE_FPGA * NB_OF_CHANNELS_IN_FE_FPGA; i++)
					{
						if(control == ChannelInCoincidenceForRateHandle[i])
						{
							GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_SELFEFPGAFORCOINC, &fourChannelGroup);
							channelInGroup = (int)(i / NB_OF_CHANNELS_IN_FE_FPGA);
							errCode = WAVECAT64CH_GetRateCounterCoincidenceMask(fourChannelGroup, channelInGroup, &coincidenceMask);
							
						 	GetCtrlVal(RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i], &boolVal);

							if(boolVal == FALSE)
								coincidenceMask &= (0xF - (0x1 << (i%NB_OF_CHANNELS_IN_FE_FPGA)));
							else
								coincidenceMask |= (0x1 << (i%NB_OF_CHANNELS_IN_FE_FPGA));
							
							errCode = WAVECAT64CH_SetRateCounterCoincidenceMask(fourChannelGroup, channelInGroup, coincidenceMask);
						}
					}
					
				break;  
  			 }
		if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n");   	 
		break;
		
	}
	return 0;
}

/* ============================================================================ */ 
/* ============================ Timer Callback ================================ */ 
/* ============================================================================ */ 

/* ============================================================================ */ 
int CVICALLBACK TimerCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* ============================================================================ */ 
{
double val;
int i;
float eventsPerSecond;

	switch (event)
	{
		case EVENT_TIMER_TICK:
			if (AcquisitionRunning == TRUE)
			{
				 CurrentTimer = Timer(); 

			 	 eventsPerSecond= (EventNumber - FormerEventNumber)/(CurrentTimer - FormerTimer);
				 
			 	 FormerTimer = CurrentTimer;
				 
			 	 FormerEventNumber = EventNumber;
				 
				 if(EventNumber != 0)
					 SetCtrlVal (MainPanelHandle,MAINPANEL_EVENTSPERSECOND, eventsPerSecond); 
	  		}
			else
			{
				 FormerTimer = 0;
				 CurrentTimer = 0;
				 FormerEventNumber = 0;
				 eventsPerSecond = 0;
			}
			
			break;
	}
	return 0;
}

/* ============================================================================ */ 
/* ============================================================================ */ 
 
//-------------------------------------------------------------------------------
// Internal functions
//-------------------------------------------------------------------------------

/* ============================================================================ */ 
static void Init_LocalVariables(void)
/* ============================================================================ */ 
{
int i, channel, fourChannelGroup;
unsigned char coincidenceMask;
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 

	NbOfChannels = DeviceInfo.NbOfFeChannels + DeviceInfo.NbOfBackExtraChannels;

	errCode = WAVECAT64CH_GetSamplingFrequency(&SamplingFrequency); 			

	for(channel = 0; channel < NbOfChannels; channel++)
		errCode = WAVECAT64CH_GetChannelDCOffset(WAVECAT64CH_FRONT_CHANNEL, channel, &DCOffset[channel]); 

	errCode = WAVECAT64CH_GetTriggerDelay(&TriggerDelay); 			

	errCode = WAVECAT64CH_GetTriggerMode(&TriggerType); 
	
 	for(channel = 0; channel < NbOfChannels; channel++)
		 errCode = WAVECAT64CH_GetTriggerSourceState(WAVECAT64CH_FRONT_CHANNEL, channel, &ChannelTriggerEnable[channel]);

	for(channel = 0; channel < NbOfChannels; channel++)
		errCode = WAVECAT64CH_GetTriggerThreshold(WAVECAT64CH_FRONT_CHANNEL, channel, &TriggerThreshold[channel]); 			

	for(channel = 0; channel < NbOfChannels; channel++)
		errCode = WAVECAT64CH_GetTriggerEdge(WAVECAT64CH_FRONT_CHANNEL, channel, &TriggerPolarity[channel]); 			

	for(channel = 0; channel < NbOfChannels; channel++)
		PulsePattern[channel] = 0x01;
	
	for(channel = 0; channel < NbOfChannels; channel++)
		EnablePulseChannels[channel] = 0;
	
	for(fourChannelGroup = 0; fourChannelGroup < DeviceInfo.NbOfFeFPGAs; fourChannelGroup++)
	{
		 CoincidenceMask[fourChannelGroup] = 0;
	 
		 for(i = 0; i < NB_OF_CHANNELS_IN_FE_FPGA; i++)
		 {
	  		 errCode = WAVECAT64CH_GetRateCounterCoincidenceMask(fourChannelGroup, i, &coincidenceMask);
			 CoincidenceMask[fourChannelGroup] += (coincidenceMask << (i * NB_OF_CHANNELS_IN_FE_FPGA));
		 }
	}
	
	PlotIntervalForRateStripchart = 1000.0; // en ms
	ChannelForRateStats = 0;
	ChannelForRatePlot = -1; // All channels
	
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
}

/* ============================================================================ */ 
void Update_Panel(void)
/* ============================================================================ */ 
{
int i, j, samIndex, channel, channelType, intVal, selCh, fourChannelGroup;
unsigned char ucharVal, coincidenceMask;
unsigned short pulsePattern;
Boolean boolVal;
float floatVal;
char label[16];
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 	
// Display

	Set_ChannelSelectionList(MainPanelHandle, MAINPANEL_SELCH_FOR_DISPLAY);

// Horizontal
	
	errCode = WAVECAT64CH_GetTriggerDelay(&ucharVal);
    SetCtrlVal(MainPanelHandle, MAINPANEL_DELAY, ucharVal); 
	
	errCode = WAVECAT64CH_GetSamplingFrequency(&intVal);
    SetCtrlVal(MainPanelHandle, MAINPANEL_HORIZONTALSCALE, intVal); 
    SetCtrlVal(MainPanelHandle, MAINPANEL_HORIZONTALSCALEIND, (int) (1000000.0 / (float)intVal)); 
	
// Trigger
	
	Set_ChannelSelectionList(MainPanelHandle, MAINPANEL_SELCH_FOR_TRIG);
	GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_TRIG, &selCh);
	if(selCh < DeviceInfo.NbOfFeChannels)
	{
		channelType = WAVECAT64CH_FRONT_CHANNEL;
		channel = selCh;
	}
	else
	{
		channelType = WAVECAT64CH_BACK_EXTRA_CHANNEL;
		channel = selCh - DeviceInfo.NbOfFeChannels;
	}
		
	errCode = WAVECAT64CH_GetTriggerSourceState(channelType, channel, &boolVal);
	SetCtrlVal(MainPanelHandle, MAINPANEL_ENTRIGCH, boolVal);

	errCode = WAVECAT64CH_GetTriggerEdge(channelType, channel, &boolVal);
	SetCtrlVal(MainPanelHandle, MAINPANEL_TRIGEDGE, boolVal);

	errCode = WAVECAT64CH_GetTriggerThreshold(channelType, channel, &floatVal);
    SetCtrlVal(MainPanelHandle, MAINPANEL_THRESHOLD, floatVal); 
	
	errCode = WAVECAT64CH_GetTriggerMode(&intVal);
    SetCtrlVal(MainPanelHandle, MAINPANEL_TRIGGERTYPE, intVal); 
	
// Vertical
	
	Set_ChannelSelectionList(MainPanelHandle, MAINPANEL_SELCH_FOR_OFFSET);
	GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_OFFSET, &intVal); 
	if(selCh < DeviceInfo.NbOfFeChannels)
	{
		channelType = WAVECAT64CH_FRONT_CHANNEL;
		channel = selCh;
	}
	else
	{
		channelType = WAVECAT64CH_BACK_EXTRA_CHANNEL;
		channel = selCh - DeviceInfo.NbOfFeChannels;
	}
		
	errCode = WAVECAT64CH_GetChannelDCOffset(channelType, channel, &floatVal);
    SetCtrlVal(MainPanelHandle, MAINPANEL_OFFSET, floatVal); 
	
// Corrections
	
	errCode = WAVECAT64CH_GetCorrectionMode(&intVal);
	if(intVal == WAVECAT64CH_NO_CORRECTION)
	{
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEPEDCORRECTION, FALSE);
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEINLCORRECTION, FALSE);
	}
	else if(intVal == WAVECAT64CH_PEDESTAL_CORRECTION_ONLY)
	{
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEPEDCORRECTION, TRUE);
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEINLCORRECTION, FALSE);
	}
	else if(intVal == WAVECAT64CH_ALL_CORRECTIONS)
	{
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEPEDCORRECTION, TRUE);
		SetCtrlVal(MainPanelHandle, MAINPANEL_ENABLEINLCORRECTION, TRUE);
	}
	
// Pulsers
	
	errCode = WAVECAT64CH_DisablePulsers();
		
	Set_ChannelSelectionList(MainPanelHandle, MAINPANEL_SELCH_FOR_PATTERN);
	GetCtrlVal(MainPanelHandle, MAINPANEL_SELCH_FOR_PATTERN, &selCh);  
	if(selCh < DeviceInfo.NbOfFeChannels)
	{
		channelType = WAVECAT64CH_FRONT_CHANNEL;
		channel = selCh;
	}
	else
	{
		channelType = WAVECAT64CH_BACK_EXTRA_CHANNEL;
		channel = selCh - DeviceInfo.NbOfFeChannels;
	}
		
	errCode = WAVECAT64CH_GetPulsePattern(channelType, channel, &pulsePattern); 
	SetCtrlVal(MainPanelHandle, MAINPANEL_PULSEPATTERN, pulsePattern); 
	
// Rate Panel
	
	Set_FourChannelGroupSelectionList(RateStatisticsPanelHandle, PANEL_RATE_SELFEFPGAFORCOINC);
	Set_ChannelSelectionList(RateStatisticsPanelHandle, PANEL_RATE_CH_FOR_RATE_STAT);
	
	GetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_SELFEFPGAFORCOINC, &fourChannelGroup);
	CoincidenceMask[fourChannelGroup] = 0;

	for(i = 0; i < NB_OF_CHANNELS_IN_FE_FPGA; i++)
	{
		sprintf(label, "%d", fourChannelGroup * NB_OF_CHANNELS_IN_FE_FPGA + i);
		SetCtrlVal (RateStatisticsPanelHandle, LabelInCoincidenceForRateHandle[i], label);
		sprintf(label, "Ch%d", fourChannelGroup * NB_OF_CHANNELS_IN_FE_FPGA + i);
		SetCtrlAttribute (RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i], ATTR_LABEL_TEXT, label);

		errCode = WAVECAT64CH_GetRateCounterCoincidenceMask(fourChannelGroup, i, &coincidenceMask);
		CoincidenceMask[fourChannelGroup] += (coincidenceMask << (i * NB_OF_CHANNELS_IN_FE_FPGA));

		for(j = 0; j < NB_OF_CHANNELS_IN_FE_FPGA; j++)
		{
			 boolVal = (coincidenceMask & (0x1 << j)) >> j;

			 SetCtrlVal(RateStatisticsPanelHandle, ChannelInCoincidenceForRateHandle[i * NB_OF_CHANNELS_IN_FE_FPGA + j], boolVal);
		}
	}
	
	 SetCtrlAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ATTR_NUM_TRACES, NbOfChannels);
	 for(channel = 0; channel < NbOfChannels; channel++)
		SetTraceAttribute(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, channel + 1, ATTR_TRACE_COLOR, Set_Color(channel));  // Attention : le numéro de la trace part de 1 dans le stripchart !!!
}

/* =========================================================================== */							
static void Set_ChannelSelectionList(int panelHandle, int controlName)
/* =========================================================================== */							
{
int channel, itemNumber;
char label[16];

	ClearListCtrl (panelHandle, controlName);
	itemNumber = 0;

	for(channel = 0; channel < NbOfChannels; channel++)
	{
		sprintf(label, "Ch%d", channel);
		InsertListItem (panelHandle, controlName, itemNumber, label, channel);
		itemNumber ++;
	}
		
	SetCtrlIndex (panelHandle, controlName, 0);
}

/* =========================================================================== */							
static void Set_FourChannelGroupSelectionList(int panelHandle, int controlName)
/* =========================================================================== */							
{
int fourChannelGroup, itemNumber;
char label[16];

	ClearListCtrl (panelHandle, controlName);
	itemNumber = 0;

	for(fourChannelGroup = 0; fourChannelGroup < DeviceInfo.NbOfFeFPGAs; fourChannelGroup++)
	{
		sprintf(label, "Group%d", fourChannelGroup);
		InsertListItem (panelHandle, controlName, itemNumber, label, fourChannelGroup);
		itemNumber ++;
	}
		
	SetCtrlIndex (panelHandle, controlName, 0);
}

/* ============================================================================ */ 
/* ============================= Gestion du run ================================*/
/* ============================================================================ */ 


/* =========================================================================== */							
static void Start_run (void)
/* =========================================================================== */							
{
	WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 
	
	errCode = WAVECAT64CH_AllocateEventStructure(&CurrentEvent);  

	errCode = WAVECAT64CH_StartRun();   
	
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
}

/* =========================================================================== */							
static void Stop_run (void)
/* =========================================================================== */							
{
	
	WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 
	
	errCode = WAVECAT64CH_StopRun();

	errCode = WAVECAT64CH_FreeEventStructure(&CurrentEvent); 
	
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
}

/* =========================================================================== */							
static void	Prepare_Event(void) 
/* =========================================================================== */							
{
	WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 
	
	errCode = WAVECAT64CH_PrepareEvent();
	
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
}
	
/* =========================================================================== */							
/* ========================== Run en rate counters ============================*/
/* =========================================================================== */							

/* =========================================================================== */							
static int CVICALLBACK Run_WithRateCounters(void* unused)
/* =========================================================================== */							
{
int twoChannelGroup, timeCounterCh0, timeCounterCh1, trigRateCode; 
unsigned short hitCounterCh0, hitCounterCh1;
Boolean boolVal;
float rawTrigRate, trigRate;
char triggerRateUnit[5];
WAVECAT64CH_ErrCode errCode = WAVECAT64CH_Success; 

	for(;;)
	{
		for(twoChannelGroup = 0;twoChannelGroup < DeviceInfo.NbOfFeSamBlocks; twoChannelGroup ++)
			errCode = WAVECAT64CH_StartRateCounters(WAVECAT64CH_FRONT_CHANNEL, twoChannelGroup);
		
		for(twoChannelGroup = 0;twoChannelGroup < DeviceInfo.NbOfBackExtraSamBlocks; twoChannelGroup ++)
			errCode = WAVECAT64CH_StartRateCounters(WAVECAT64CH_BACK_EXTRA_CHANNEL, twoChannelGroup);
		
		
		Sleep(PlotIntervalForRateStripchart); // en ms
		
		for(twoChannelGroup = 0;twoChannelGroup < DeviceInfo.NbOfFeSamBlocks; twoChannelGroup ++)
		{
			errCode = WAVECAT64CH_ReadRateCounters(WAVECAT64CH_FRONT_CHANNEL, twoChannelGroup, 
			&hitCounterCh0, &hitCounterCh1, &timeCounterCh0, &timeCounterCh1);
			if(timeCounterCh0 != 0)
				ChannelHitRate[twoChannelGroup * 2] = 1000000 * (float)hitCounterCh0 / timeCounterCh0; // en hits/s
			else
				ChannelHitRate[twoChannelGroup * 2] = 0.0;
			
			if(timeCounterCh1 != 0)
				ChannelHitRate[twoChannelGroup * 2 + 1] = 1000000 * (float)hitCounterCh1 / timeCounterCh1; // en hits/s
			else
				ChannelHitRate[twoChannelGroup * 2 + 1] = 0.0;
		}
		
		for(twoChannelGroup = 0;twoChannelGroup < DeviceInfo.NbOfBackExtraSamBlocks; twoChannelGroup ++)
		{
			errCode = WAVECAT64CH_ReadRateCounters(WAVECAT64CH_FRONT_CHANNEL, twoChannelGroup, 
			&hitCounterCh0, &hitCounterCh1, &timeCounterCh0, &timeCounterCh1);
			if(timeCounterCh0 != 0)
				ChannelHitRate[twoChannelGroup * 2 + DeviceInfo.NbOfBackExtraSamBlocks] = 1000000 * (float)hitCounterCh0 / timeCounterCh0; // en hits/s
			else
				ChannelHitRate[twoChannelGroup * 2 + DeviceInfo.NbOfBackExtraSamBlocks] = 0.0;
			
			if(timeCounterCh1 != 0)
				ChannelHitRate[twoChannelGroup * 2 + 1 + DeviceInfo.NbOfBackExtraSamBlocks] = 1000000 * (float)hitCounterCh1 / timeCounterCh1; // en hits/s
			else
				ChannelHitRate[twoChannelGroup * 2 + 1 + DeviceInfo.NbOfBackExtraSamBlocks] = 0.0;
		}
		
		if(ChannelForRatePlot == -1)
			PlotStripChart(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ChannelHitRate, NbOfChannels, 0, 0, VAL_FLOAT);
		else
			PlotStripChart(RateStatisticsPanelHandle, PANEL_RATE_RAWTRIGGERSTRIPCHART, ChannelHitRate + ChannelForRatePlot, 1, 0, 0, VAL_FLOAT);
		
		rawTrigRate = ChannelHitRate[ChannelForRateStats]; // pour l'affichage au desssus du stripchart

		if(rawTrigRate >= 1000000.0)
		{
		   trigRateCode = 1;
		   sprintf(triggerRateUnit,"MHz");  // MHz
		}
		else if((rawTrigRate < 1000000.0) && (rawTrigRate >= 1000.0))
		{
		   trigRateCode = 2;
		   sprintf(triggerRateUnit,"kHz");  // kHz
		}
		else 
		{
		   trigRateCode = 3;
		   sprintf(triggerRateUnit," Hz");  //Hz
		}

		if (trigRateCode == 3)		// Hz
		    trigRate = rawTrigRate;

		else if (trigRateCode == 2)   // kHz
		    trigRate = rawTrigRate / 1000.0; 

		else trigRate = rawTrigRate / 1000000.0;

		SetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_RATEVALUE, trigRate);
		SetCtrlVal(RateStatisticsPanelHandle, PANEL_RATE_RATEUNITY, triggerRateUnit);

		if(RateRunStopRequested == TRUE) break;
	}
	
	errCode = WAVECAT64CH_DisableRateCounters();
	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
	return 0;
}
	
/* =========================================================================== */							
/* ============================== Utilitaires ================================*/
/* =========================================================================== */							

/* =========================================================================== */	
static int Set_Color(int channel)
/* =========================================================================== */
{
int color=0;
	
	if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 0)
		color = MakeColor(255,0,0);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 1)
		color = MakeColor(255,108,108);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 2)
		color =  MakeColor(255,128,0);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 3)
		color =  MakeColor(255,191,128);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 4)
		color =  MakeColor(140,140,0);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 5)
		color =  MakeColor(255,255,0); 
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 6)
		color = MakeColor(0,176,0);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 7)
		color = MakeColor(128,255,128);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 8)
		color =  MakeColor(0,168,168);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 9)
		color =  MakeColor(119,255,255); 
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 10)
		color =  MakeColor(0,128,255);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 11)
		color =  MakeColor(128,191,255);   
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 12)
		color =  MakeColor(128,0,255);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 13)
		color =  MakeColor(191,128,255);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 14)
		color = MakeColor(204,0,204);
	else if(channel%MAX_NB_OF_CHANNELS_TO_PLOT == 15)
		color = MakeColor(255,130,255);
	
	return color;
}		

