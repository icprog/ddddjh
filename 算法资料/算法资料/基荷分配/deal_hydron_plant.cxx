/*--------------------------------------------------------------
**   FILE NAME: Get_single_attribute.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine receive sql string from socket class,link to
**       pointed database and query data qualifyed by sql string,
**       write data to a common string.
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**      Zhang zhigang     14/12/99
**
**--------------------------------------------------------------*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <locale.h>
#include "global_var.h"
#include "load_dispatch_db.h"
#include "var_def.h"

void get_hydron_plant()
{
	num_of_prior_hydron_plant = get_num_of_prior_hydron_plant();
	prior_hydron_plant = new hydron_para[num_of_prior_hydron_plant];
	get_prior_hydron_plant();
	set_prior_hydron_plant_flag();
	for(int i = 0; i < num_of_prior_hydron_plant; i++)
	{
		num_of_lower_hydron_plant = get_num_of_lower_hydron_plant(prior_hydron_plant[i].un_id);
		lower_hydron_plant = new hydron_para[num_of_lower_hydron_plant];
		get_lower_hydron_plant(prior_hydron_plant[i].un_id);
		prior_hydron_plant[i].pcount = get_pcount_of_prior_hydron_plant(prior_hydron_plant[i].un_id);
		for(int id = 0; id < num_of_lower_hydron_plant; id++)
		{
			lower_hydron_plant[id].pcount = (int)(prior_hydron_plant[i].pcount * prior_hydron_plant[i].consume_water / lower_hydron_plant[id].consume_water + 0.5);
			send_lower_hydron_plant_pcount(lower_hydron_plant[id].un_id,lower_hydron_plant[id].pcount);
		}
		delete[] lower_hydron_plant;
	}
	free_prior_hydron_plant();
}

