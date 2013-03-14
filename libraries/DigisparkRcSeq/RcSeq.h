#ifndef RC_SEQ_H
#define RC_SEQ_H

/*
 English: by RC Navy (2012)
 =======
 <RcSeq> is an asynchronous library for ATmega328P (UNO), ATtiny84 and ATtiny85 to easily create servo's sequences and/or to execute short actions from RC commands.
 It can also be used to trig some short "actions" (the duration must be less than 20ms to not disturb the servo commands)
 The Application Programming Interface (API) makes <RcSeq> library very easy to use.
 <RcSeq> needs 3 other libraries written by the same author:
 1) <TinyPinChange>:  a library to catch asynchronously the input change using Pin Change Interruption capability of the AVR
 2) <SoftRcPulseIn>:  a library to catch asynchronously the input pulses using <TinyPinChange> library
 3) <SoftRcPulseOut>: a library mainly based on the <SoftwareServo> library, but with a better pulse generation to limit jitter
 RC Signals (receiver outputs) can be assigned to a control type:
 -Stick Positions (up to 8, but in practice, 4 is the maximum to manually discriminate each stick position)
 -Keyboard (<RcSeq> assumes Push-Buttons associated Pulse duration are equidistant)
 -Custom Keyboard (The pulse durations can be defined independently for each Push-Button)
 Some definitions:
 -Sequence: is used to sequence one or several servos (sequence is defined in a structure in the user's sketch to be performed when the RC command rises)
            The Sequence table (structure) may contain some servo motions and some short actions to call.
 -Short Action:   is used to perform a quick action (action is a short function defined in the user's sketch to be called when the RC command rises)
 CAUTION: the end user shall also use asynchronous programmation method in the loop() function (no blocking functions such as delay() or pulseIn()).
 http://p.loussouarn.free.fr

 Francais: par RC Navy (2012)
 ========
 <RcSeq> est une librairie asynchrone pour ATmega328P (UNO), ATtiny84 et ATtiny85 pour creer facilement des sequences de servos et/ou executer des actions depuis des commandes RC.
 Elle peut egalement etre utilisee pour lancer des "actions courtes" (la duree doit etre inferieure a 20ms pour ne pas perturber la commande des servos)
 L'Interface de Programmation d'Application (API) fait que la librairie <RcSeq> est tres facile a utiliser.
 <RcSeq> necessite 3 autres librairies ecrites par le meme auteur:
 1) <TinyPinChange>:  une librarie pour capter de maniere asynchrone les changements d'etat des broches utilisant les interruptions sur changement des pins des AVR
 2) <SoftRcPulseIn>:  une librarie pour capter de maniere asynchrone les impulsions entrantes en utilisant la librairie <TinyPinChange>
 3) <SoftRcPulseOut>: une librairie majoritairement basee sur la librairie <SoftwareServo>, mais avec une meilleur generation des impulsions pour limiter la gigue
 Les signaux RC (sorties du recepteur) peuvent etre associes a un type de controle:
 -Positions de Manche (jusqu'a 8, mais en pratique, 4 est le maximum pour discriminer manuellement les positions du manche)
 -Clavier (<RcSeq> suppose que les durees d'impulsion des Bouton-Poussoirs sont equidistantes)
 -Clavier "Maison" (Les durees d'impulsion peuvent etre definies de manière independante pour chaque Bouton-Poussoir)
 Quelques definitions:
 -Sequence: est utilisee pour sequencer un ou plusieurs servos (sequence est definie dans une structure dans le sketch utilisateur: est lancee quand la commande RC est recue)
            La table de sequence (structure) peut contenir des mouvements de servo et des actions courtes a appeler.
 -Action courte: est utilisee pour une action rapide (action est une fonction courte definie dans le sketch utilsateur: est appelee quand la commande RC est recue)
 ATTENTION: l'utilisateur final doit egalement utiliser la methode de programmation asynchrone dans la fonction loop() (pas de fonctions bloquantes comme delay() ou pulseIn()).
 http://p.loussouarn.free.fr
*/

#include "../SoftRcPulseIn/SoftRcPulseIn.h"
#include "../SoftRcPulseOut/SoftRcPulseOut.h"
#include "../TinyPinChange/TinyPinChange.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include <stdio.h>

#define SERVO_MAX_NB    10
#define SEQUENCE_MAX_NB 8
#define RC_CMD_MAX_NB   4

typedef struct {
  uint8_t      ServoIndex;
  uint8_t      StartInDegrees;
  uint8_t      EndInDegrees;
  uint32_t     StartMotionOffsetMs;
  uint32_t     MotionDurationMs;
  void         (*ShortAction)(void);
}SequenceSt_t;

typedef struct {
  uint16_t  Min;
  uint16_t  Max;
} KeyMap_t;

#define TABLE_ITEM_NBR(Tbl)  (sizeof(Tbl)/sizeof(Tbl[0]))

/* Macro de sequencement d'un mouvement avec demarrge lent et arret lents à utiliser dans la table de structure "Sequence[]" */
#define MOTION_WITH_SOFT_START_AND_STOP(ServoIndex,StartInDegrees,EndInDegrees,StartMvtOffsetMs,MvtDurationMs,PourCent)                                                                                                                                                                    \
  {ServoIndex, StartInDegrees,                                               StartInDegrees+((EndInDegrees-StartInDegrees)*PourCent)/100L, StartMvtOffsetMs,                                                                           (MvtDurationMs*2L*PourCent)/100L,       NULL  }, \
  {ServoIndex, StartInDegrees+((EndInDegrees-StartInDegrees)*PourCent)/100L, EndInDegrees-((EndInDegrees-StartInDegrees)*PourCent)/100L,   StartMvtOffsetMs+(MvtDurationMs*2L*PourCent)/100L,                                          (MvtDurationMs*(100L-4L*PourCent))/100L,NULL  }, \
  {ServoIndex, EndInDegrees-((EndInDegrees-StartInDegrees)*PourCent)/100L,   EndInDegrees,                                                (StartMvtOffsetMs+(MvtDurationMs*2L*PourCent)/100L)+(MvtDurationMs*(100L-4L*PourCent))/100L, (MvtDurationMs*2L*PourCent)/100L,       NULL  },

#define SHORT_ACTION_TO_PERFORM(ShortAction,StartActionOffsetMs) {255, 0, 0, StartActionOffsetMs, 0L, (ShortAction)},

enum {RC_CMD_STICK=0, RC_CMD_KEYBOARD, RC_CMD_CUSTOM};

#define RC_SEQUENCE(Sequence)			Sequence, TABLE_ITEM_NBR(Sequence)
#define RC_CUSTOM_KEYBOARD(KeyMap)		KeyMap, TABLE_ITEM_NBR(KeyMap)

#define CENTER_VALUE_US(CenterVal,Tol)  ((CenterVal)-(Tol)),((CenterVal)+(Tol))

void    RcSeq_Init(void);
uint8_t RcSeq_LibVersion(void);
uint8_t RcSeq_LibRevision(void);
char   *RcSeq_LibTextVersionRevision(void);
void    RcSeq_DeclareSignal(uint8_t Idx, uint8_t DigitalPin);
void    RcSeq_DeclareServo(uint8_t Idx, uint8_t DigitalPin);
void    RcSeq_DeclareKeyboardOrStickOrCustom(uint8_t ChIdx, uint8_t Type, uint16_t PulseMinUs, uint16_t PulseMaxUs, KeyMap_t *KeyMapTbl, uint8_t PosNb);
void    RcSeq_DeclareCustomKeyboard(uint8_t ChIdx, KeyMap_t *KeyMapTbl, uint8_t PosNb);
#define RcSeq_DeclareStick(ChIdx, PulseMinUs, PulseMaxUs, PosNb)      RcSeq_DeclareKeyboardOrStickOrCustom(ChIdx, RC_CMD_STICK, PulseMinUs, PulseMaxUs, NULL, PosNb)
#define RcSeq_DeclareKeyboard(ChIdx, PulseMinUs, PulseMaxUs, KeyNb)   RcSeq_DeclareKeyboardOrStickOrCustom(ChIdx, RC_CMD_KEYBOARD, PulseMinUs, PulseMaxUs, NULL, KeyNb)
void    RcSeq_DeclareCommandAndSequence(uint8_t CmdIdx,uint8_t TypeCmd,SequenceSt_t *Table, uint8_t SequenceLength);
void    RcSeq_DeclareCommandAndShortAction(uint8_t CmdIdx,uint8_t TypeCmd,void(*ShortAction)(void));
uint8_t RcSeq_LaunchSequence(SequenceSt_t *Table);
uint8_t RcSeq_LaunchShortAction(void(*ShortAction)(void));
void    RcSeq_Refresh(void);

/*******************************************************/
/* Application Programming Interface (API) en Francais */
/*******************************************************/

/* Macro en Francais de declaration mouvement   English native Macro to declare a motion */
#define MVT_AVEC_DEBUT_ET_FIN_MVT_LENTS			MOTION_WITH_SOFT_START_AND_STOP
#define ACTION_COURTE_A_EFFECTUER				SHORT_ACTION_TO_PERFORM
#define RC_CLAVIER_MAISON						RC_CUSTOM_KEYBOARD
#define VALEUR_CENTRALE_US						CENTER_VALUE_US

/*      Methodes en Francais                    English native methods */
#define RcSeq_DeclareManche						RcSeq_DeclareStick
#define RcSeq_DeclareClavier					RcSeq_DeclareKeyboard
#define RcSeq_DeclareClavierMaison				RcSeq_DeclareCustomKeyboard
#define RcSeq_DeclareCommandeEtSequence			RcSeq_DeclareCommandAndSequence
#define RcSeq_DeclareCommandeEtActionCourte		RcSeq_DeclareCommandAndShortAction
#define RcSeq_LanceSequence						RcSeq_LaunchSequence
#define RcSeq_LanceActionCourte					RcSeq_LaunchShortAction
#define RcSeq_Rafraichit						RcSeq_Refresh

#endif
