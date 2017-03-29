//==============================================================================
//
// Title:       WAVECAT64CH_DLL   Version 2.5.3
// Purpose:     A short description of the interface.
//
// Created on:  03/07/2014 J.Maalmi / D.Breton
// Copyright:   LAL. All Rights Reserved.
//
//==============================================================================


#ifndef __WAVECAT64CH_LIB_H__
#define __WAVECAT64CH_LIB_H__

#if defined(_WINDOWS) || defined(_WINDOWS_) 
	#ifdef WAVECAT64CH_DLL_EXPORTS
		#define WAVECAT64CH_DLL_API __declspec(dllexport)
	#else
		#define WAVECAT64CH_DLL_API __declspec(dllimport)
	#endif
#else
	#define WAVECAT64CH_DLL_API
	#define _cstmcall
#endif

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files


//#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types
#ifndef BOOLEAN
typedef int Boolean;
#define BOOLEAN
#endif

#define TRUE 1
#define FALSE 0

//==============================================================================
// External variables

//==============================================================================
// Global functions

// ================================================================================================ //
// ==================================    Constants Definitions       ============================== //

#define  MAX_NUM_OF_DEVICES 8
#define  MAX_DESCRIPTOR_SIZE 1024
#define  SERNUM_SIZE 8
#define  WAVECAT64CH_MAX_DATA_SIZE 1024
#define  WAVECAT64CH_INFO_NOT_AVAILABLE -1
#define  WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK 2
#define  WAVECAT64CH_MAX_NB_OF_FE_BOARDS 4
#define  WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK 2
#define  WAVECAT64CH_MAX_TOTAL_NB_OF_SAMBLOCKS 36
#define  WAVECAT64CH_MAX_TOTAL_NB_OF_CHANNELS  72
#define  WAVECAT64CH_ADCTOVOLTS 0.00061

		
// ================================================================================================ // 
// ===============================   Enumérations    ============================================ // 
// ================================================================================================ //  

typedef enum
{
	WAVECAT64CH_SYSTEM_IS_WAVECAT_8CH,
	WAVECAT64CH_SYSTEM_IS_WAVECAT_16CH,
	WAVECAT64CH_SYSTEM_IS_WAVECAT_64CH
	
} WAVECAT64CH_TypeOfSystem;


typedef enum
{
	WAVECAT64CH_Success = 0,
	WAVECAT64CH_CommError = -1, 
	WAVECAT64CH_GenericError = -2 ,
	WAVECAT64CH_DeviceNotFound = -3,
	WAVECAT64CH_DeviceAlreadyOpen = -4, 
	WAVECAT64CH_NoDeviceConnected = -5,
	WAVECAT64CH_InvalidParam = -6, // valeur de paramètre absente d'une liste
	WAVECAT64CH_No_Event = -7,
	WAVECAT64CH_IncompleteEvent = -8,
	WAVECAT64CH_ReadoutError = -9,
	WAVECAT64CH_ErrorAcquisitionRunning = -10,
	WAVECAT64CH_EventHeaderError = -11,
	WAVECAT64CH_EventIdInconsistenceError = -12,
	WAVECAT64CH_SourceIdInconsistenceError = -13,
	WAVECAT64CH_EventTrailerError = -14,
	WAVECAT64CH_ExtendedReadoutError = -15,
	WAVECAT64CH_OutOfRangeParam = -16, // valeur de paramètre out of range
	WAVECAT64CH_NotYetImplemented = -99

} WAVECAT64CH_ErrCode;


typedef enum
{
	WAVECAT64CH_EVENT_WITH_WAVEFORM =0,
	WAVECAT64CH_EVENT_WITHOUT_WAVEFORM =1
 
} WAVECAT64CH_DataType;


typedef enum
{
	WAVECAT64CH_SYSTEM_FPGA,
	WAVECAT64CH_CTRL_FPGA,
	WAVECAT64CH_FE_FPGA
 
} WAVECAT64CH_FpgaType; 


typedef enum
{
	 WAVECAT64CH_THRESHOLDDACS_NOT_AVAILABLE = -1,
	 WAVECAT64CH_THRESHOLDDACS_LOADED_FROM_EEPROM = 0,
	 WAVECAT64CH_THRESHOLDDACS_LOADED_FROM_FILES = 1, 
	
} WAVECAT64CH_ThresholdDacs_INFO;


typedef enum
{
	 WAVECAT64CH_SAMDACOFFSETS_NOT_AVAILABLE = -1,
	 WAVECAT64CH_SAMDACOFFSETS_LOADED_FROM_EEPROM = 0,
	 WAVECAT64CH_SAMDACOFFSETS_LOADED_FROM_FILES = 1, 
	
} WAVECAT64CH_SamDacOffsets_INFO;


typedef enum
{
	 WAVECAT64CH_PEDESTALS_NOT_AVAILABLE = -1,
	 WAVECAT64CH_PEDESTALS_LOADED_FROM_EEPROM = 0,
	 WAVECAT64CH_PEDESTALS_LOADED_FROM_FILES = 1, 
	
} WAVECAT64CH_Pedestals_INFO;


typedef enum
{
	WAVECAT64CH_INL_NOT_AVAILABLE = -1,
	WAVECAT64CH_INL_LOADED_FROM_EEPROM = 0,
	WAVECAT64CH_INL_LOADED_FROM_FILES = 1
	
} WAVECAT64CH_INL_INFO;


typedef enum
{
	WAVECAT64CH_TRIGGER_SOFT = 0,
	WAVECAT64CH_TRIGGER_NORMAL = 1,
	WAVECAT64CH_TRIGGER_INTERNAL = 2,
	WAVECAT64CH_TRIGGER_EXTERNAL = 3,
	WAVECAT64CH_TRIGGER_COINCIDENCE = 4,
	WAVECAT64CH_TRIGGER_NORMAL_PAIRED = 5,
	WAVECAT64CH_TRIGGER_COINCIDENCE_WITH_EXT_SIG = 6
	
} WAVECAT64CH_TriggerType;


typedef enum
{
	WAVECAT64CH_FRONT_CHANNEL = 0,
	WAVECAT64CH_BACK_EXTRA_CHANNEL = 1
	 
} WAVECAT64CH_ChannelType;


typedef enum
{
	WAVECAT64CH_EXT_TRIG = 0,
	WAVECAT64CH_EXT_SYNC = 1
	
} WAVECAT64CH_ExternalSignalType;


typedef enum
{
	WAVECAT64CH_LVTTL = 0,
	WAVECAT64CH_NIM = 1
	
} WAVECAT64CH_ExternalSignalNormType;


typedef enum
{
	WAVECAT64CH_3_2GHZ = 3200,
	WAVECAT64CH_2_13GHZ = 2133, 
	WAVECAT64CH_1_6GHZ = 1600,
	WAVECAT64CH_1_28GHZ = 1280,
	WAVECAT64CH_1_07GHZ = 1067,
	WAVECAT64CH_800MHZ = 800,
	WAVECAT64CH_533MHZ = 533,
	WAVECAT64CH_400MHZ = 400
	
} WAVECAT64CH_SamplingFrequencyType;


typedef enum
{
	WAVECAT64CH_STATE_ON = 1,
	WAVECAT64CH_STATE_OFF = 0
	 
} WAVECAT64CH_StateType;  


typedef enum
{
 WAVECAT64CH_POS_EDGE = 0,	
 WAVECAT64CH_NEG_EDGE = 1
 
} WAVECAT64CH_TriggerEdgeType;

typedef enum
{
 WAVECAT64CH_STANDARD_READOUT = 0,	
 WAVECAT64CH_SHORT_READOUT = 1
 
} WAVECAT64CH_ReadoutLatencyType;

typedef enum
{
 WAVECAT64CH_PULSER_SOURCE_IS_USB =0 ,
 WAVECAT64CH_PULSER_SOURCE_IS_EXT_SIG =1
	
} WAVECAT64CH_PulserSourceType;


typedef enum
{
 WAVECAT64CH_NO_CORRECTION,
 WAVECAT64CH_PEDESTAL_CORRECTION_ONLY,
 WAVECAT64CH_ALL_CORRECTIONS
	
} WAVECAT64CH_CorrectionModeType;
  
// ================================================================================================ //
// =================================== Device Info Struct ========================================= // 
// ================================================================================================ // 

typedef struct{

char	LibraryVersion[8];
	
int 	BoardConnected;

int		ControllerBoardVersion;		 // For 64-channel systems
int 	ControllerBoardSerialNumber; 
int 	ControllerBoardFPGAVersion;
int 	ControllerBoardFPGAEvolution;

int		MotherBoardVersion;		   // For 8-channel systems
int 	MotherBoardSerialNumber;   
int 	MotherBoardFPGAVersion;
int 	MotherBoardFPGAEvolution;

int		BoardVersionFromFeFPGA;   // Front-end board or mezzanine
int 	FPGAVersionFromFeFPGA;			 
int 	FPGAEvolutionFromFeFPGA;			 

WAVECAT64CH_ThresholdDacs_INFO   ThresholdDacs_Info;
WAVECAT64CH_SamDacOffsets_INFO   SamDacOffsets_Info;
WAVECAT64CH_Pedestals_INFO   	 Pedestals_Info;
WAVECAT64CH_INL_INFO		 	 INL_Info;

WAVECAT64CH_TypeOfSystem	SystemType;
int 						NbOfLayers;
int							NbOfFeBoards;
int							FE_Paths[WAVECAT64CH_MAX_NB_OF_FE_BOARDS];
int							NbOfFeFPGAs;
int							NbOfFeSamBlocks;
int							NbOfBackExtraSamBlocks;
int							NbOfFeChannels;
int							NbOfBackExtraChannels;
char						BoardFullSerialNumber[WAVECAT64CH_MAX_NB_OF_FE_BOARDS][8]; // Peut être utilisé soit pour les cartes en 64 voies, soit pour les mezzanines en 8 voies
	
} WAVECAT64CH_DeviceInfoStruct;

// ================================================================================================ // 
// =================================  Event Structs =============================================== //
// ================================================================================================ // 

typedef struct{
	
	WAVECAT64CH_ChannelType		ChannelType;
	int							Channel;
	int							TrigCount;
	int							TimeCount;
	int							WaveformDataSize; // In samples
	float						*WaveformData; // In ADC Counts
	float						Baseline;
	float						Peak;	 // In ADC Counts           
	int							PeakCell;
	float						Charge;
	Boolean						ChargeMeasureOverflow;
	float						CFDRisingEdgeTime;
	float						CFDFallingEdgeTime;
	int							FCR;
	
} WAVECAT64CH_ChannelDataStruct;

typedef struct{
	
	int								EventID;
	unsigned long long				TDC;
	WAVECAT64CH_ChannelDataStruct   *ChannelData;
	int 							NbOfSAMBlocksInEvent; // 2 channels per SAMBlock
	
} WAVECAT64CH_EventStruct;


// ================================================================================================ // 
// =================================  Measurement Struct ========================================== //
// ================================================================================================ // 

typedef struct{
			
	Boolean				ForceBaseline;	
	float				EnforcedBaselineValue;
	Boolean				FixedThresholdOrCFD;
	float				FixedThresholdValue;
	Boolean				PeakPolarity;
	int					PeakRatio;	   // Entre 1 et 16 (en seizièmes)
	Boolean				EnableDynamicCharge;
	int					PreCharge;	   // Entre 1 et 99 (en samples)
	int					RefCellForCharge;  // En samples
	int					ChargeLength; // Entre 4 et 63 (en colonnes => multiplier par 16 pour samples)
	
} WAVECAT64CH_FirmwareMeasurementParameters;

// ================================================================================================ // 
// ================================      	 Library Functions        ============================= //
// ================================================================================================ // 

// Initialization and basic functions 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetWaveCatDevices(char deviceDescriptor[MAX_NUM_OF_DEVICES][MAX_DESCRIPTOR_SIZE], char deviceSerNumChar[MAX_NUM_OF_DEVICES][SERNUM_SIZE], int *nbOfWaveCatDevices);
	
WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_OpenDevice(int *handle);

WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_OpenSelectedDevice(char deviceDescriptor[MAX_DESCRIPTOR_SIZE], char usbSerNumchar[SERNUM_SIZE], int *handle);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_CloseDevice(void);   
																   	   
WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_WriteAccess(int handle, WAVECAT64CH_FpgaType fpga_type, int board_target, int front_end_target, char sub_address, void* buffer, int word_count);   

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_ReadAccess(int handle, WAVECAT64CH_FpgaType fpga_type, int board_target, int front_end_target, char sub_address, unsigned char* buffer,int word_count);  

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_ResetDevice(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetDefaultParameters(void); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_LoadCalibrationParametersFromEEPROM(void); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetDeviceInfo(WAVECAT64CH_DeviceInfoStruct *DeviceInfoStruct);

// Enable or Disable channels

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetChannelState(WAVECAT64CH_ChannelType channelType, int channel, WAVECAT64CH_StateType channelState /*ON/OFF*/);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetChannelState(WAVECAT64CH_ChannelType channelType, int channel, WAVECAT64CH_StateType *channelState /*ON/OFF*/);

// Trigger Related functions 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerMode(WAVECAT64CH_TriggerType triggerType);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerMode(WAVECAT64CH_TriggerType *triggerType);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerSourceState(WAVECAT64CH_ChannelType channelType, int channel, WAVECAT64CH_StateType triggerState /*ON/OFF*/);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerSourceState(WAVECAT64CH_ChannelType channelType, int channel, WAVECAT64CH_StateType *triggerState);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetExternalSignalNorm(WAVECAT64CH_ExternalSignalType extSigType /*Sync/Trig*/, WAVECAT64CH_ExternalSignalNormType extSigNormType /*NIM/TTL*/);	  

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerEdge(WAVECAT64CH_ChannelType channelType,int channel, WAVECAT64CH_TriggerEdgeType triggerEdge);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerEdge(WAVECAT64CH_ChannelType channelType,int channel, WAVECAT64CH_TriggerEdgeType *triggerEdge);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerThreshold(WAVECAT64CH_ChannelType channelType,int channel, float thresholdValue);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerThreshold(WAVECAT64CH_ChannelType channelType,int channel, float *thresholdValue);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetExternalSignalEdge(WAVECAT64CH_ExternalSignalType  extSigType /*Sync/Trig*/, WAVECAT64CH_TriggerEdgeType triggerEdge); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetExternalSignalEdge(WAVECAT64CH_ExternalSignalType  extSigType /*Sync/Trig*/, WAVECAT64CH_TriggerEdgeType *triggerEdge); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerDelay(unsigned char triggerDelay); /* from 1 to 63 */

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerDelay(unsigned char *triggerDelay); /* from 1 to 63 */

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerPrimitivesGateLength(unsigned int gateLength);  // in ns

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerPrimitivesGateLength(unsigned int *gateLength);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerPrimitivesPairedOption(WAVECAT64CH_StateType pairedOptionState /*ON/OFF*/); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerPrimitivesPairedOption(WAVECAT64CH_StateType *pairedOptionState);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetTriggerPrimitivesOnExtTrigOutOption(WAVECAT64CH_StateType primitiveOptionState /*ON/OFF*/); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetTriggerPrimitivesOnExtTrigOutOption(WAVECAT64CH_StateType *primitiveOptionState);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SendSoftwareTrigger(void);

// Time Base related functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetRecordingDepth(unsigned short recordingDepth); /* from 64 to 1024 in multiples of 16 */

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetRecordingDepth(unsigned short *recordingDepth); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetSamplingFrequency(WAVECAT64CH_SamplingFrequencyType samplingFrequency);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetSamplingFrequency(WAVECAT64CH_SamplingFrequencyType *samplingFrequency);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetReadoutLatency(WAVECAT64CH_ReadoutLatencyType readoutLatency);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetReadoutLatency(WAVECAT64CH_ReadoutLatencyType *readoutLatency);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetExternalClockOption(WAVECAT64CH_StateType externalClockOptionState /*ON/OFF*/); // Enables an external clock which has to be at 200 MHz !

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetExternalClockOption(WAVECAT64CH_StateType *externalClockOptionState); 


// Other parameters related functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetChannelDCOffset(WAVECAT64CH_ChannelType channelType, int channel, float channelDCOffset);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetChannelDCOffset(WAVECAT64CH_ChannelType channelType, int channel, float *channelDCOffset);


// Pulser Parameters

WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_EnablePulsers(void);

WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_DisablePulsers(void);

WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_SetPulsePattern(WAVECAT64CH_ChannelType channelType, int channel, unsigned short pulsePattern);

WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_GetPulsePattern(WAVECAT64CH_ChannelType channelType, int channel, unsigned short *pulsePattern);


// Firmware Measurement related functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetFirmwareMeasurementParameters(WAVECAT64CH_FirmwareMeasurementParameters measParams); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetFirmwareMeasurementParameters(WAVECAT64CH_FirmwareMeasurementParameters *measParams); 


// Rate Counters related functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_EnableRateCounters(void); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_DisableRateCounters(void); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetRateCounterCoincidenceMask(int fourChannelGroup, int rateCounter, unsigned char coincidenceMask); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetRateCounterCoincidenceMask(int fourChannelGroup, int rateCounter, unsigned char *coincidenceMask); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_StartRateCounters(WAVECAT64CH_ChannelType channelType, int twoChannelGroup);
	
WAVECAT64CH_DLL_API WAVECAT64CH_ErrCode WAVECAT64CH_ReadRateCounters(WAVECAT64CH_ChannelType channelType, int twoChannelGroup, 
	unsigned short *hitCounterCh0, unsigned short *hitCounterCh1, int *timeCounterCh0, int *timeCounterCh1);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_EnableRateCounterVeto(void); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_DisableRateCounterVeto(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetRateCounterVetoLength(float vetoLength); 

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetRateCounterVetoLength(float *vetoLength); 


// Correction functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_SetCorrectionMode(WAVECAT64CH_CorrectionModeType correctionMode);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_GetCorrectionMode(WAVECAT64CH_CorrectionModeType *correctionMode);


// Acquisition related functions

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_AllocateEventStructure(WAVECAT64CH_EventStruct *eventStruct);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_StartRun(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_PrepareEvent(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_ReadEventBuffer(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_DecodeEvent(WAVECAT64CH_EventStruct *eventStruct);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_StopRun(void);

WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_FreeEventStructure(WAVECAT64CH_EventStruct *eventStruct);

/* =========================================================================== */							
// ================================================================================================ // 
// ================================================================================================ // 

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __WAVECAT64CH_LIB_H__ */
