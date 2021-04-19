#include <stdio.h>
#include <math.h>
#include "contiki.h"
#include "cc2420.h"

/**
 * @ THIS PROGRAM HAS ONLY BEEN TESTED IN A SIMULATED ENVIROMENT (COOJA)
 * 
 * @ Author: Peter Marcus Hoveling
 * @ Date: 15/04/2021
*/

/* PROTOTYPING */ 

/* GLOBAL */


/*-----------------------------------------------------------*/
PROCESS(signalDataCompression, "data compression using discrete cosine transform (DCT)");
PROCESS(turnOnRadio, "Print output");
AUTOSTART_PROCESSES(&signalDataCompression);
/*-----------------------------------------------------------*/

PROCESS_THREAD(signalDataCompression, ev, data)
{
    PROCESS_BEGIN();
    process_start(&turnOnRadio, NULL); //<--- Change value here (256/512)
    PROCESS_END();
}


PROCESS_THREAD(turnOnRadio, ev, data)
{
    PROCESS_BEGIN();
    cc2420_init();
    cc2420_on();            // Turn on radio


    int bestChannelStrength = -90;
    while(1){
        int fetchedChannelStrength = cc2420_rssi();
        // int P = fetchedChannelStrength + RSSI_OFFSET;
        if(fetchedChannelStrength < bestChannelStrength){
            bestChannelStrength = fetchedChannelStrength;
        }
        printf("Best channel strength: %d\n", bestChannelStrength);
    }
    u_int8_t i;

    for(i = 11; i < 26; i++){
        int fetchedChannelStrength = cc2420_rssi();
        // int P = fetchedChannelStrength + RSSI_OFFSET;
        if(fetchedChannelStrength < bestChannelStrength){
            bestChannelStrength = fetchedChannelStrength;
        }
        printf("Best channel strength: %d\n", bestChannelStrength);
    }
    
    cc2420_set_channel(bestChannelStrength);
    int currentRSSI = cc2420_rssi();

    /* convert the RSSI register value to the RSSI value in dBm */
    int P = currentRSSI + RSSI_OFFSET;
    printf("Current RSSI: %d \n", currentRSSI);
    printf("RSSI value in dBm: %d \n", P);

    PROCESS_END();
}
