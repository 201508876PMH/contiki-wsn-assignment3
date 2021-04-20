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

/* GLOBAL */
static int channelStrengthMean;
static process_event_t channelMeasuringEvent;

/*-----------------------------------------------------------*/
PROCESS(signalDataCompression, "data compression using discrete cosine transform (DCT)");
PROCESS(mesureBestChannel, "Process for measuring the best channel strength");
PROCESS(turnOnRadio, "Print output");
AUTOSTART_PROCESSES(&signalDataCompression);
/*-----------------------------------------------------------*/

PROCESS_THREAD(signalDataCompression, ev, data)
{
    PROCESS_BEGIN();
    process_start(&turnOnRadio, NULL);
    PROCESS_END();
}

PROCESS_THREAD(turnOnRadio, ev, data)
{
    static int currentChannel = 0;
    static int bestChannelStrength = -90;
    static int bestChannel = 11;
    PROCESS_BEGIN();
    cc2420_init(); // init radio
    channelMeasuringEvent = process_alloc_event();

    for (currentChannel = 11; currentChannel <= 26; currentChannel++)
    {
        process_start(&mesureBestChannel, &currentChannel);
        PROCESS_WAIT_EVENT_UNTIL(ev == channelMeasuringEvent);
        printf("Measured mean strength, from channel: %d, with value: %d \n", currentChannel, channelStrengthMean);
        if (channelStrengthMean > bestChannelStrength)
        {
            bestChannelStrength = channelStrengthMean;
            bestChannel = currentChannel;
        }
    }
    printf("Best measured channel strength: %d\n", bestChannelStrength);
    printf("Best channel by strength: %d\n", bestChannel);
    cc2420_set_channel(bestChannel);
    PROCESS_END();
}

PROCESS_THREAD(mesureBestChannel, ev, data)
{
    static struct timer timer;

    PROCESS_BEGIN();

    int counter = 0;
    int ChannelStrength = 0;
    cc2420_on(); // Turn on radio
    cc2420_set_channel(*((int*)data));

    int i;
    for(i = 0; i < 10; i++){
        ChannelStrength = ChannelStrength + cc2420_rssi();
        timer_set(&timer, CLOCK_SECOND * 1);
        timer_reset(&timer);
        counter++;
    }

    channelStrengthMean = ChannelStrength / counter;
    cc2420_off(); // Turn off radio

    process_post(&turnOnRadio, channelMeasuringEvent, NULL);
    PROCESS_END();
}