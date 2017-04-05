/* External definitions for single-server queueing system using simlib. */

#include "simlib.h"             /* Required for use of simlib.c. */

#define EVENT_ARRIVAL         1  /* Event type for arrival. */
#define EVENT_DEPARTURE       2  /* Event type for departure. */
#define EVENT_END_SIMULATION  3

#define INSPEC                  1
#define REPAIR                  2

#define SAMPST_INSPECT_DELAYS   1  /* sampst variable for delays in inspection queue. */
#define SAMPST_REPAIR_DELAYS    2  /* sampst variable for delays in repair queue. */

#define STREAM_INSPECT          1  /* Random-number stream for inspection service times. */
#define STREAM_REPAIR           2  /* Random-number stream for repair service times. */
#define STREAM_INTERARRIVAL     3  /* Random-number stream for interarrivals. */
#define STREAM_REINSPECT        4  /* Random-number stream for re-inspection. */


#define MAX_NUM_STATIONS        2  /* Maximum number of stations. */

typedef int bool;
enum { false, true };

/* Declare non-simlib global variables. */

int   num_stations, num_machines[MAX_NUM_STATIONS+1], 
      num_machines_busy[MAX_NUM_STATIONS+1], route[MAX_NUM_STATIONS+1], i, j, station;
float length_simulation,mean_interarrival, service_time_param[MAX_NUM_STATIONS + 1][2], 
      p_route[MAX_NUM_STATIONS + 1][3];
FILE  *infile, *outfile;

int cnt_arrival, cnt_departure;

/* Declare non-simlib functions. */

void init_model(void);
void arrive(bool);
void depart(void);
void report(void);


main()  /* Main function. */
{
    /* Open input and output files. */
    
    infile  = fopen("input/hw1.in", "r");
    outfile = fopen("output/hw1.out", "w");

    /* Read input parameters. */

    fscanf(infile, "%d %f %f", &num_stations,  &mean_interarrival, &length_simulation);
    
    /* service_time_param[station][0,1] -> uniform dist.*/
    for (i = 1; i <= num_stations; ++i)
        for (j = 0; j < 2; ++j)
            fscanf(infile, "%f", &service_time_param[i][j]);
    
    /* p_route[INSPEC][2] == 0.3, p_route[repair][2] == 0.9
     * Hence infile: 0.7, 1.0; 0.1, 1.0 */
    for (i = 1; i <= num_stations; ++i)
        for (j = 1; j <=2 ; ++j)
            fscanf(infile, "%f", &p_route[i][j]);
    /* Write report heading and input parameters. */
    fprintf(outfile, "Homework Problem 1 using simlib\n\n");
    fprintf(outfile, "=================== simulation parameters ==========================\n\n");
    fprintf(outfile, "Number of stations: %d \n\n", num_stations);
    fprintf(outfile, "Mean interarrival time: %.3f hours\n\n",
            mean_interarrival);
    fprintf(outfile, "Inspec time: uniform(%.3f, %.3f) (hours)\n\n", 
            service_time_param[INSPEC][0], service_time_param[INSPEC][1]);
    fprintf(outfile, "Repair time: uniform(%.3f, %.3f) (hours)\n\n", 
            service_time_param[REPAIR][0], service_time_param[REPAIR][1]);
    fprintf(outfile, "Probability of repair: (%.3f, %.3f)\n\n", 
            p_route[REPAIR][1], p_route[REPAIR][2]);
    fprintf(outfile, "Probability of re-inspection: (%.3f,%.3f)\n\n", 
            p_route[INSPEC][1], p_route[INSPEC][2]);
    fprintf(outfile, "Length of the simulation: %.3f hours\n\n", length_simulation);

    /* Initialize simlib */

    init_simlib();

    /* Set maxatr = max(maximum number of attributes per record, 4) */

    maxatr = 4;  /* NEVER SET maxatr TO BE SMALLER THAN 4. */

    /* Initialize the model. */

    init_model();

    /* Run the simulation while more delays are still needed. */

    do {

        /* Determine the next event. */

        timing();

        /* Invoke the appropriate event function. */

        switch (next_event_type) {
            case EVENT_ARRIVAL:
                arrive(true);
                ++cnt_arrival;
                break;
            case EVENT_DEPARTURE:
                depart();
                break;
            case EVENT_END_SIMULATION:
                report();
                break;
        }
    
        /* If the event just executed was not the end-simulation event (type
        EVENT_END_SIMULATION), continue simulating.  Otherwise, end the
        simulation. */
    
    } while (next_event_type != EVENT_END_SIMULATION);


    fclose(infile);
    fclose(outfile);

    return 0;
}


void init_model(void)  /* Initialization function. */
{

    /* 2 stations, with machines 1 and 2 respeectively*/
    num_stations = 2;
    num_machines[INSPEC] = 1;
    num_machines[REPAIR] = 2;
    route[INSPEC] = REPAIR;
    route[REPAIR] = INSPEC;
    cnt_arrival = 0;
    cnt_departure = 0;

    for (i = 1; i <= num_stations; ++i)
        num_machines_busy[i] = 0;
    /*The first arrival*/
    event_schedule(sim_time + expon(mean_interarrival, STREAM_INTERARRIVAL),
                   EVENT_ARRIVAL);
    event_schedule(length_simulation, EVENT_END_SIMULATION);
}


void arrive(bool new_job)  /* Arrival event function. */
{
    /* If this is a new arrival to the system, generate the time of the next
       arrival and determine the job type and task number of the arriving
       job. */

    /* new job always arriving at station 1 */

    if (new_job == true) {
     
        event_schedule(sim_time + expon(mean_interarrival, STREAM_INTERARRIVAL),
                        EVENT_ARRIVAL);
        station = INSPEC;
    
    }
    
    /* Check to see whether inspector is busy (i.e., list SERVER contains a
       record). */

    if (num_machines_busy[station] == num_machines[station]) {

        /* Server is busy, so store time of arrival of arriving customer at end
           of list LIST_QUEUE. */

        transfer[1] = sim_time;
        list_file(LAST, station);
    }

    else {

        /* Server is idle, so start service on arriving customer, who has a
           delay of zero.  (The following statement IS necessary here.) */

        sampst(0.0, station);

        /* Make server busy.*/

        ++num_machines_busy[station];
        timest((float) num_machines_busy[station], station);

        /* Schedule a departure (service completion). Note defining attributes of the event*/
        
        transfer[3] = station;
        event_schedule(sim_time + uniform(service_time_param[station][0],
                    service_time_param[station][1], station),EVENT_DEPARTURE);
        //event_schedule(sim_time + uniform(dist_inspect_a, dist_inspect_b, STREAM_INSPECT),
        //               EVENT_DEPARTURE);
    }
}


void depart(void)  /* Departure event function. */
{
    /* Check to see whether queue is empty. */
    station = transfer[3];

    if (list_size[station] == 0){

        /* The queue is empty, so make the server idle and leave the departure
           (service completion) event out of the event list. (It is currently
           not in the event list, having just been removed by timing before
           coming here.) */
        --num_machines_busy[station];
        timest((float) num_machines_busy[station], station);
    }

    else {

        /* The queue is nonempty, so remove the first customer from the queue, register delay */

        list_remove(FIRST, station);
        sampst(sim_time - transfer[1], station);
        
        /* Schedual the departure, with "station" for service time stream*/
        transfer[3] = station;
        
        event_schedule(sim_time + uniform(service_time_param[station][0],
                    service_time_param[station][1], station),EVENT_DEPARTURE);
        

    }

    
    /* Determine after the departure, leave the system or not */

    bool leave;
    int test;
    leave = true;
    /* Flip the coin to determine leave or not*/
    test = random_integer(p_route[station], station+2);
    if (test == 2 )
        leave = false;
    if (test == 1)
        ++cnt_departure;

    if (!leave) {
        station = route[station];
        arrive(false);
    }

}


void report(void)  /* Report generator function. */
{
    /* Get and write out estimates of desired measures of performance. */

    fprintf(outfile, "\n\n\n=================== simulation counters ==========================\n\n");
    fprintf(outfile, "\nTime simulation ended:%12.3f hours\n", sim_time);
    fprintf(outfile, "\nArrival Counter:%17d \n", cnt_arrival);
    fprintf(outfile, "\nDeparture Counter:%15d \n", cnt_departure);
    fprintf(outfile, "\nQueue Inspec:%20d \n", list_size[INSPEC]);
    fprintf(outfile, "\nQueue Repair:%20d \n\n", list_size[REPAIR]);
    
    fprintf(outfile, "\n\n\n=================== simulation statistics ==========================\n\n");
    
    fprintf(outfile,
           "\n Work      Average number      Average       Average delay");
    fprintf(outfile,
             "\nstation       in queue       utilization        in queue");
    for (j = 1; j <= num_stations; ++j)
        fprintf(outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest(j),
                timest(0.0, -j) / num_machines[j], sampst(0.0, -j));
    
    fprintf(outfile,"\n\n\n");
}

