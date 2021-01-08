#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "mat.h"
#include "mex.h"
#include "main.h"
#include "matrix.h"
#include "direct.h"
#include "LTE_NR.h"
#include "LTENR_PHY.h"

Engine* ep;
mxArray* out;

/*Pass LTE NR Network parameters to MATLAB
	Grid Length (meters)
	Step Length (meters)
	UE Height (meters)
	Output filepath
	gNB Name
	gNB X,Y,Z coordinates
	gNB Height
	gNB frequency (Hz) (Upper+Lower)/2 1MHz = 1e+6
	gNB Tx Power (Watts) - P(W)= (1W x 10^(dbm/10))/1000
	*/
char command[BUFSIZ] = "";
int grid_length = 0;
int step_length = 5; //default 5 meter
int gNB_Count = 0;
int* gNB_Id;

double fn_netsim_matlab_init()
{
	/*
	 * Start the MATLAB engine Process
	 */
	fprintf(stderr, "\nPress any key to start MATLAB...\n");
	_getch();
	if (!(ep = engOpen(NULL))) {
		MessageBox((HWND)NULL, (LPCWSTR)"Can't start MATLAB engine",
			(LPCWSTR)"MATLAB_Interface.c", MB_OK);
		exit(-1);
	}

	char gNB_Name[BUFSIZ];
	double gNB_X;
	double gNB_Y;
	double gNB_Z;
	double gNB_height;
	double gNB_fq;
	double gNB_Tx_Pwr;
	double ue_height;
	
	//grid length
	char* temp = NULL;
	temp = getenv("NETSIM_SIM_AREA_X");
	if (temp)
	{
		if (atoi(temp))
			grid_length = atoi(temp);
		else
			grid_length = 0xFFFFFFFF;
	}
		
	//gNB_Count
	int ue_height_flag = 0;
	for (NETSIM_ID d = 0; d < NETWORK->nDeviceCount; d++)
	{
		for (NETSIM_ID i = 0; i < DEVICE(d + 1)->nNumOfInterface; i++)
		{
			if (!isLTE_NRInterface(d + 1, i + 1))
				continue;

			ptrLTENR_PROTODATA data = LTENR_PROTODATA_GET(d + 1, i + 1);
			if (!ue_height_flag && data && data->deviceType == LTENR_DEVICETYPE_UE)
			{
				ptrLTENR_UEPHY phy = LTENR_UEPHY_GET(d + 1, i + 1);
				ue_height = phy->propagationConfig->UE_height;
				ue_height_flag = 1;
			}

			if (!data || data->deviceType != LTENR_DEVICETYPE_GNB)
				continue;
			gNB_Count++;
		}
	}

	
	gNB_Id = (int*)calloc(gNB_Count, sizeof(*gNB_Id));
	//gNB_PLoss = (double**)calloc(gNB_Count, sizeof(*gNB_PLoss));

	int index = 0;

	//matrix dimention
	if (grid_length > step_length)
	{
		if (grid_length % step_length != 0)
			grid_length += step_length - (grid_length % step_length);
	}
	else
		grid_length = step_length;

	
	for (NETSIM_ID d = 0; d < NETWORK->nDeviceCount; d++)
	{
		for (NETSIM_ID i = 0; i < DEVICE(d + 1)->nNumOfInterface; i++)
		{
			if (!isLTE_NRInterface(d + 1, i + 1))
				continue;

			ptrLTENR_PROTODATA data = LTENR_PROTODATA_GET(d + 1, i + 1);

			if (!data || data->deviceType != LTENR_DEVICETYPE_GNB)
				continue;
			
			ptrLTENR_GNBPHY phy = LTENR_GNBPHY_GET(d + 1, i + 1);
			gNB_Id[index] = d + 1;			
			//gNB_PLoss[index] = (double*)calloc(dim, sizeof(*gNB_PLoss[index]));
			

			strcpy(gNB_Name, DEVICE_NAME(d + 1));
			gNB_X = DEVICE_POSITION(d + 1)->X;
			gNB_Y = DEVICE_POSITION(d + 1)->Y;
			gNB_Z = DEVICE_POSITION(d + 1)->Z;
			gNB_height = phy->propagationConfig->gNB_height;
			//fetching the frequency set for first carrier
			//MHz to Hz
			gNB_fq = ((phy->spectrumConfig->CA[0]->Fhigh_MHz +
				phy->spectrumConfig->CA[0]->Flow_MHZ) / 2) * 1000000;
			//dBm to Watts
			gNB_Tx_Pwr = pow(10, (phy->propagationConfig->txPower_dbm / 10)) / 1000; 
			
			sprintf(command, "tx%d=txsite(\"Name\",'%s',\
				\"CoordinateSystem\",'cartesian',\
				\"AntennaPosition\",[%f;%f;%f],\
				\"TransmitterPower\",%f,\
				\"TransmitterFrequency\",%f,\
				\"AntennaHeight\", %f)", gNB_Id[index], gNB_Name,
				gNB_X, gNB_Y, gNB_Z, gNB_Tx_Pwr,
				gNB_fq, gNB_height);
			engEvalString(ep, command);
			char filepath[BUFSIZ];
			sprintf(filepath, "%s\\%s_Pathloss.txt", pszIOPath, gNB_Name);
			//MATLAB_5G_Channel_Model(grid_length, step_length, ueheight, txsite, filepath)
			sprintf(command, "PathLoss=MATLAB_5G_Channel_Model(%d,%d,%f,tx%d,'%s')", grid_length,
				step_length, ue_height, gNB_Id[index],filepath);
			engEvalString(ep, command);

			index++;
		}
	}
	
	engEvalString(ep, "desktop");
	
	return 0;
}

double fn_netsim_matlab_finish()
{
	//Close the MATLAB Engine Process
	fprintf(stderr, "\nPress any key to close MATLAB...\n");
	_getch();
	engEvalString(ep, "exit");
	return 0;
}
