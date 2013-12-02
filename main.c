#include <stdio.h>
#include <stdlib.h>


unsigned char VReg[0xF];            // 15 Status registers used in the Chip8...
unsigned char ControlReg[0xF];
unsigned char ROM[0xFFF];           // Program Space
unsigned char Stack[0x48];          // For Stack Simulation
unsigned char Graphics[32][64];    // Use this for graphics debugging...
unsigned long PC;                   // Program Counter
unsigned long SP;                   // Stack Pointer
unsigned long I;                    // Address Register
void CPU(FILE *);                     // This function is used to calculate the varou instructions....
void GraphicsUpdate(void);
unsigned long POP(void);
void Push(unsigned long);
void DrawSprite(unsigned char, unsigned char, unsigned char,FILE*);
char KeysPressed;
unsigned char DT;
unsigned char ST;
unsigned char Sprites[15][5] =
    {
          {0xF0,0x90,0x90,0x90,0xF0},   //0
          {0x20,0x60,0x20,0x20,0x70},   //1
          {0xF0,0x10,0xf0,0x80,0xf0},   //2
          {0xF0,0x10,0xf0,0x10,0xF0},   //3
          {0x90,0x90,0xf0,0x10,0x10},   //4
          {0xF0,0x80,0xf0,0x10,0xF0},   //5
          {0xF0,0x80,0xf0,0x90,0xF0},   //6
          {0xF0,0x20,0x20,0x40,0x40},   //7
          {0xF0,0x90,0xf0,0x90,0xF0},   //8
          {0xF0,0x90,0xf0,0x10,0xF0},   //9
          {0xF0,0x90,0xf0,0x90,0x90},   //A
          {0xe0,0x90,0xe0,0x90,0xe0},   //B
          {0xF0,0x80,0x80,0x80,0xF0},   //C
          {0xe0,0x90,0x90,0x90,0xe0},   //D
          {0xF0,0x80,0xF0,0x80,0xF0},   //E
          {0xF0,0x80,0xF0,0x80,0x80},   //F
    };

unsigned long POP() // Retreives the latest item from the stack
{
    return Stack[0];
}

void Push(unsigned long value)
{
    Stack[SP] = value;      // Add the value to the stack
    SP++;                   // Incremnt the stack pointer
}

void GraphicsUpdate()
{
    system("cls");
    int x,y;
    for(y=0;y<32;y++)
    {
        for(x=0;x<64;x++)
        {
            if(Graphics[y][x]==0x01)
            {
                printf("1");
            }else
            {
            printf("0");
            }
        }
        printf("\n");
    }


}



FILE *f;
void DrawSprite(unsigned char x,unsigned char y, unsigned char sprite, FILE *LOG)
{
    unsigned char DrawCount;
    for(DrawCount = 8;DrawCount>0;DrawCount--)
    {
        if(Graphics[y][x+DrawCount]==0x01&&(sprite&0x01)==0x01) VReg[0x0F]=0x01;

        if((x+DrawCount)>63) x=0;


        Graphics[y][x+DrawCount] ^=(sprite&0x01);

        fprintf(LOG,"\n%s %02X - %02X - %02X","Y-X-Bit: ",y,(x+DrawCount),(sprite&0x01));
        sprite = sprite>>1;
    }
}
//nms886@gmail.com

int main()
{



//fclose(LOG);

char filename[0xff];
unsigned char buffer[0xDFF];
unsigned char n;
printf("Which rom will we be loading?\n");
scanf("%s",filename);

f = fopen(filename, "rb");

if (f)
{
    n = fread(buffer, 0xDFF, 1, f);
}
else
{
    printf("ERROR");
}
FILE *LOG = fopen("Chip8Log.txt","w");
if(LOG == NULL)
{
    printf("Couldn't open log File R-Tard");

}
const char *header = "Chip 8 Debugging Log File";
fprintf(LOG,"%s\n",header);
fprintf(LOG,"%s\n",filename);

int i;


// initializing graphics
int j,k;
for(j=0;j<32;j++)
{
    for(k=0;k<64;k++)
    {
        Graphics[j][k] = 0x00;
    }
}

for(i=0;i<0xDFF;i++)
{
    ROM[0x200+i] = buffer[i];
}
    int x1,y;
    unsigned char x;
    unsigned char StartLoc = 0x100;
    for(x = 0x0;x<=0xF;x++)
    {
        unsigned char y;
        for(y=0;y<5;y++)
        {
            ROM[StartLoc] = Sprites[x][y];
            StartLoc++;
         }
    }
    PC = 0x200;
    srand(time(NULL));
    // Initialize Stack to zer
    SP = 0x00;

int po = 0;
while(1)
{
    CPU(LOG);
    PC+=2;
    if(DT>0) DT-=5;
    if(ST>0) ST-=5;
    po++;
}
 fclose(LOG);
return 0;
}

void ClearDisplay()
{
     system("cls");
    int x,y;
    for(y=0;y<32;y++)
    {
        for(x=0;x<64;x++)
        {
            Graphics[y][x]==0x00;

            printf("O");

        }
        printf("\n");
    }
}

void CPU(FILE *LOG)
{


    unsigned char NibbleOne = (ROM[PC]&0xF0)>>4;
    unsigned char NibbleTwo = (ROM[PC]&0x0F);
    unsigned char NibbleThree = (ROM[PC+1]&0xF0)>>4;
    unsigned char NibbleFour = (ROM[PC+1]&0x0F);
    unsigned char r = rand();
    long Temp;
    fprintf(LOG,"\n%s %02X %02X\n","On Instruction ",ROM[PC],ROM[PC+1]);

    switch(NibbleOne)
    {
    case 0x00:
        {

            switch(NibbleFour)
            {
            case 0:
              //  ClearDisplay();
         //     fprintf(LOG,"%s\n","IN CLEAR DISPLAY");
            break;
            case 0x0e:

                PC = (POP()-1);
           //     fprintf(LOG,"%s","Popping From the Stack");

                break;
            }
        }
        break;
    case 0x01: // Jumps to address NNN, 0x1NNN
        PC = (NibbleTwo<<8)+ROM[PC+1]-2;
      //   fprintf(LOG,"%s %4X \n","Jumping to Address...will be off by -2",PC);
        break;
    case 0x02:
        PC+=2;
        Push(PC); // Store our current address on the Stack
        PC = (NibbleTwo<<8)+ROM[PC+1];
     //   fprintf(LOG,"%s %4X", "Pushing the PC onto the stack, stack now holds: ",PC);
        break;
    case 0x03: //3XNN	Skips the next instruction if VX equals NN
        if(VReg[NibbleTwo]==ROM[PC+1]) PC+=2;

     //   fprintf(LOG,"%s %4X","3XNN Will Skip Next instruction if Vx == NN, NN: ",ROM[PC+1]);
        break;
    case 0x04:
        if(VReg[NibbleTwo]!=ROM[PC+1]) PC+=2;

    //    fprintf(LOG,"%s %4X","4XNN Will Skip Next instruction if Vx != NN, NN: ",ROM[PC+1]);
        break;
    case 0x05:
        if(VReg[NibbleTwo]==VReg[NibbleThree]) PC+=2;

    //    fprintf(LOG,"%s %4X","5XYN Will Skip Next instruction if Vx = Vy, NN: ",ROM[PC+1]);
        break;
    case 0x06:
        VReg[NibbleTwo] = ROM[PC+1];

     //    fprintf(LOG,"%s ","6XNN Sets Vx = NN: ");
        break;
    case 0x07:
        VReg[NibbleTwo]+=ROM[PC+1];

      //   fprintf(LOG,"%s ","7XNN Sets Vx += NN: ");
        break;
    case 0x08:
        switch(NibbleFour)
        {
            case 0x00:
                {
                VReg[NibbleTwo]=VReg[NibbleThree];
             //   fprintf(LOG,"%s","Sets Vx = Vy");
                break;
            case 0x01:
                VReg[NibbleTwo]|=VReg[NibbleThree];
             //   fprintf(LOG,"%s ","Sets Vx |= Vy, ");
                break;
            case 0x02:
                VReg[NibbleTwo]&=VReg[NibbleThree];
            //    fprintf(LOG,"%s ","Sets Vx &= Vy");
                break;
            case 0x03:
                VReg[NibbleTwo]^=VReg[NibbleThree];
              //  fprintf(LOG,"%s ","Sets Vx ^= Vy");
                break;
            case 0x04:
                 Temp = VReg[NibbleTwo]+VReg[NibbleThree];
                if(Temp>255)
                {
                    VReg[0x0F]=1;
                }else
                {
                    VReg[0x0F]=0;
                }
                VReg[NibbleTwo]+=VReg[NibbleThree];
              //  fprintf(LOG,"%s ","Adds Vx and Vy, if >255 then V[f]==1, else V[f]==0");
                break;
            case 0x05:
                if(VReg[NibbleTwo]>VReg[NibbleThree])
                {
                    VReg[0xF]=1;
                }
                else
                    VReg[0xF]=0;
                VReg[NibbleTwo]-=VReg[NibbleThree];
             //   fprintf(LOG,"%s"," 8XY5: Subtracts Vx from Vy, if Vx>Vy then V[f]==1, else V[f]==0");
                break;
            case 0x06:
                if((VReg[NibbleTwo]&0x01)==1){ VReg[0xF]=1; }
                else { VReg[NibbleTwo]=0; }
                VReg[NibbleTwo] = VReg[NibbleTwo]>>1;
               // fprintf(LOG,"%s"," 8XY6: If the LS bit of Vx is 1, Vf is 1, otherwise, 0, Vx = Vx/2");
                break;
            case 0x07:
                if(VReg[NibbleTwo]<VReg[NibbleThree])
                {
                    VReg[0xF]=1;
                }
                else
                    VReg[0xF]=0;
                 VReg[NibbleTwo] = VReg[NibbleThree]-VReg[NibbleTwo];
                // fprintf(LOG,"%s"," 8XY7: If Vy>Vx then Vf = 1, else Vf = 0; Vx-=Vy");
                break;
            case 0x08:
                if((VReg[NibbleTwo]&0xEF)==0x80){ VReg[0xF]=1; }
                else { VReg[NibbleTwo]=0; }
                VReg[NibbleTwo] = VReg[NibbleTwo]<<1;
               // fprintf(LOG,"%s","8XY8: If MS bit of Vx = 1, then Vf =1, else Vf =0, Vx*=2");
                }
                break;
        }
        break;
    case 0x09:
        if(VReg[NibbleTwo]!=VReg[NibbleThree]) PC+=2;
     //   fprintf(LOG,"%s","9XY0: If Vx!=Vy then PC+=2");
        break;
    case 0x0A:
        I = (NibbleTwo<<8)+ROM[PC+1];
     //   fprintf(LOG,"%s","ANNN: I=NNN");
        break;
    case 0x0B:
        PC = (NibbleTwo<<8)+ROM[PC+1]+VReg[0];
      //  fprintf(LOG,"%s","BIII: Jump to Location III");
        break;
    case 0x0C:

        VReg[NibbleTwo] = r & ROM[PC+1];
     //    fprintf(LOG,"%s %4X","CXKK: Vx = Vx&Random, Random = ",r);
        break;
    case 0x0D:
        {
            unsigned char n;
            unsigned char Xcor = VReg[NibbleTwo];
            unsigned char Ycor = VReg[NibbleThree];
            for(n=0;n<NibbleFour;n++)
            {
              //  fprintf(LOG,"%s %4X","Drawing sprite: ",ROM[I+n]);
              //  fprintf(LOG,"%s","\n");
                fprintf(LOG,"\n%s %02X - %02X - %02X","X - Y - SPRITE ",Xcor,(Ycor+n),ROM[I+n]);
                DrawSprite(Xcor,(Ycor+n),ROM[I+n],LOG);

            }
            GraphicsUpdate();
        }
        break;
    case 0x0E:
        {
            switch(NibbleThree)
            {
            case 0x09:
                if(ControlReg[NibbleTwo]==1)
                    PC+=2;
                break;
            case 0x0A:
                if(ControlReg[NibbleTwo]!=1)
                    PC+=2;
                break;
            }
        }
        break;
            case 0x0F:
                {
                    switch(NibbleFour)
                    {
                    case 0x07:
                        VReg[NibbleTwo] = DT;
                        break;
                    case 0x0A:
                        KeysPressed = 0;
                        while(!KeysPressed) // wait for a keypress
                        {
                            // Call Check Keys Function
                        }
                        int t;
                        for( t= 0;t<0xF;t++) // find out which button was pressed
                        {
                            if(ControlReg[t]==1)
                            {
                                VReg[NibbleTwo]=t;
                                break;
                            }
                        }
                        break;
                    case 0x05:
                        switch(NibbleThree)
                        {
                        case 0x01:
                            DT = VReg[NibbleTwo];
                            break;
                        case 0x05:
                            {
                            int t;
                            for(t=I;t<=I+NibbleTwo;t++)
                            {
                                ROM[t]=VReg[t-I];
                            }
                            }
                            break;
                        case 0x06:
                            {
                              int t;
                            for(t=I;t<=I+NibbleTwo;t++)
                            {
                                VReg[t-I]=ROM[t];
                            }
                            }
                            break;

                        }

                        break;
                    case 0x08:
                        ST = VReg[NibbleTwo];
                        break;
                    case 0x0E:
                        I+=VReg[NibbleTwo];
                        break;
                    case 0x09:
                        // All prites start at 0x100 in ROM
                        I = 0x100+(VReg[NibbleTwo]*5);
                        break;
                    case 0x03:
                        {
                            unsigned int temp = (int)VReg[NibbleTwo];
                            int t;
                            unsigned int rem;
                            unsigned char y;
                            for(t = 0;t<3;t++)
                            {
                                rem = temp%10;
                                y = (char)rem;
                                temp = temp/10;
                                ROM[I+2-t]=y;
                            }

                        }
                        break;


                    }
                }






    }
     fprintf(LOG,"\n%s%2X ","V0-",VReg[0]);
     fprintf(LOG,"%s%2X ","V1-",VReg[1]);
     fprintf(LOG,"%s%2X ","V2-",VReg[2]);
     fprintf(LOG,"%s%2X ","V3-",VReg[3]);
     fprintf(LOG,"%s%2X ","V4-",VReg[4]);
     fprintf(LOG,"%s%2X ","V5-",VReg[5]);
     fprintf(LOG,"%s%2X ","V6-",VReg[6]);
     fprintf(LOG,"%s%2X ","V7-",VReg[7]);
     fprintf(LOG,"%s%2X ","V8-",VReg[8]);
     fprintf(LOG,"%s%2X ","V9-",VReg[9]);
     fprintf(LOG,"%s%2X ","VA-",VReg[10]);
     fprintf(LOG,"%s%2X ","VB-",VReg[11]);
     fprintf(LOG,"%s%2X ","VC-",VReg[12]);
     fprintf(LOG,"%s%2X ","VD-",VReg[13]);
     fprintf(LOG,"%s%2X ","VE-",VReg[14]);
     fprintf(LOG,"%s%2X \n","VF-",VReg[15]);
     fprintf(LOG,"%s%8X \n","PC-",PC);
     fprintf(LOG,"%s%8X \n","I-",I);

}




void getInput()
{
    char in = getchar();
 while(in == NULL)
 {
     // Wait for keypress
 }


}

