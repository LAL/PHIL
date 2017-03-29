/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2014. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL_RATE                       1       /* callback function: PanelRateStatCB */
#define  PANEL_RATE_CH_FOR_RATE_STAT      2       /* control type: ring, callback function: RateStatCB */
#define  PANEL_RATE_PLOTINTERVAL          3       /* control type: numeric, callback function: RateStatCB */
#define  PANEL_RATE_RATEVALUE             4       /* control type: numeric, callback function: (none) */
#define  PANEL_RATE_RAWTRIGGERSTRIPCHART  5       /* control type: strip, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_27            6       /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_DECORATION_8          7       /* control type: deco, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_3             8       /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_38            9       /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_37            10      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_ENABLERATESTAT        11      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_RATEUNITY_2           12      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_RATEUNITY             13      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_SELFEFPGAFORCOINC     14      /* control type: ring, callback function: RateStatCB */
#define  PANEL_RATE_CH_FOR_RATE_PLOT      15      /* control type: binary, callback function: RateStatCB */
#define  PANEL_RATE_DECORATION_37         16      /* control type: deco, callback function: (none) */
#define  PANEL_RATE_COINCBOX3_3           17      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX3_2           18      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX3_1           19      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX3_0           20      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX2_3           21      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX2_2           22      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX2_1           23      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX2_0           24      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX1_3           25      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX1_2           26      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX1_1           27      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX1_0           28      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX0_3           29      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX0_2           30      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX0_1           31      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_COINCBOX0_0           32      /* control type: radioButton, callback function: RateStatCB */
#define  PANEL_RATE_DECORATION_36         33      /* control type: deco, callback function: (none) */
#define  PANEL_RATE_APPLYCOINCTOALL       34      /* control type: command, callback function: RateStatCB */
#define  PANEL_RATE_TEXTMSG_44            35      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_43            36      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_42            37      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_41            38      /* control type: textMsg, callback function: (none) */
#define  PANEL_RATE_TEXTMSG_40            39      /* control type: textMsg, callback function: (none) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelRateStatCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RateStatCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
