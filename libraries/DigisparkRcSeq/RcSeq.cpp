#include "RcSeq.h"
/*
 English: by RC Navy (2012/2013)
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

 ASTUCE:
 ======

 Il est possible de declarer 8 sequences par manche (4 avec la voie du potentiometre vertical et 4 avec la voie du potentiometre horizontal).
 Il est possible de lancer 2 sequences en meme temps en utilisant les diagonales (la ou il y a des X dans la figure ci-dessous).

 POSITION MANCHE SUR EMETTEUR
   ,---------------------.                                \
   |  X       O       X  | --> RC_IMPULSION_NIVEAU_PLUS_2  |
   |          |          |                                 |
   |      X   O   X      | --> RC_IMPULSION_NIVEAU_PLUS_1  |
   |          |          |                                 /
   |  O---O---O---O---O  | --> Neutre (Aucune action)      >  4 sequences possibles avec le manche vertical
   |          |          |                                 \
   |      X   O   X      | --> RC_IMPULSION_NIVEAU_MOINS_1 |
   |          |          |                                 |
   | X        O       X  | --> RC_IMPULSION_NIVEAU_MOINS_2 |
   '---------------------'                                /
      |   |   |   |   |
      |   |   |   |   |                                   \
      |   |   |   |   '------> RC_IMPULSION_NIVEAU_PLUS_2  |
      |   |   |   |                                        |
      |   |   |   '----------> RC_IMPULSION_NIVEAU_PLUS_1  |
      |   |   |                                            /
      |   |   '--------------> Neutre (Aucune action)      >  4 sequences possibles avec le manche horizontal
      |   |                                                \
      |   '------------------> RC_IMPULSION_NIVEAU_MOINS_1 |   
      |                                                    | 
      '----------------------> RC_IMPULSION_NIVEAU_MOINS_2 |
                                                          /   
*/
/*************************************************************************
								MACROS
*************************************************************************/
/* For an easy Library Version Management */
#define LIB_VERSION			1
#define LIB_REVISION			0

#define STR(s)				#s
#define MAKE_TEXT_VER_REV(Ver,Rev)	(char*)(STR(Ver)"."STR(Rev))

#define LIB_TEXT_VERSION_REVISION	MAKE_TEXT_VER_REV(LIB_VERSION,LIB_REVISION) /* Make Full version as a string "Ver.Rev" */

/* A Set of Macros for bit manipulation */
#define SET_BIT(Value,BitIdx)		(Value)|= (1<<(BitIdx))
#define CLR_BIT(Value,BitIdx)		(Value)&=~(1<<(BitIdx))
#define TST_BIT(Value,BitIdx)		((Value)&(1<<(BitIdx)))

/* Servo refresh interval in ms (do not change this value, this one allows "round" values) */
#define REFRESH_INTERVAL_MS            20L

/* A pulse shall be valid during XXXX_PULSE_CHECK_MS before being taken into account */
#define STICK_PULSE_CHECK_MS           100L
#define KBD_PULSE_CHECK_MS            	10L

/* Duration between 2 consecutive commands */
#define INTER_CMD_DURATION_MS          1000L

/* Free servo Indicator */
#define NO_SEQ_LINE                    255

/* Free Position Indicator */
#define NO_POS                         255

/* The macro below computes how many refresh to perform while a duration in ms */
#define REFRESH_NB(DurationMs)         ((DurationMs)/REFRESH_INTERVAL_MS)

/* The motion goes from StartInDegrees to EndInDegrees and will take MotionDurationMs in ms */
#define STEP_IN_DEGREES_PER_REFRESH(StartInDegrees,EndInDegrees,MotionDurationMs) (EndInDegrees-StartInDegrees)/REFRESH_NB(MotionDurationMs)
/* A set of Macros to read an (u)int8_t (Byte), an (u)int16_t (Word) in  program memory (Flash memory) */
#define PGM_READ_8(FlashAddr)		pgm_read_byte(&(FlashAddr))
#define PGM_READ_16(FlashAddr)		pgm_read_word(&(FlashAddr))
#define PGM_READ_32(FlashAddr)		pgm_read_dword(&(FlashAddr))

/*
STICK TYPE:
==========
Pos 0     1      2     3
  |---|-|---|--|---|-|---|
1000us                 2000us (Typical Pulse Width values)
*/
#define ACTIVE_AREA_STEP_NBR           3
#define INACTIVE_AREA_STEP_NBR         1
#define TOTAL_STEP_NBR(KeyNb,Type)     ((Type==RC_CMD_STICK)?((KeyNb)*(ACTIVE_AREA_STEP_NBR+INACTIVE_AREA_STEP_NBR)):(((KeyNb)*(ACTIVE_AREA_STEP_NBR+INACTIVE_AREA_STEP_NBR))-1))
#define STEP(MinUs, MaxUs,KeyNb,Type)  ((MaxUs-MinUs)/TOTAL_STEP_NBR(KeyNb,Type))
#define KEY_MIN_VAL(Idx,Step)          ((ACTIVE_AREA_STEP_NBR+INACTIVE_AREA_STEP_NBR)*(Step)*(Idx))
#define KEY_MAX_VAL(Idx,Step)          (KEY_MIN_VAL(Idx,Step)+(ACTIVE_AREA_STEP_NBR*(Step)))

typedef struct {
  int8_t   InProgress;
  int8_t   CmdIdx;
  int8_t   Pos;
  uint32_t StartChronoMs;
  void    *TableOrShortAction;
  uint8_t  SequenceLength;
  uint8_t  ShortActionMap;
}CmdSequenceSt_t;

#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
typedef struct {
  int8_t   Idx;
  uint32_t StartChronoMs;
}PosST_t;

typedef struct {
  SoftRcPulseIn Pulse;
  PosST_t     Pos;
  uint8_t     Type; /* RC_CMD_STICK or RC_CMD_KEYBOARD or RC_CMD_CUSTOM */
  uint8_t     PosNb;
  uint16_t    PulseMinUs;
  uint16_t    PulseMaxUs;
  uint16_t    StepUs;
  KeyMap_t   *KeyMap;
}RcCmdSt_t;
#endif

typedef struct {
  SoftRcPulseOut Motor;
  uint16_t       RefreshNb;       /* Used to store the number of refresh to perform during a servo motion (if not 0 -> Motion in progress) */
  uint8_t        SeqLineInProgress;
}ServoSt_t;
/*************************************************************************
							GLOBAL VARIABLES
*************************************************************************/
static uint8_t SeqNb;
static uint8_t ServoNb;
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
static uint8_t CmdSignalNb;
static RcCmdSt_t          RcChannel[RC_CMD_MAX_NB];
#endif
#ifdef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
#define  AsMember 	   .
static ServoSt_t          Servo[SERVO_MAX_NB];
static CmdSequenceSt_t    CmdSequence[SEQUENCE_MAX_NB];
#else
#define  AsMember 	   ->
static ServoSt_t          **Servo=NULL;
static CmdSequenceSt_t    **CmdSequence=NULL;
#endif
/*************************************************************************
					PRIVATE FUNCTION PROTOTYPES
*************************************************************************/
static void   ExecuteSequence(uint8_t CmdIdx, uint8_t Pos);
#ifndef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
static void   LoadSequenceOrShortAction(uint8_t CmdIdx,uint8_t Pos,void *SequenceOrShortAction, uint8_t SequenceLength);
#endif
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
static int8_t GetPos(uint8_t ChIdx,uint16_t PulseWidthUs);
#endif

//========================================================================================================================
void RcSeq_Init(void)
{
	SeqNb=0;
	ServoNb=0;
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
	for(uint8_t ChIdx=0;ChIdx<RC_CMD_MAX_NB;ChIdx++)
	{
		RcChannel[ChIdx].Pos.Idx=NO_POS;
	}
#endif
#ifdef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
	for(uint8_t SeqIdx=0;SeqIdx<SEQUENCE_MAX_NB;SeqIdx++)
	{
		CmdSequence[SeqIdx].InProgress=0;
		CmdSequence[SeqIdx].TableOrShortAction=NULL;
		CmdSequence[SeqIdx].SequenceLength=0;
		CmdSequence[SeqIdx].ShortActionMap=0;
	}
#endif
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
	TinyPinChange_Init();
#endif
}
//========================================================================================================================
uint8_t RcSeq_LibVersion(void)
{
	return(LIB_VERSION);
}
//========================================================================================================================
uint8_t RcSeq_LibRevision(void)
{
	return(LIB_REVISION);
}
//========================================================================================================================
char *RcSeq_LibTextVersionRevision(void)
{
	return(LIB_TEXT_VERSION_REVISION);
}
//========================================================================================================================
void RcSeq_DeclareServo(uint8_t Idx, uint8_t DigitalPin)
{
#ifdef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
	if(Idx<SERVO_MAX_NB)
	{
		Servo[Idx].Motor.attach(DigitalPin);
		Servo[Idx].SeqLineInProgress=NO_SEQ_LINE;
		if(ServoNb<(Idx+1)) ServoNb=(Idx+1);
	}
#else
	if(Idx<SERVO_MAX_NB)
	{
		ServoNb++;
		if(!Servo) Servo=(ServoSt_t**)malloc(sizeof(ServoSt_t));
		else       Servo=(ServoSt_t**)realloc(Servo, sizeof(ServoSt_t)*ServoNb);
		Servo[Idx] AsMember Motor.attach(DigitalPin);
		Servo[Idx] AsMember SeqLineInProgress=NO_SEQ_LINE;
	}
#endif
}
//========================================================================================================================
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
void RcSeq_DeclareSignal(uint8_t Idx, uint8_t DigitalPin)
{
	if(Idx<RC_CMD_MAX_NB)
	{
		RcChannel[Idx].Pulse.attach(DigitalPin);
		CmdSignalNb++;
	}
}
//========================================================================================================================
void RcSeq_DeclareKeyboardOrStickOrCustom(uint8_t ChIdx, uint8_t Type, uint16_t PulseMinUs, uint16_t PulseMaxUs, KeyMap_t *KeyMap, uint8_t PosNb)
{
	RcChannel[ChIdx].Type = Type;
	RcChannel[ChIdx].PosNb = PosNb;
	RcChannel[ChIdx].PulseMinUs = PulseMinUs;
	RcChannel[ChIdx].PulseMaxUs = PulseMaxUs;
	RcChannel[ChIdx].StepUs = STEP(PulseMinUs, PulseMaxUs, PosNb, Type);
	RcChannel[ChIdx].KeyMap = KeyMap;
}
//========================================================================================================================
void RcSeq_DeclareCustomKeyboard(uint8_t ChIdx, KeyMap_t *KeyMapTbl, uint8_t KeyNb)
{
	RcSeq_DeclareKeyboardOrStickOrCustom(ChIdx, RC_CMD_CUSTOM, 0, 0, KeyMapTbl, KeyNb);
}
#endif
//========================================================================================================================
void RcSeq_DeclareCommandAndSequence(uint8_t CmdIdx,uint8_t Pos,SequenceSt_t *Table, uint8_t SequenceLength)
{
uint8_t  Idx, ServoIdx;
uint16_t StartInDegrees;
uint32_t StartMinMs[SERVO_MAX_NB];
#ifdef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
	for(Idx=0;Idx<SEQUENCE_MAX_NB;Idx++)
	{
		if(!CmdSequence[Idx].TableOrShortAction)
		{
			CmdSequence[Idx].CmdIdx=CmdIdx;
			CmdSequence[Idx].Pos=Pos;
			CmdSequence[Idx].TableOrShortAction=(void*)Table;
			CmdSequence[Idx].SequenceLength=SequenceLength;
			SeqNb++;
			break;
		}
	}
#else
	LoadSequenceOrShortAction(CmdIdx,Pos,(void*)Table, SequenceLength);
#endif

	/* Get initial pulse width for each Servo */
	for(Idx=0;Idx<SERVO_MAX_NB;Idx++)
	{
		StartMinMs[Idx]=0xFFFFFFFF;
	}
	for(Idx=0;Idx<SequenceLength;Idx++)
	{
		ServoIdx=(int8_t)PGM_READ_8(Table[Idx].ServoIndex);
		if(ServoIdx!=255)
		{
			if((uint32_t)PGM_READ_32(Table[Idx].StartMotionOffsetMs)<=StartMinMs[ServoIdx])
			{
				StartMinMs[ServoIdx]=(uint32_t)PGM_READ_32(Table[Idx].StartMotionOffsetMs);
				StartInDegrees=(uint16_t)PGM_READ_8(Table[Idx].StartInDegrees);
				Servo[ServoIdx] AsMember Motor.write(StartInDegrees);
			}
		}
	}
}
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
//========================================================================================================================
void RcSeq_DeclareCommandAndShortAction(uint8_t CmdIdx,uint8_t Pos,void(*ShortAction)(void))
{
#ifdef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
uint8_t Idx;
	for(Idx=0;Idx<SEQUENCE_MAX_NB;Idx++)
	{
		if(!CmdSequence[Idx].TableOrShortAction)
		{
			CmdSequence[Idx].CmdIdx=CmdIdx;
			CmdSequence[Idx].Pos=Pos;
			CmdSequence[Idx].TableOrShortAction=(void*)ShortAction;
			SeqNb++;
			break;
		}
	}
#else
	LoadSequenceOrShortAction(CmdIdx,Pos,(void*)ShortAction, 0);
#endif
}
#endif

#ifndef RC_SEQ_WITH_STATIC_MEM_ALLOC_SUPPORT
static void LoadSequenceOrShortAction(uint8_t CmdIdx,uint8_t Pos,void *SequenceOrShortAction, uint8_t SequenceLength)
{
	if(!CmdSequence) CmdSequence=(CmdSequenceSt_t**)malloc(sizeof(CmdSequenceSt_t));
	else             CmdSequence=(CmdSequenceSt_t**)realloc(CmdSequence,sizeof(CmdSequence)+sizeof(CmdSequenceSt_t));
	CmdSequence[SeqNb] AsMember CmdIdx=CmdIdx;
	CmdSequence[SeqNb] AsMember Pos=Pos;
	CmdSequence[SeqNb] AsMember TableOrShortAction=(void*)SequenceOrShortAction;
	CmdSequence[SeqNb] AsMember SequenceLength=SequenceLength;
	SeqNb++;
}
#endif
//========================================================================================================================
uint8_t RcSeq_LaunchSequence(SequenceSt_t *Table)
{
uint8_t Idx, Ret=0;
	for(Idx=0;Idx<SEQUENCE_MAX_NB;Idx++)
	{
		if(CmdSequence[Idx] AsMember TableOrShortAction==(void*)Table)
		{
			ExecuteSequence(CmdSequence[Idx] AsMember CmdIdx,CmdSequence[Idx] AsMember Pos);
			Ret=1;
			break;
		}
	}
	return(Ret);
}
//========================================================================================================================
void RcSeq_Refresh(void)
{
static uint32_t NowMs=millis();
static uint32_t StartChronoInterPulseMs=millis();
SequenceSt_t   *SequenceTable;
void           (*ShortAction)(void);
int8_t          ShortActionCnt;
uint8_t         ServoIdx;
uint32_t        MotionDurationMs, StartOfSeqMs, EndOfSeqMs, Pos;
uint16_t        StartInDegrees, EndInDegrees;

#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
uint8_t         ChIdx, CmdPos;
uint32_t        RcPulseWidthUs;

	/* Asynchronous RC Command acquisition */
	for(ChIdx=0;ChIdx<CmdSignalNb;ChIdx++)
	{
		if(!RcChannel[ChIdx].Pulse.available()) continue; /* Channel not used or no pulse received */
		RcPulseWidthUs=RcChannel[ChIdx].Pulse.width_us();
		CmdPos=GetPos(ChIdx,RcPulseWidthUs);
		if(CmdPos>=0)
		{
			if(RcChannel[ChIdx].Pos.Idx!=CmdPos)
			{
				  if((millis()-RcChannel[ChIdx].Pos.StartChronoMs)>=INTER_CMD_DURATION_MS) /* Check the last command was received for at least 1 second */
				  {
					  RcChannel[ChIdx].Pos.Idx=CmdPos;
					  RcChannel[ChIdx].Pos.StartChronoMs=millis();
				  }
			}	
			else
			{
				  if((millis()-RcChannel[ChIdx].Pos.StartChronoMs)>=((RcChannel[ChIdx].Type==RC_CMD_STICK)?STICK_PULSE_CHECK_MS:KBD_PULSE_CHECK_MS)) /* Check the Pulse is valid at least for 100 ms or 50 ms */
				  {
					  ExecuteSequence(ChIdx,CmdPos);
					  RcChannel[ChIdx].Pos.Idx=NO_POS;
				  }
			}   
		}
		else
		{
			RcChannel[ChIdx].Pos.Idx=NO_POS;
		}
	}
#endif
    NowMs=millis();
    if((NowMs - StartChronoInterPulseMs) >= 20L)
    {
		/* We arrive here every 20 ms */
		/* Asynchronous Servo Sequence management */
		for(int8_t Idx=0;Idx<SeqNb;Idx++)
		{
			if(!CmdSequence[Idx] AsMember InProgress || !CmdSequence[Idx] AsMember SequenceLength) continue;
			ShortActionCnt=-1;
			for(int8_t SeqLine=0;SeqLine<CmdSequence[Idx] AsMember SequenceLength;SeqLine++) /* Read all lines of the sequence table: this allows to run several servos simultaneously (not forcibly one after the other) */
			{
				SequenceTable=(SequenceSt_t *)CmdSequence[Idx] AsMember TableOrShortAction;
				ServoIdx=(int8_t)PGM_READ_8(SequenceTable[SeqLine].ServoIndex);
#ifdef RC_SEQ_WITH_SHORT_ACTION_SUPPORT
				if(ServoIdx==255) /* Not a Servo: it's a short Action to perform only if not already done */
				{
					ShortActionCnt++;
					StartOfSeqMs = CmdSequence[Idx] AsMember StartChronoMs + (int32_t)PGM_READ_32(SequenceTable[SeqLine].StartMotionOffsetMs);
					if( (NowMs>=StartOfSeqMs) && !TST_BIT(CmdSequence[Idx] AsMember ShortActionMap,ShortActionCnt) )
					{
						ShortAction=(void(*)(void))PGM_READ_16(SequenceTable[SeqLine].ShortAction);
						ShortAction();
						SET_BIT(CmdSequence[Idx] AsMember ShortActionMap,ShortActionCnt); /* Mark short Action as performed */
						/* If the last line contains an Action   AsMember  End of Sequence */
						if(SeqLine==(CmdSequence[Idx] AsMember SequenceLength-1))
						{
							CmdSequence[Idx] AsMember InProgress=0;
							CmdSequence[Idx] AsMember ShortActionMap=0; /* Mark all Short Action as not performed */
						}
					}
					continue;
				}
#endif
				if(Servo[ServoIdx] AsMember RefreshNb && SeqLine!=Servo[ServoIdx] AsMember SeqLineInProgress)
				{
					continue;
				}
				StartOfSeqMs = CmdSequence[Idx] AsMember StartChronoMs + (int32_t)PGM_READ_32(SequenceTable[SeqLine].StartMotionOffsetMs);
				MotionDurationMs = (int32_t)PGM_READ_32(SequenceTable[SeqLine].MotionDurationMs);
				EndOfSeqMs = StartOfSeqMs + MotionDurationMs;
				if(!Servo[ServoIdx] AsMember RefreshNb && Servo[ServoIdx] AsMember SeqLineInProgress==NO_SEQ_LINE)
				{
					if( (NowMs>=StartOfSeqMs) && (NowMs<=EndOfSeqMs) )
					{
						Servo[ServoIdx] AsMember SeqLineInProgress=SeqLine;
						StartInDegrees=(uint16_t)PGM_READ_8(SequenceTable[SeqLine].StartInDegrees);
						Servo[ServoIdx] AsMember RefreshNb=REFRESH_NB(MotionDurationMs);
						Servo[ServoIdx] AsMember Motor.write(StartInDegrees);
					}
				}
				else
				{
					/* A sequence line is in progress: update the next position */
					if(Servo[ServoIdx] AsMember RefreshNb) Servo[ServoIdx] AsMember RefreshNb--;
					StartInDegrees=(uint16_t)PGM_READ_8(SequenceTable[SeqLine].StartInDegrees);
					EndInDegrees=(uint16_t)PGM_READ_8(SequenceTable[SeqLine].EndInDegrees);
					Pos=(int32_t)EndInDegrees-((int32_t)Servo[ServoIdx] AsMember RefreshNb*STEP_IN_DEGREES_PER_REFRESH((int32_t)StartInDegrees,(int32_t)EndInDegrees,(int32_t)MotionDurationMs)); //For refresh max nb, Pos = StartInDegrees
					Servo[ServoIdx] AsMember Motor.write(Pos);
					if( !Servo[ServoIdx] AsMember RefreshNb )
					{
						Servo[ServoIdx] AsMember SeqLineInProgress=NO_SEQ_LINE;
						/* Last servo motion and refresh = 0  ->  End of Sequence */
						if(SeqLine==(CmdSequence[Idx] AsMember SequenceLength-1))
						{
							CmdSequence[Idx] AsMember InProgress=0;
							CmdSequence[Idx] AsMember ShortActionMap=0; /* Mark all Short Action as not performed */
						}
					}
				}
			}
		}
		SoftRcPulseOut::refresh(1); /* Force Refresh */
		StartChronoInterPulseMs=millis();
    }
}

//========================================================================================================================
// PRIVATE FUNCTIONS
//========================================================================================================================
static void ExecuteSequence(uint8_t CmdIdx, uint8_t Pos)
{
void(*ShortAction)(void);
uint8_t Idx;

	for(Idx=0;Idx<SeqNb;Idx++)
	{
		if((CmdSequence[Idx] AsMember CmdIdx==CmdIdx) && (CmdSequence[Idx] AsMember Pos==Pos))
		{
#ifdef RC_SEQ_WITH_SHORT_ACTION_SUPPORT
			if(CmdSequence[Idx] AsMember TableOrShortAction && !CmdSequence[Idx] AsMember SequenceLength)
			{
				/* It's a short action */
				ShortAction=(void(*)(void))CmdSequence[Idx] AsMember TableOrShortAction;
				ShortAction();
			}
			else
#endif
			{
				/* It's a Table of Sequence */
				if(!CmdSequence[Idx] AsMember InProgress)
				{
					CmdSequence[Idx] AsMember InProgress=1;
					CmdSequence[Idx] AsMember StartChronoMs=millis();
				}
			}
			break;
		}
	}
}
//========================================================================================================================
#ifdef RC_SEQ_WITH_SOFT_RC_PULSE_IN_SUPPORT
static int8_t GetPos(uint8_t ChIdx,uint16_t PulseWidthUs)
{
int8_t  Idx, Ret=-1;
uint16_t PulseMinUs,PulseMaxUs;

    for(Idx=0;Idx<RcChannel[ChIdx].PosNb;Idx++)
    {
        switch(RcChannel[ChIdx].Type)
        {            
	    case  RC_CMD_STICK: /* No break: normal */
            case  RC_CMD_KEYBOARD:
            if(Idx<(RcChannel[ChIdx].PosNb/2))
            {
                PulseMinUs=RcChannel[ChIdx].PulseMinUs+KEY_MIN_VAL(Idx,RcChannel[ChIdx].StepUs);
                PulseMaxUs=RcChannel[ChIdx].PulseMinUs+KEY_MAX_VAL(Idx,RcChannel[ChIdx].StepUs);
            }
            else
            {
                PulseMinUs=RcChannel[ChIdx].PulseMaxUs-KEY_MAX_VAL(RcChannel[ChIdx].PosNb-1-Idx,RcChannel[ChIdx].StepUs);
                PulseMaxUs=RcChannel[ChIdx].PulseMaxUs-KEY_MIN_VAL(RcChannel[ChIdx].PosNb-1-Idx,RcChannel[ChIdx].StepUs);
            }
            break;
			
	    case RC_CMD_CUSTOM:
	    PulseMinUs=(uint16_t)PGM_READ_16(RcChannel[ChIdx].KeyMap[Idx].Min);
	    PulseMaxUs=(uint16_t)PGM_READ_16(RcChannel[ChIdx].KeyMap[Idx].Max);
	    break;
        }
        if((PulseWidthUs>=PulseMinUs) && (PulseWidthUs<=PulseMaxUs))
        {
            Ret=Idx;
            break;
        }
    }
    return(Ret);
}
#endif
//========================================================================================================================
