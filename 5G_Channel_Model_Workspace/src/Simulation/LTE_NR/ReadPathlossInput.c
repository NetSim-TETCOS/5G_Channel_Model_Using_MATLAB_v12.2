#include "main.h"
#include "ReadPathlossInput.h"
#include "LTE_NR.h"
#include "LTENR_PHY.h"
#include "NetSim_utility.h"

static void add_pathloss_info(NETSIM_ID d,
	double x,
	double y,
	double pathloss)
{

	if (!pathlossInfo)
	{
		pathlossInfo = calloc(NETWORK->nDeviceCount, sizeof * pathlossInfo);
		lastInfo = calloc(NETWORK->nDeviceCount, sizeof * lastInfo);
	}
	ptrINFO info = pathlossInfo[d - 1];

	ptrINFO t = calloc(1, sizeof * t);
	t->x = x;
	t->y = y;
	t->pathloss = pathloss;
	if (info)
	{
		lastInfo[d - 1]->next = t;
		lastInfo[d - 1] = t;
	}
	else
	{
		pathlossInfo[d - 1] = t;
		lastInfo[d - 1] = t;
	}

}

/** This function is to free the file pointers */
int FileBasedpathlossPointersFree()
{
	NETSIM_ID d;
	if (pathlossInfo)
	{
		for (d = 0; d < NETWORK->nDeviceCount; d++)
		{
			ptrINFO i = pathlossInfo[d];
			while (i)
			{
				pathlossInfo[d] = i->next;
				free(i);
				i = pathlossInfo[d];
			}
		}
		free(pathlossInfo);
	}
	return 0;
}

/** This function is to open the file, and identify the pathloss for a receiver  */
int FileBasedpathlossReadingFile()
{
	int lineno = 0;
	double x, y, pathloss;
	FILE* fp;
	char str[BUFSIZ];

	for (NETSIM_ID d = 0; d < NETWORK->nDeviceCount; d++)
	{
		for (NETSIM_ID i = 0; i < DEVICE(d + 1)->nNumOfInterface; i++)
		{
			if (!isLTE_NRInterface(d + 1, i + 1))
				continue;

			ptrLTENR_PROTODATA prdata = LTENR_PROTODATA_GET(d + 1, i + 1);

			if (!prdata || prdata->deviceType != LTENR_DEVICETYPE_GNB)
				continue;

			sprintf(str, "%s\\%s_%s", pszIOPath, DEVICE_NAME(d + 1), "Pathloss.txt");
			fp = fopen(str, "r");
			if (!fp)
			{
				fnSystemError("Unable to open %s file.\n", str);
				perror(str);
				return -1;
			}

			char data[BUFSIZ];
			while (fgets(data, BUFSIZ, fp))
			{
				lineno++;
				lskip(data);
				if (*data == '#' ||
					*data == 0)
					continue;

				sscanf(data, "%lf,%lf,%lf",
					&x, &y, &pathloss);
				
				add_pathloss_info(d + 1, x, y, pathloss);
			}
			fclose(fp);

			
		}
	}

	return 0;
}

double fn_netsim_get_pathloss_from_file(int gnbid, int ueid)
{

	ptrINFO info = calloc(1, sizeof * info);
	info = pathlossInfo[gnbid - 1];
	ptrINFO t = calloc(1, sizeof * t);
	double distance = 0.0, mindistance = INFINITY, loss = 0.0;
	double ue_x = DEVICE_POSITION(ueid)->X;
	double ue_y = DEVICE_POSITION(ueid)->Y;
	while (info)
	{
		t = info;
		info = info->next;

		distance = ((t->x - ue_x) * (t->x - ue_x)) + ((t->y - ue_y) * (t->y - ue_y));
		distance = sqrt(distance);
		if (distance < mindistance)
		{
			mindistance = distance;
			loss = t->pathloss;
		}
	}
	
	return loss;
}
