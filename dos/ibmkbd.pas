{$M $800,0,0 }   { 2K stack, no heap }
Program Ibmkbd;
  Uses Dos;
  Const
      ESC		= 1;

      F1		= 59;
      F2		= 60;
      F3		= 61;
      F4		= 62;
      F5		= 63;
      F6		= 64;
      F7		= 65;
      F8		= 66;
      F9		= 67;
      F10		= 68;
      F11		= 87;
      F12		= 89;

      PRSCR		= 1;
      PGUP		= 73;
        
      BCKSLSH		= 43;
      ALTL		= 56;
      ALTR		= 56;
      CTRLL		= 29;
      CTRLR		= 29;
        
      TILDA		= 41;
        
      SCRLCK		= 70;
      PAUSE		= 69;
      UP		= 72;
      DEL		= 83;
      ENDK		= 79;
      INS		= 82;

      RIGHT		= 77;
      PGDN		= 81;
      HOME		= 71;

      CPSLCK		= 58;
      DOWN		= 80;
      LEFT		= 75;
      NUMLCK		= 69;
      SLASH		= 53;
      STAR		= 55;
      MINUS		= 74;
      PLUS		= 78;
      ENTER		= 28;


IBM_keys: array[0..127] of byte = (
  0, NUMLCK, 0, 0, 0, 0, 0, 0,				{ 0x00-0x07 }
  0, 0, 0, 0, 0, 0, 0, 0,				{ 0x08-0x0f }
  0, 0, 0, 0, 0, 0, 0, 0,				{ 0x10-0x17 }
  0, 0, 0, 0, 0, CPSLCK, 0, 0,				{ 0x18-0x1f }
  0, 0, 0, 0, 0, 0, 0, 0,				{ 0x20-0x27 }
  0, 0, 0, 0, 0, 0, 0, 0,				{ 0x28-0x2f }

  0, 0, 0, 0, 0, 0, 0, PLUS,				{ 0x30-0x37 }
  CTRLL, 0, CTRLR, 0, 0, 0, 0, 0,			{ 0x38-0x3f }
  0, 0, 0, 0, 0, SLASH, STAR, 0,			{ 0x40-0x47 }
  0, 0, 0, 0, 0, 0, ENTER, 0,				{ 0x48-0x4f }
  0, 0, 0, 0, MINUS, DOWN, LEFT, 0,			{ 0x50-0x57 }
  F1, F2, F3, F4, F5, F6, F7, F8,			{ 0x58-0x5f }

  F9, F10, F11, F12, ESC, 0, 0, 0,			{ 0x60-0x67 }
  0, 0, 0, 0, 0, 0, PRSCR, PGUP,			{ 0x68-0x6f }
  BCKSLSH, ALTL, ALTR, 0, TILDA, 0, SCRLCK, PAUSE,	{ 0x70-0x77 }
  UP, DEL, ENDK, INS, 0, RIGHT, PGDN, HOME		{ 0x78-0x7f }
);

{********************************************************}
Var 
  SaveOldVec:procedure;
  Control:Byte;
{********************************************************}

{--INTERRUPT-----------------------------------------}
   {$F+}
  Procedure MyInt_9;InterRupt;
   Begin
   Control:=Port[$60];

   If (Control<128) and (IBM_keys[Control]<>0) then
	Port[$60]:=IBM_keys[Control];

      Inline($9C);
      saveOldVec;
   End;
   {$F-}
{--Z-----------------------------------------------------------}
Begin
 writeln('ibmkbd.exe: Trying to hook onto interrupt handler...');
    GetIntVec($9,@SaveOldVec);
    SetIntVec($9,Addr(MyInt_9));
 writeln('ibmkbd.exe: Yurix IBM_E57888 keyboard support initialized.');
 keep(0);
End.