#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>

#include <3ds.h>

#include "drawing.h"

#define NUM_OF_BUTTONS 0x9
#define REC_SIZE 0x400

static jmp_buf exitJmp;
u32 *irmemloc;
Result resultStartup = 0xffffffff; //error or not on start
Result resultSetBit = 0xffffffff;	//error or not of setting the bit rate of the IR port
Result resultTransferIR = 0xffffffff; //error or not of getting bits from the IR port.
Result resultGetStatus = 0xffffffff; //error or not of getting the transfer state of the IR port. 
u8 StatusIR; //return of the status of IR.
u32 TransIR = 0;
u32 *CountIR = &TransIR; //Transfer count received. Help, I have no idea how pointers wooooooooorrrrrkkkk.
u8 *bot;
u8 *top;

int bitrate = 3;

bool rec = true;

u32 *recordedIR = NULL;
u32 *recentIR = NULL;
bool hasDataIR[NUM_OF_BUTTONS] = {false, false, false, false, false, false, false, false, false}; //probably inits to false...



void printMemory(void *ptr, int size, int row, bool save) //This is probably very bad but whatever
{ 
	unsigned char *c;
	if(save) {
		c = (unsigned char *)ptr;
		memcpy(recentIR, ptr, size);
	} else {
		c = (unsigned char *)recentIR;
	}
	int i = 0; //goes through all memory
	int colu = 0; //goes through the colums in desp
	int rowu = 0; //goes through the row in desp
	int lastzero = 0; //the last zero before a lot of zeros.
	while(i  != size){
		int mem = (int)c[i];
        if(mem != 0) {
			lastzero = i;
		}
		i++;
	}  
	i = 0;
    while(i  != lastzero + 1){
		int mem = (int)c[i];
		drawString(bot, 10 + (colu*18), row + (rowu * 10), "%02x ", mem); 
		if(colu>15) {
			colu = -1;
			rowu++;
		}
		colu++;
		i++;
	}  
	if(lastzero == 0) {
		drawString(bot, 10, 20, "I didn't get anything."); 
	}
}

void ir(char in) {
	if(rec) {
		resultGetStatus = irucmd_GetTransferState(&StatusIR);
		if(StatusIR == 1) {
			if(in == 'A') {
				u8 *blast = calloc(REC_SIZE, 0x1); //Mem to be copied
				u8 *zeros = calloc(REC_SIZE, 0x1); //reset mem, JUST IN CASE! (ya never know, man)
				drawString(top, 10, 100, "Downloading...");
				resultTransferIR = IRU_RecvData(blast, REC_SIZE, 0x0, CountIR, 0x1);
				memcpy(recordedIR + (REC_SIZE * 0),zeros, REC_SIZE); //Copy the zeros to replace the section
				memcpy(recordedIR + (REC_SIZE * 0),blast, TransIR); //Copy only what was got.
				printMemory(recordedIR + (REC_SIZE * 0),REC_SIZE,10, true); //might be dangerous?
				hasDataIR[0] = true;
				free(blast);
				free(zeros);
			}
		}else {
			drawString(top, 10, 100, "IR is busy.");
		}
	} else {
		resultGetStatus = irucmd_GetTransferState(&StatusIR);
		if(StatusIR == 1) {
			if(in == 'A'){
				if(hasDataIR[0]) {
					u8 *blast = calloc(REC_SIZE,0x1); //reset mem, JUST IN CASE!
					blast = memcpy(blast, recordedIR + (REC_SIZE * 0), REC_SIZE);
					resultTransferIR = IRU_SendData(blast, REC_SIZE, 0x0); //Just send the got darn data.
					printMemory(blast,REC_SIZE,10, true); //might be dangerous?
					free(blast);
				} else {
					drawString(top, 10, 100, "Button has no data!");
				}
			}
		} else {
			drawString(top, 10, 100, "IR is busy.");
		}
	}
}

int main(void) {
	irmemloc = (u32*) memalign(0x1000, 0x1000);
	resultStartup = IRU_Initialize(irmemloc, 0x1000);
	resultSetBit = IRU_SetBitRate(0xB);
	recordedIR = (u32*) malloc(REC_SIZE * NUM_OF_BUTTONS); //Currently there are 11(NUM_OF_BUTTONS) recordable buttons. (start switches mode)
	recentIR = (u32*) calloc(REC_SIZE, 0x1); //Whatever was last viewed in memory
	resultGetStatus = irucmd_GetTransferState(&StatusIR);
	srvInit();        // services
	aptInit();        // applets
	hidInit(NULL);    // input
	gfxInitDefault(); // gfx
	
	gfxSetDoubleBuffering(GFX_TOP, true);
	gfxSetDoubleBuffering(GFX_BOTTOM, true);
	bot = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	
	if(setjmp(exitJmp)) goto exit;
	
	clearScreen();
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	bool startToggle = false;
	bool upToggle = false;
	bool downToggle = false;
	
	while(aptMainLoop()) {
		bot = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
		top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		hidScanInput();
		irrstScanInput();
		
		u32 kHeld = hidKeysHeld();
		circlePosition circlePad;
		circlePosition cStick;
		hidCstickRead(&cStick);
		hidCircleRead(&circlePad);
		touchPosition touch;
		touchRead(&touch);
		
		clearScreen();
		
		char keys[30] = "ABXY URDL SEST LR ZLZR";
		
		if(kHeld & KEY_A) {
			keys[0] = '*';
			ir('A');
		}
		if(kHeld & KEY_B) {
			keys[1] = '*';
			ir('B');
		}
		if(kHeld & KEY_X) {
			keys[2] = '*';
			ir('X');
		}
		if(kHeld & KEY_Y) {
			keys[3] = '*';
			ir('Y');
		}
		if(kHeld & KEY_DUP) {
			keys[5] = '*';
			if(upToggle) {
				if(bitrate < 18) {
					bitrate++;
					resultSetBit = IRU_SetBitRate(bitrate);
				}
				upToggle = false;
			}
		} else {
			upToggle = true;
		}
		if(kHeld & KEY_DRIGHT) {
			keys[6] = '*';
			ir('R');
		}
		if(kHeld & KEY_DDOWN) {
			keys[7] = '*';
			if(downToggle) {
				if(bitrate > 3) {
					bitrate--;
					resultSetBit = IRU_SetBitRate(bitrate);
				}
				downToggle = false;
			}
		} else {
			downToggle = true;
		}
		if(kHeld & KEY_DLEFT) {
			keys[8] = '*';
			ir('L');
		}
		if(kHeld & KEY_SELECT) {
			keys[10] = '*';
			keys[11] = '*';
			ir('S');
		}
		if(kHeld & KEY_START) {
			keys[12] = '*';
			keys[13] = '*';
			if(startToggle) {
				rec = !rec;
				startToggle = false;
			}
		} else {
			startToggle = true;
		}
		if(kHeld & KEY_L) {
			keys[15] = '*';
			ir('L');
		}
		if(kHeld & KEY_R) {
			keys[16] = '*';
			ir('R');
		}
		if(kHeld & KEY_ZL) {
			keys[18] = '*';
			keys[19] = '*';
			ir('1');
		}
		if(kHeld & KEY_ZR) {
			keys[20] = '*';
			keys[21] = '*';
			ir('2');
		}
		drawString(top, 10, 10, keys);
		drawString(top, 10, 20, "Circle Pad   x: %04+d, y: %04+d", circlePad.dx, circlePad.dy);
		drawString(top, 10, 30, "Touch        x: %04d, y: %04d", touch.px, touch.py );
		if(resultStartup == 0) {
			drawString(top, 10, 40, "IR started!");
		} else {
			drawString(top, 10, 40, "IR Init     |Error: %x", resultStartup);
		}
		if(resultSetBit == 0) {
			drawString(top, 10, 50, "IR bit rate works!");
		} else {
			drawString(top, 10, 50, "IR bit rate |Error: %x", resultSetBit);
		}
		if(resultTransferIR == 0) {
			drawString(top, 10, 60, "IR transfer works!");
		} else {
			drawString(top, 10, 60, "IR transfer |Error: %x", resultTransferIR);
		}
		irucmd_GetTransferState(&StatusIR);
		drawString(top, 10, 70, "IR mode: %x", StatusIR);
		drawString(top, 10, 80, "Got %d bytes", TransIR);
		drawString(top, 10, 90, "Bitrate: %d (see IRU:SetBitRate)", bitrate);
		printMemory(NULL,REC_SIZE,10, false); //might be dangerous?
		
		if(rec) {
			drawString(top, 10, 210, "Recording mode active.");
		}else{
			drawString(top, 10, 210, "Sending mode active.");
		}
		drawString(top, 10, 220, "Start + Select to exit.");
		
		if((kHeld & KEY_START) && (kHeld & KEY_SELECT)) longjmp(exitJmp, 1);
		gfxFlushBuffers();
		gspWaitForVBlank();
		gfxSwapBuffers();
	}
	
	exit: //I should really be fixing these.
	
	free(recordedIR); //Crashes the program. Should free on release.
	free(irmemloc); //Crashes the program. Should free on release.
	free(recentIR); //Crashes the program. Should free on release.
	
	IRU_Shutdown(); //Crashes the program. Should free on release.
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	
	return 0;
}
