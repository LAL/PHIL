/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2015. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MAINPANEL                        1       /* callback function: MainpanelCB */
#define  MAINPANEL_GRAPH                  2       /* control type: graph, callback function: (none) */
#define  MAINPANEL_HORIZONTALSCALE        3       /* control type: slide, callback function: HorizontalCB */
#define  MAINPANEL_DELAY                  4       /* control type: scale, callback function: HorizontalCB */
#define  MAINPANEL_HORIZONTALSCALEIND     5       /* control type: numeric, callback function: (none) */
#define  MAINPANEL_TRIGGER                6       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_HORIZONTAL             7       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_ENTRIGCH               8       /* control type: radioButton, callback function: TriggerCB */
#define  MAINPANEL_TEXTMSG_2              9       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION_31          10      /* control type: deco, callback function: (none) */
#define  MAINPANEL_VERTICALSCALE          11      /* control type: slide, callback function: VerticalCB */
#define  MAINPANEL_TEXTMSG_21             12      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TEXTMSG_4              13      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_VERTICAL               14      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION_33          15      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_30          16      /* control type: deco, callback function: (none) */
#define  MAINPANEL_START                  17      /* control type: pictButton, callback function: RunCB */
#define  MAINPANEL_STOP                   18      /* control type: pictButton, callback function: RunCB */
#define  MAINPANEL_EVENTID                19      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_EVENTNUMBER            20      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_OFFSET                 21      /* control type: scale, callback function: VerticalCB */
#define  MAINPANEL_TRIGGERTYPE            22      /* control type: slide, callback function: TriggerCB */
#define  MAINPANEL_TRIGEDGE               23      /* control type: binary, callback function: TriggerCB */
#define  MAINPANEL_EDGEMSG                24      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_PICTURE_2              25      /* control type: picture, callback function: (none) */
#define  MAINPANEL_THRESHOLD              26      /* control type: scale, callback function: TriggerCB */
#define  MAINPANEL_ENABLEPULSERS          27      /* control type: radioButton, callback function: PulserCB */
#define  MAINPANEL_PULSEPATTERN           28      /* control type: numeric, callback function: PulserCB */
#define  MAINPANEL_SELCH_FOR_OFFSET       29      /* control type: ring, callback function: VerticalCB */
#define  MAINPANEL_SELCH_FOR_DISPLAY      30      /* control type: ring, callback function: DisplayCB */
#define  MAINPANEL_SELCH_FOR_PATTERN      31      /* control type: ring, callback function: PulserCB */
#define  MAINPANEL_SELCH_FOR_TRIG         32      /* control type: ring, callback function: TriggerCB */
#define  MAINPANEL_TEXTMSG_19             33      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_ENABLEPEDCORRECTION    34      /* control type: radioButton, callback function: SAMCorrectionsCB */
#define  MAINPANEL_ENABLEINLCORRECTION    35      /* control type: radioButton, callback function: SAMCorrectionsCB */
#define  MAINPANEL_TEXTMSG                36      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TEXTMSG_22             37      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_APPLYTOALL_TRIGEDGE    38      /* control type: command, callback function: TriggerCB */
#define  MAINPANEL_APPLYTOALL_ENTRIGCH    39      /* control type: command, callback function: TriggerCB */
#define  MAINPANEL_APPLYTOALL_PATTERN     40      /* control type: command, callback function: PulserCB */
#define  MAINPANEL_OPENRATEPANEL          41      /* control type: command, callback function: DisplayCB */
#define  MAINPANEL_APPLYTOALL_THRESHOLD   42      /* control type: command, callback function: TriggerCB */
#define  MAINPANEL_APPLYTOALL_DCOFFSET    43      /* control type: command, callback function: VerticalCB */
#define  MAINPANEL_DISPLAYON              44      /* control type: radioButton, callback function: DisplayCB */
#define  MAINPANEL_EVENTSPERSECOND        45      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_TEXTMSG_12             46      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_VERTICAL_2             47      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TEXTMSG_33             48      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_PICTURE                49      /* control type: picture, callback function: (none) */
#define  MAINPANEL_TIMER                  50      /* control type: timer, callback function: TimerCB */
#define  MAINPANEL_DECORATION_34          51      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_35          52      /* control type: deco, callback function: (none) */
#define  MAINPANEL_VERTICAL_3             53      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION             54      /* control type: deco, callback function: (none) */

typedef enum {
	
	SYSTEM_TRIGGER_SOFT,
	SYSTEM_TRIGGER_NORMAL,
	SYSTEM_TRIGGER_AUTO,
	SYSTEM_TRIGGER_EXTERN
	
}TriggerType_t;


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK DisplayCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK HorizontalCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainpanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PulserCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RunCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SAMCorrectionsCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TriggerCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK VerticalCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

/* ============================================================================= */ 
/* ========================= Static functions ================================== */ 
/* ============================================================================= */ 

static int CVICALLBACK Start_Acquisition (void* unused);

static void Stop_run (void);
static void Start_run (void);
static void	Prepare_Event(void);
static int  CVICALLBACK Run_WithRateCounters(void* unused);
static int	Read_EventBuffer(void);
static int  Get_NumberOfEvents(void); 
static int  Decode_Event(int eventNumber);
static void Init_LocalVariables(void);
 void Update_Panel(void); 
static void Set_ChannelSelectionList(int panelHandle, int controlName);
static void Set_FourChannelGroupSelectionList(int panelHandle, int controlName);
static int 	Set_Color(int channel);

int Start_Acquisition_TCP(void);




//variables needed by the server
int EventNumber;
int StopAcquisition;
int AcquisitionRunning;
WAVECAT64CH_EventStruct CurrentEvent;   
Boolean DisplayON;


static int ThreadID;
static void *ThreadData;
static int RunRateThreadID;
static void *RunRateThreadData;


#ifdef __cplusplus
    }
#endif
