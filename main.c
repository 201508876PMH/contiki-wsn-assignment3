#include <stdio.h>
#include "contiki.h"
#include "cc2420.h"

/**
 * @ Author: Peter Marcus Hoveling
 * @ Date: 15/04/2021
*/

/* GLOBAL */
static int channelStrengthMean;
static process_event_t channelMeasuringEvent;

/*-----------------------------------------------------------*/
PROCESS(Initial, "Initial process");
PROCESS(mainProcess, "Main process");
PROCESS(measureBestChannel, "Process for measuring the best channel strength");
AUTOSTART_PROCESSES(&mainProcess);
/*-----------------------------------------------------------*/

PROCESS_THREAD(Initial, ev, data)
{
    PROCESS_BEGIN();
    process_start(&mainProcess, NULL);
    PROCESS_END();
}

/**
 * Process that inits radio and loops through channels 11 to 26, where it compares
 * the measured channel strengths by the previous best...finally finding the best 
 * channel to choose.
 * ---
 * Arguments: mainProcess, ev, data
 * Returns: none
*/
PROCESS_THREAD(mainProcess, ev, data)
{
    static int currentChannel = 0;
    static int bestChannelStrength = -90;
    static int bestChannel = 11;
    static struct timer timer;
    PROCESS_BEGIN();
    cc2420_init(); // init radio
    channelMeasuringEvent = process_alloc_event();

    timer_set(&timer, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(timer_expired(&timer));    // Delay, since mote login takes time.
    printf("Measuring strongest channel...\n");
    timer_reset(&timer);

    for (currentChannel = 11; currentChannel <= 26; currentChannel++)
    {
        process_start(&measureBestChannel, &currentChannel);
        PROCESS_WAIT_EVENT_UNTIL(ev == channelMeasuringEvent);
        printf("Measured mean strength, from channel: %d, with value: %d\n", currentChannel, channelStrengthMean);
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

/**
* Process for measuring the best channel, by sampling the RSSI value 10 times, and taking the means
* ---
* Arguments: measureBestChannel, ev, data (currentChannel)
* Returns: None
*/
PROCESS_THREAD(measureBestChannel, ev, data)
{
    static struct timer timer;

    PROCESS_BEGIN();

    int counter = 0;
    int ChannelStrength = 0;
    cc2420_on(); // Turn on radio
    cc2420_set_channel(*((int *)data));

    int i;
    for (i = 0; i < 10; i++)
    {
        ChannelStrength = ChannelStrength + cc2420_rssi();
        timer_set(&timer, CLOCK_SECOND * 1);
        timer_reset(&timer);
        counter++;
    }

    channelStrengthMean = ChannelStrength / counter;
    cc2420_off(); // Turn off radio

    process_post(&mainProcess, channelMeasuringEvent, NULL);
    PROCESS_END();
}