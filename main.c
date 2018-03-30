/*
	Baudrait detect by bitbang
	2018 Hiroki Mori
*/

#include <stdio.h>
#include <stdlib.h>
#include "../ftd2xx.h"

#define BUF_SIZE 0x2000

int main(int argc, char *argv[])
{
	char * pcBufRead;
	DWORD dwBytesRead;
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	int iport;
	int i;
	int lastbit, lastpos;
	int curbit, minlen, len;
	int lastbaud, curbaud;
	int baudcount;
	int div;
	
	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	}
	else {
		iport = 0;
	}
	
	ftStatus = FT_Open(iport, &ftHandle);
	if(ftStatus != FT_OK) {
		/* 
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed\n", iport);
		return 1;
	}

	pcBufRead = (char *)malloc(BUF_SIZE);
	FT_ResetDevice(ftHandle);
	FT_SetBaudRate(ftHandle, FT_BAUD_115200);
	FT_SetBitMode(ftHandle, 0xFE, 0x01);

	baudcount = 0;
	for (;;) {
		FT_Read(ftHandle, pcBufRead, BUF_SIZE, &dwBytesRead);

		lastpos = 0;
		minlen = 0;
		lastbit = pcBufRead[0] & 0x02 ? 1 : 0;
		for (i = 1; i < BUF_SIZE; ++i) {
			curbit = pcBufRead[i] & 0x02 ? 1 : 0;
			if (lastbit != curbit) {
				len = i - lastpos;
				if (lastpos != 0) {
					if (minlen == 0)
						minlen = len;
					else if(len < minlen)
						minlen = len;
				}
				lastpos = i;
				lastbit = curbit;
			}
		}
		div = (minlen + 5) / 20;
		if (div != 0) {
			curbaud = 115200 / div;
			if (curbaud == lastbaud) {
				++baudcount;
				if (baudcount > 3)
					break;
			}
			lastbaud = curbaud;
		}
	}

	printf("Detectied baudrate : %d\n", lastbaud);
		
	free(pcBufRead);
	FT_Close(ftHandle);
	
	return 0;
}
