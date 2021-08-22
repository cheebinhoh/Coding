/* Copyright © 2019 - 2021, Chee Bin HOH. All rights reserved.
 *
 * This program is built for programming challenge, https://www.aiforsea.com/traffic-management
 *
 * This program provides a simple command-line interface to filter traffic demand dataset.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>


enum
{
    NUM_DEMAND_PER_NODE  = 500,
    MIN_IN_MININTERVAL   = 15,
    MININTERVALS_IN_DAY  = 4,
    HOURS_IN_DAY         = 24,
    DAYS_IN_YEAR         = 365,
    HOURS_IN_YEAR        = DAYS_IN_YEAR * HOURS_IN_DAY,
    MININTERVALS_IN_YEAR = HOURS_IN_YEAR * MININTERVALS_IN_DAY,
    HASH_MULTIPLIER      = 37,
    NUM_HASH_SIZE        = 5000
};


typedef struct demand Demand;

struct demand
{
    char   geohash6[7];
    int    day;
    int    hh;
    int    mm;
    double value;
};


typedef struct demandnode DemandNode;

struct demandnode
{
    DemandNode *next;
    DemandNode *prev;
    long        cnt;
    Demand     *d[1]; //varying size, allocated by malloc
};


typedef struct demandintime DemandInTime;

struct demandintime
{
    DemandNode * day[DAYS_IN_YEAR];
    DemandNode * hour[HOURS_IN_YEAR];
    DemandNode * mininterval[MININTERVALS_IN_YEAR];
};


typedef struct demandingeohash6 DemandInGeohash6;

struct demandingeohash6
{
    DemandNode       * d;
    DemandInGeohash6 * next;
    char               geohash6[7];
};


Demand * scanDemand(char * cptr, Demand * dptr);
int parseRange(char * s, int * from, int * to);


/* Start of DemandInTime API
 *
 * DemandInTime is ADT that allows us to store and organize demands by day and hour
 * and min interval.
 */

DemandInTime * newDemandInTime(void);

void deleteDemandInTime(DemandInTime * dit);

void processDemandInTime(DemandInTime * dit, Demand * dptr, long nrDemand);

void processDemandNodeInTime(DemandInTime * dit, DemandNode * list);

void printDebugDemandInTime(DemandInTime * dit);

/* End of DemandInTime API */


/* Start of DemandNode API
 */

DemandNode * newDemandNode(void);

void deleteDemandNode(DemandNode * list);

DemandNode * processDemandNode(DemandNode * list, Demand * dptr, long nrDemand);

void printDebugDemandNode(DemandNode * list);

/* End of DemandNode API */


/* Start of DemandInGeohash6 API
 *
 * DemandInGeohash6 is ADT that allows us to store and organize demands by geohash6 value.
 *
 * The ADT is a hash struture that each entry referring to a list of object DemandInGeohash6
 * that its geohash6 values are hashed into the same hash key. Each DemandInGeohash6 object stores
 * a list of demands of the same geohash6 and link to next DemandInGeohash6 that has different
 * geohash6 value but hashed into the same key in the high level DemandInGeohash6 hash structure.
 */

long getHashValueOfString(char * s);

DemandInGeohash6 * * newDemandInGeohash6(void);

void deleteDemandInGeohash6(DemandInGeohash6 * * digh6);

DemandInGeohash6 * insertGeohash6(DemandInGeohash6 * * digh6, char * geohash6, int createIfNotExist );

DemandInGeohash6 * insertDemandInGeohash6(DemandInGeohash6 * * digh6, Demand * d, int createIfNotExist);

void processDemandInGeohash6(DemandInGeohash6 * * digh6, Demand * d, long nrDemand, int createIfNotExist);

void printDebugDemandInGeohash6(DemandInGeohash6 * * digh6);

/* End of DemandInGeohash6 API */


/* global variables */
static char * baseProgramName = NULL;


int
main(int argc, char * argv[])
{
    int      day[DAYS_IN_YEAR] = { 0 };
    int      hourMinInterval[MININTERVALS_IN_DAY * HOURS_IN_DAY] = { 0 };
    char   * geohash6 = NULL;
    int      hourMinFrom;
    int      hourMinTo;
    int      dayFrom;
    int      dayTo;
    Demand * base  = NULL;
    Demand * dptr  = NULL;
    long     nrDemand = 0;
    int      i;
    int      ret;
    int      opt;
    char     buf[BUFSIZ];
    FILE *   file = stdin;


    // initialization

    baseProgramName = strrchr(argv[0], '/');
    if ( NULL == baseProgramName )
    {
        baseProgramName = argv[0];
    }
    else
    {
        baseProgramName++;
    }

    for ( i = 0; i < DAYS_IN_YEAR; i++ )
    {
        day[i] = 1;
    }

    for ( i = 0; i < MININTERVALS_IN_DAY * HOURS_IN_DAY; i++ )
    {
        hourMinInterval[i] = 1;
    }

    // end of initialization


    // program option and argument parsing

    while ( ( opt = getopt(argc, argv, "g:d:t:h") ) != -1 )
    {
        switch ( opt )
        {
            case 'h':
                printf("OVERVIEW: Trafic demand management and filtering tool\n");
                printf("\n" );
                printf("USAGE: %s [options] file ...\n", baseProgramName);
                printf("\n");
                printf("[options]\n");
                printf("    -gqp03tu,qp09fu                    Filter demands matching the geohash6 values qp03tu or qp09fu\n");
                printf("    -d1..3,5..6,9                      Filter demands matching day 1 to 3, 5 to 6 or 9\n");
                printf("    -t1000..1045,1315..1345            Filter demands matching time 10:00 to 10:45 or 13:15 to 13:45\n");
                printf("\n\n");
                printf("If file is not given, it is reading from standard input\n");
                printf("\n");
                exit(0);
                break;

            case 'g':
                geohash6 = optarg;
                break;

            case 't':
                for ( i = 0; i < MININTERVALS_IN_DAY * HOURS_IN_DAY; i++ )
                {
                    hourMinInterval[i] = 0;
                }

                ret = parseRange(optarg, &hourMinFrom, &hourMinTo);

                do
                {
                    if ( ret < 0 )
                    {
                        fprintf(stderr, "Invalid argument to -d%s\n", optarg);
                        exit(1);
                    }
                    else if ( ret == 0 )
                    {
                        break;
                    }
                    else
                    {
                        int hourFrom;
                        int hourTo;
                        int minFrom;
                        int minTo;
                        int minIntervalFrom;
                        int minIntervalTo;

                        if ( hourMinFrom > hourMinTo )
                        {
                            fprintf(stderr, "Invalid argument to -t, start range argument must be less than or equal to end range argument, example -t1000..1045\n");
                            exit(1);
                        }

                        hourFrom = hourMinFrom / 100;
                        hourTo   = hourMinTo / 100;
                        minFrom  = hourMinFrom % 100;
                        minTo    = hourMinTo % 100;

                        if ( hourFrom >= 24
                             || hourTo >= 24
                             || minFrom >= 60
                             || minTo >= 60 )
                        {
                            fprintf(stderr, "Invalid argument to -t, 1015 means hour 10 and min 15\n");
                            exit(1);
                        }

                        minIntervalFrom = minFrom / MIN_IN_MININTERVAL;
                        minIntervalTo = minTo / MIN_IN_MININTERVAL;

                        for ( i = hourFrom * MININTERVALS_IN_DAY + minIntervalFrom; i <= hourFrom * MININTERVALS_IN_DAY + minIntervalTo; i++ )
                        {
                            hourMinInterval[i] = 1;
                        }

                        ret = parseRange(NULL, &hourMinFrom, &hourMinTo);
                    }
                }
                while ( 1 );

                break;

            case 'd':
                for ( i = 0; i < DAYS_IN_YEAR; i++ )
                {
                    day[i] = 0;
                }

                ret = parseRange(optarg, &dayFrom, &dayTo);

                do
                {
                    if ( ret < 0 )
                    {
                        fprintf(stderr, "Invalid argument to -d%s\n", optarg);
                        exit(1);
                    }
                    else if ( ret == 0 )
                    {
                        break;
                    }
                    else
                    {
                        if ( dayFrom <= 0
                             || dayTo <= 0 )
                        {
                            fprintf(stderr, "argument to -d must be 1 and less than 366\n");
                            exit(1);
                        }

                        if ( dayFrom > dayTo )
                        {
                            fprintf(stderr, "start range argument must be less than or equal to end range argument, example -d1..3\n");
                            exit(1);
                        }

                        for ( i = dayFrom - 1; i < dayTo; i++ )
                        {
                            day[i] = 1;
                        }

                        ret = parseRange(NULL, &dayFrom, &dayTo);
                     }
                }
                while ( 1 );

                break;

            default:
                fprintf(stderr, "invalid option (%c) and argument\n", opt);
                exit(1);
        }
    }

    argc -= optind;
    argv += optind;

    if ( argc-- > 0 )
    {
        file = fopen(*argv++, "r");
        if ( file == NULL)
        {
            fprintf(stderr, "open file error: %s\n", *argv);
            exit(1);
        }
    }

    // end of program option and argument parsing

    // read data from standard input or files

    do
    {
        while ( fgets(buf, sizeof buf, file) != NULL )
        {
            if ( NULL == base )
            {
               base = malloc(NUM_DEMAND_PER_NODE  * ( sizeof ( base[0] ) ));
               dptr = base;
            }
            else if ( ( nrDemand % NUM_DEMAND_PER_NODE ) == 0 )
            {
                dptr = realloc(base, ( nrDemand + NUM_DEMAND_PER_NODE ) * sizeof ( base[0] ));
                if ( dptr  == NULL )
                {
                    free(base);
                }

                base = dptr;
            }

            if ( dptr == NULL )
            {
                fprintf(stderr, "no memory\n");
                exit(1);
            }

            if ( scanDemand(buf, &( dptr[nrDemand] )) )
            {
                nrDemand++;
            }
        } // while still got next line

        if ( argc-- > 0 )
        {
           file = fopen(*argv++, "r");
           if ( file == NULL)
           {
               fprintf(stderr, "open file error: %s\n", *argv);
               exit(1);
           }
        }
        else
        {
           break;
        }
    } while ( 1 );

    // end of read data from standard input or files

    // processing data into output
    {
        DemandInGeohash6 * * glist = NULL;
        int                  hasFilterGeohash6 = 0;


        hasFilterGeohash6 = ( NULL != geohash6 );

        glist = newDemandInGeohash6();

        if ( hasFilterGeohash6 )
        {
            char *tok = NULL;

            tok = strtok(geohash6, ",");
            while ( NULL != tok )
            {
                insertGeohash6(glist, tok, 1);

                tok = strtok(NULL, ",");
            }
        }

        for ( i = 0; i < nrDemand; i++ )
        {
            if ( day[dptr[i].day - 1]
                 && hourMinInterval[dptr[i].hh * MININTERVALS_IN_DAY + ( dptr[i].mm / MIN_IN_MININTERVAL ) ] )
            {
                insertDemandInGeohash6(glist, &( dptr[i] ), ! hasFilterGeohash6);
            }
        }

        printDebugDemandInGeohash6(glist);

        deleteDemandInGeohash6(glist);
    }

    // processing data into output

    return 0;
}


/* Start of DemandNode API */

DemandNode * newDemandNode(void)
{
    DemandNode * newNode;


    /* reduce by 1 because DemandNode already contains 1 element for the array
     */
    newNode = malloc(sizeof( DemandNode ) + ( ( NUM_DEMAND_PER_NODE - 1 ) * ( sizeof( Demand * ) ) ));
    if ( NULL == newNode )
    {
        fprintf(stderr, "failed to allocate memory for more DemandNode\n");
        exit(1);
    }

    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->cnt  = 0;

    return newNode;
}


void deleteDemandNode(DemandNode * item)
{
    DemandNode * tmp;


    while ( NULL != item )
    {
        tmp = item;
        item = item->next;

        free(tmp);
    }

    free(item);
}


DemandNode * processDemandNode(DemandNode * list, Demand * dptr, long nrDemand)
{
    long         i;
    DemandNode * newNode;


    for ( i = 0; i < nrDemand; i++ )
    {
        if ( NULL == list
             || list->cnt >= NUM_DEMAND_PER_NODE )
        {
            newNode = newDemandNode();

            newNode->next = list;
            if ( NULL != list )
            {
                list->prev = newNode;
            }

            list = newNode;
        }

        list->d[list->cnt++] = &( dptr[i] );
    }

    return list;
}


void printDebugDemandNode(DemandNode * item)
{
    int i;


    while ( NULL != item )
    {
        for ( i = 0; i < item->cnt; i++ )
        {
            printf("%s,%02d,%02d:%02d,%.18lf\n",
                   item->d[i]->geohash6,
                   item->d[i]->day,
                   item->d[i]->hh,
                   item->d[i]->mm,
                   item->d[i]->value);
        }

        item = item->next;
    }
}

/* End of DemandNode API */


/* Start of DemandInGeohash6 API */

long getHashValueOfString(char * s)
{
    long result;
    int  c;


    result = 0;
    while ( ( c = *s++ )  != '\0' )
    {
        result = result * HASH_MULTIPLIER + c;
    }

    return result % NUM_HASH_SIZE;
}


DemandInGeohash6 * * newDemandInGeohash6(void)
{
    DemandInGeohash6 * * digh6 = NULL;
    int                  i;


    digh6 = malloc(NUM_HASH_SIZE * sizeof( DemandInGeohash6 * ));
    if ( NULL == digh6 )
    {
        fprintf(stderr, "failed to allocte memory for more DemandInGeohash6 *\n");
        exit(1);
    }

    for ( i = 0; i < NUM_HASH_SIZE; i++ )
    {
        digh6[i] = NULL;
    }

    return digh6;
}


void deleteDemandInGeohash6(DemandInGeohash6 * * digh6)
{
    int                i;
    DemandInGeohash6 * hashItem;
    DemandInGeohash6 * next;


    for ( i = 0; i < NUM_HASH_SIZE; i++ )
    {
        for ( hashItem = digh6[i]; NULL != hashItem; hashItem = next )
        {
            next = hashItem->next;

            deleteDemandNode(hashItem->d);
            free(hashItem);
        }
    }

    free(digh6);
}


DemandInGeohash6 * insertGeohash6(DemandInGeohash6 * * digh6, char * geohash6, int createIfNotExist)
{
    long               hashkey;
    DemandInGeohash6 * hashItem = NULL;


    hashkey = getHashValueOfString(geohash6);

    assert(hashkey >= 0 && hashkey < NUM_HASH_SIZE);

    for ( hashItem = digh6[hashkey]; hashItem != NULL; hashItem = hashItem->next )
    {
        if ( strcmp(hashItem->geohash6, geohash6) == 0 )
        {
            break;
        }
    }

    if ( NULL == hashItem
         && createIfNotExist )
    {
        hashItem = malloc(sizeof( * hashItem ));
        if ( NULL == hashItem )
        {
            fprintf(stderr, "failed to allocate memory for more DemandInGeohash6\n");
            exit(1);
        }

        strncpy(hashItem->geohash6, geohash6, sizeof( hashItem->geohash6 ));
        hashItem->d = NULL;
        hashItem->next = digh6[hashkey];
        digh6[hashkey] = hashItem;
    }

    return hashItem;
}


DemandInGeohash6 * insertDemandInGeohash6(DemandInGeohash6 * * digh6, Demand * d, int createIfNotExist)
{
    DemandInGeohash6 * hashItem;


    hashItem = insertGeohash6(digh6, d->geohash6, createIfNotExist);

    if ( NULL != hashItem )
    {
        hashItem->d = processDemandNode(hashItem->d, d, 1);
    }

    return hashItem;
}


void processDemandInGeohash6(DemandInGeohash6 * * digh6, Demand * d, long nrDemand, int createIfNotExist)
{
    while ( nrDemand-- > 0 )
    {
        insertDemandInGeohash6(digh6, d++, createIfNotExist);
    }
}


void printDebugDemandInGeohash6(DemandInGeohash6 * * digh6)
{
    int                i;
    DemandInGeohash6 * hashItem;


    for ( i = 0; i < NUM_HASH_SIZE; i++ )
    {
        for ( hashItem = digh6[i]; NULL != hashItem; hashItem = hashItem->next )
        {
            DemandInTime * dit;
            DemandNode   * list;

            dit = newDemandInTime();

            for ( list = hashItem->d; NULL != list; list = list->next )
            {
                processDemandNodeInTime(dit, list);
            }

            printDebugDemandInTime(dit);

            deleteDemandInTime(dit);
        }
    }
}

/* End of DemandInGeohash6 API */


/* Start of DemandInTime API */

DemandInTime * newDemandInTime(void)
{
    DemandInTime * dit;
    int            i;


    dit = malloc(sizeof( *dit ));
    if ( NULL == dit )
    {
        fprintf(stderr, "failed to allocate more memory for DemandInTime\n");
        exit(1);
    }

    for ( i = 0; i < DAYS_IN_YEAR; i++ )
    {
        dit->day[i] = NULL;
    }

    for ( i = 0; i < HOURS_IN_YEAR; i++ )
    {
        dit->hour[i] = NULL;
    }

    for ( i = 0; i < MININTERVALS_IN_YEAR; i++ )
    {
        dit->mininterval[i] = NULL;
    }

    return dit;
}


void deleteDemandInTime(DemandInTime * dit)
{
    int i;


    for ( i = 0; i < DAYS_IN_YEAR; i++ )
    {
        deleteDemandNode(dit->day[i]);
    }

    for ( i = 0; i < HOURS_IN_YEAR; i++ )
    {
        deleteDemandNode(dit->hour[i]);
    }

    for ( i = 0; i < MININTERVALS_IN_YEAR; i++ )
    {
        deleteDemandNode(dit->mininterval[i]);
    }

    free(dit);
}


void printDebugDemandInTime(DemandInTime * dit)
{
    int dayIndex;
    int hourIndex;
    int minIntervalIndex;


    for ( dayIndex = 0; dayIndex < DAYS_IN_YEAR; dayIndex++ )
    {
        if ( NULL != dit->day[dayIndex] )
        {
            for ( hourIndex = dayIndex * HOURS_IN_DAY; hourIndex < dayIndex * HOURS_IN_DAY + HOURS_IN_DAY; hourIndex++ )
            {
                if ( NULL != dit->hour[hourIndex] )
                {
                    for ( minIntervalIndex = hourIndex * MININTERVALS_IN_DAY;
                          minIntervalIndex < hourIndex * MININTERVALS_IN_DAY + MININTERVALS_IN_DAY;
                          minIntervalIndex++ )
                    {
                         if ( NULL != dit->mininterval[minIntervalIndex] )
                         {
                             printDebugDemandNode(dit->mininterval[minIntervalIndex]);
                         }
                    }
                }
            }
        }
    }
}


void
processDemandInTime(DemandInTime * dit, Demand * dptr, long nrDemand)
{
    long i;
    int  dayIndex;
    int  hourIndex;
    int  minIntervalIndex;


    for ( i = 0; i < nrDemand; i++ )
    {
        // day is started at 1, hour at 0 and min interval at 0 from import data

        dayIndex  = dptr[i].day - 1;
        hourIndex = dayIndex * HOURS_IN_DAY + dptr[i].hh;
        minIntervalIndex = hourIndex * MININTERVALS_IN_DAY + ( dptr[i].mm / MIN_IN_MININTERVAL );

        assert(dayIndex < DAYS_IN_YEAR);
        assert(hourIndex < HOURS_IN_YEAR);
        assert(minIntervalIndex < MININTERVALS_IN_YEAR);

        dit->day[dayIndex] = processDemandNode(dit->day[dayIndex], &( dptr[i] ), 1);
        dit->hour[hourIndex] = processDemandNode(dit->hour[hourIndex], &( dptr[i] ), 1);
        dit->mininterval[minIntervalIndex] = processDemandNode(dit->mininterval[minIntervalIndex], &( dptr[i] ), 1);
    }
}


void processDemandNodeInTime(DemandInTime * dit, DemandNode * list)
{
    int i;


    for ( i = 0; i < list->cnt; i++ )
    {
        processDemandInTime(dit, list->d[i], 1);
    }
}

/* End of DemandInTime API */


Demand * scanDemand(char * cptr, Demand * dptr)
{
    int    c;
    int    inMm        = 0;
    int    index       = 0;
    int    geohash6Len = 0;
    Demand d           = { "\0", 0, 0, 0.0 };


    while ( ( c = *cptr++ ) != '\0'
            && c != '\n' )
    {
        if ( ',' == c )
        {
            if ( 0 == index )
            {
                d.geohash6[geohash6Len] = '\0';
            }
            else if ( 2 == index )
            {
                d.value = atof(cptr);
                break;
            }

            index++;
        }
        else
        {
            switch ( index )
            {
                case 0:
                    if ( geohash6Len < 6 )
                    {
                        d.geohash6[geohash6Len++] = c;
                    }
                    else
                    {
                        return NULL;
                    }

                    break;

                case 1:
                    if ( !isdigit(c) )
                    {
                        return NULL;
                    }

                    d.day = d.day * 10 + ( c - '0' );
                    break;

                case 2:
                    if ( c == ':' )
                    {
                        inMm = 1;
                    }
                    else
                    {
                        if ( !isdigit(c) )
                        {
                            return NULL;
                        }

                        if ( inMm )
                        {
                            d.mm = d.mm * 10 + ( c - '0' );
                        }
                        else
                        {
                            d.hh = d.hh * 10 + ( c - '0' );
                        }
                    }

                    break;
            } // switch
        } // else not yet reach ','
    } // while not end of line

    *dptr = d;

    return dptr;
}


/* the function parses the following pattern of string into both from and to values
 * in each call. Passing NULL as s when you want to continue parsing where the last
 * parse stops.
 *
 * 1,3,5
 * - 1st parse (not NULL) will reeturn 1 and 1
 * - 2nd parse (NULL) will return 3 and 3
 * - 3rd parse (NULL) will return 5 and 5
 *
 * 1..3,5,7..9
 * - 1st parse (not NULL) will return 1 and 3
 * - 2nd parse (NULL) will return 5 and 5
 * - 3rd parse (NULL) will return 7 and 9
 *
 * it returns 1 when parsing is success, 0 when EOF (no data for further parsing)
 * -1 when it fails to parse the string because of none-numeric character or because
 * instead of 1..5, we put 1.,5
 */
int parseRange(char * s, int * from, int * to)
{
    int           val;
    int           c;
    static char * ls  = NULL;


    if ( NULL != s )
    {
        ls = s;
    }

    assert(NULL != ls);

    if ( '\0' == *ls )
    {
        return 0;
    }

    val = 0;
    while ( ( c = *ls ) != '\0'
            && isdigit(c) )
    {
        val = val * 10 + ( c - '0' );
        ls++;
    }

    if ( '\0' != c
         && '.' != c
         && ',' != c )
    {
        return -1;
    }

    *from = val;
    *to   = val;

    if ( ',' == c )
    {
        ls++;
    }
    else if ( '.' == c )
    {
        ls++;

        if ( ( c = *( ls++ ) ) != '.' )
        {
            return -1;
        }

        val = 0;
        while ( ( c = *ls ) != '\0'
                && isdigit(c) )
        {
            val = val * 10 + ( c - '0' );
            ls++;
        }

        if ( '\0' != c
             && '.' != c
             && ',' != c )
        {
            return -1;
        }

        *to = val;

        if ( ',' == c )
        {
            ls++;
        }
    }

    return 1;
}
