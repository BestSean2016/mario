/* Copyright 1988,1990,1993,1994 by Paul Vixie
 * All rights reserved
 */

/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1997,2000 by Internet Software Consortium, Inc.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Modified 2010/09/12 by Colin Dean, Durham University IT Service,
 * to add clustering support.
 */

#include "config.h"

#define	MAIN_PROGRAM

#include <errno.h>
#include <langinfo.h>
#include <locale.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

#ifdef WITH_INOTIFY
# include <sys/inotify.h>
#endif

#ifdef HAVE_SYS_FCNTL_H
# include <sys/fcntl.h>
#endif

#include "cronie_common.h"
#include "funcs.h"
#include "globals.h"
#include "pathnames.h"


#if defined WITH_INOTIFY
int inotify_enabled;
#else
# define inotify_enabled 0
#endif

enum timejump { negative, small, medium, large };

void find_jobs(int, cron_db *, int, int, long);
void run_reboot_jobs(cron_db * db);
int timeRunning, virtualTime, clockTime;
long GMToff;


#define MYLOG "MYLOG"



int cron_main(void) {
	cron_db database;
	database.head = NULL;
	database.tail = NULL;
	database.mtime = (time_t) 0;

	load_database(&database);

	run_reboot_jobs(&database);
    while (1) {
		check_orphans(&database);
		load_database(&database);

        find_jobs(virtualTime, &database, TRUE, TRUE, GMToff);

        job_runqueue();
        //find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);

        sleep(60);
    }

	return 0;
}

void run_reboot_jobs(cron_db * db) {
	user *u;
	entry *e;
	int reboot;
	pid_t pid = getpid();

	/* lock exist - skip reboot jobs */
	if (access(REBOOT_LOCK, F_OK) == 0) {
		log_it("CRON", pid, "INFO",
			"@reboot jobs will be run at computer's startup.", 0);
		return;
	}
	/* lock doesn't exist - create lock, run reboot jobs */
	if ((reboot = creat(REBOOT_LOCK, S_IRUSR & S_IWUSR)) < 0)
		log_it("CRON", pid, "INFO", "Can't create lock for reboot jobs.",
			errno);
	else
		close(reboot);

	for (u = db->head; u != NULL; u = u->next) {
		for (e = u->crontab; e != NULL; e = e->next) {
			if (e->flags & WHEN_REBOOT)
				job_add(e, u);
		}
	}
	(void) job_runqueue();
}

void find_jobs(int vtime, cron_db * db, int doWild, int doNonWild, long vGMToff) {
	char *orig_tz, *job_tz;
	struct tm *tm;
	int minute, hour, dom, month, dow;
	user *u;
	entry *e;

    log_it(MYLOG, 0, "L_INFO", "find_jobs", 0);

	/* The support for the job-specific timezones is not perfect. There will
	 * be jobs missed or run twice during the DST change in the job timezone.
	 * It is recommended not to schedule any jobs during the hour when
	 * the DST changes happen if job-specific timezones are used.
	 *
	 * Make 0-based values out of tm values so we can use them as indicies
	 */
#define maketime(tz1, tz2) do { \
	char *t = tz1; \
	if (t != NULL && *t != '\0') { \
		setenv("TZ", t, 1); \
		tm = localtime(&virtualGMTSecond); \
	} else { if ((tz2) != NULL) \
			setenv("TZ", (tz2), 1); \
		else \
			unsetenv("TZ"); \
		tm = gmtime(&virtualSecond); \
	} \
	minute = tm->tm_min -FIRST_MINUTE; \
	hour = tm->tm_hour -FIRST_HOUR; \
	dom = tm->tm_mday -FIRST_DOM; \
	month = tm->tm_mon +1 /* 0..11 -> 1..12 */ -FIRST_MONTH; \
	dow = tm->tm_wday -FIRST_DOW; \
	} while (0)

	orig_tz = getenv("TZ");

		/* the dom/dow situation is odd.  '* * 1,15 * Sun' will run on the
		 * first and fifteenth AND every Sunday;  '* * * * Sun' will run *only*
		 * on Sundays;  '* * 1,15 * *' will run *only* the 1st and 15th.  this
		 * is why we keep 'e->dow_star' and 'e->dom_star'.  yes, it's bizarre.
		 * like many bizarre things, it's the standard.
		 */
		for (u = db->head; u != NULL; u = u->next) {
		for (e = u->crontab; e != NULL; e = e->next) {
			time_t virtualSecond = (vtime - e->delay) * SECONDS_PER_MINUTE;
			time_t virtualGMTSecond = virtualSecond - vGMToff;
			job_tz = env_get("CRON_TZ", e->envp);
			maketime(job_tz, orig_tz);

			/* here we test whether time is NOW */
			if (bit_test(e->minute, minute) &&
				bit_test(e->hour, hour) &&
				bit_test(e->month, month) &&
				(((e->flags & DOM_STAR) || (e->flags & DOW_STAR))
					? (bit_test(e->dow, dow) && bit_test(e->dom, dom))
						: (bit_test(e->dow, dow) || bit_test(e->dom, dom))
				)
			) {
				if (job_tz != NULL && vGMToff != GMToff)
					/* do not try to run the jobs from different timezones
					 * during the DST switch of the default timezone.
					 */
					continue;

				if ((doNonWild &&
						!(e->flags & (MIN_STAR | HR_STAR))) ||
					(doWild && (e->flags & (MIN_STAR | HR_STAR))))
					job_add(e, u);	/*will add job, if it isn't in queue already for NOW. */
			}
		}
	}
	if (orig_tz != NULL)
		setenv("TZ", orig_tz, 1);
	else
		unsetenv("TZ");
}


static void set_time(int initialize) {
    struct tm tm;
    static int isdst;

    StartTime = time(NULL);

    /* We adjust the time to GMT so we can catch DST changes. */
    tm = *localtime(&StartTime);
    if (initialize || tm.tm_isdst != isdst) {
        isdst = tm.tm_isdst;
        GMToff = get_gmtoff(&StartTime, &tm);
        Debug(DSCH, ("[%ld] GMToff=%ld\n", (long) getpid(), (long) GMToff));
    }
    clockTime = (StartTime + GMToff) / (time_t) SECONDS_PER_MINUTE;
}

/*
 * Try to just hit the next minute.
 */
static void cron_sleep(int target, cron_db * db) {
    time_t t1, t2;
    int seconds_to_wait;

    t1 = time(NULL) + GMToff;
    seconds_to_wait = (int) (target * SECONDS_PER_MINUTE - t1) + 1;
    Debug(DSCH, ("[%ld] Target time=%ld, sec-to-wait=%d\n",
            (long) getpid(), (long) target * SECONDS_PER_MINUTE,
            seconds_to_wait));

    while (seconds_to_wait > 0 && seconds_to_wait < 65) {
        sleep((unsigned int) seconds_to_wait);

        t2 = time(NULL) + GMToff;
        seconds_to_wait -= (int) (t2 - t1);
        t1 = t2;
    }
}

int cron_main_main() {
    cron_db database;
    int fd;
    char *cs;
    pid_t pid = getpid();
    long oldGMToff;
    struct timeval tv;
    struct timezone tz;
    char buf[256];


    pid = getpid();

    /* obtain a random scaling factor for RANDOM_DELAY */
    if (gettimeofday(&tv, &tz) != 0)
        tv.tv_usec = 0;
    srandom(pid + tv.tv_usec);
    RandomScale = random() / (double)RAND_MAX;
    snprintf(buf, sizeof(buf), "RANDOM_DELAY will be scaled with factor %d%% if used.", (int)(RandomScale*100));
    log_it("CRON", pid, "INFO", buf, 0);

    acquire_daemonlock(0);
    database.head = NULL;
    database.tail = NULL;
    database.mtime = (time_t) 0;

    log_it("MYLOG", pid, "INFO", "to do load database", 0);
    load_database(&database);

    fd = -1;

    set_time(TRUE);

    log_it("MYLOG", pid, "INFO", "to do: run_reboot_jobs", 0);
    run_reboot_jobs(&database);
    timeRunning = virtualTime = clockTime;
    oldGMToff = GMToff;

    /*
     * Too many clocks, not enough time (Al. Einstein)
     * These clocks are in minutes since the epoch, adjusted for timezone.
     * virtualTime: is the time it *would* be if we woke up
     * promptly and nobody ever changed the clock. It is
     * monotonically increasing... unless a timejump happens.
     * At the top of the loop, all jobs for 'virtualTime' have run.
     * timeRunning: is the time we last awakened.
     * clockTime: is the time when set_time was last called.
     */
    while (1) {
        int timeDiff;
        enum timejump wakeupKind;

        /* ... wait for the time (in minutes) to change ... */
        do {
            cron_sleep(timeRunning + 1, &database);
            set_time(FALSE);
        } while (clockTime == timeRunning);

        timeRunning = clockTime;

        /*
         * Calculate how the current time differs from our virtual
         * clock.  Classify the change into one of 4 cases.
         */
        timeDiff = timeRunning - virtualTime;
        check_orphans(&database);
        load_database(&database);

        {
            char buf[4096];
            snprintf(buf, 4096, "the time is %d, %d, %d", timeDiff, timeRunning, virtualTime);
            log_it(MYLOG, pid, "L_DEBUG", buf, 0);
        }
        /* shortcut for the most common case */
        if (timeDiff == 1) {
            virtualTime = timeRunning;
            oldGMToff = GMToff;
            find_jobs(virtualTime, &database, TRUE, TRUE, oldGMToff);
        }
        else {
            if (timeDiff > (3 * MINUTE_COUNT) || timeDiff < -(3 * MINUTE_COUNT))
                wakeupKind = large;
            else if (timeDiff > 5)
                wakeupKind = medium;
            else if (timeDiff > 0)
                wakeupKind = small;
            else
                wakeupKind = negative;

            switch (wakeupKind) {
            case small:
                /*
                 * case 1: timeDiff is a small positive number
                 * (wokeup late) run jobs for each virtual
                 * minute until caught up.
                 */
                Debug(DSCH, ("[%ld], normal case %d minutes to go\n",
                        (long) pid, timeDiff));
                do {
                    log_it(MYLOG, pid, "INFO", "todo small run queue", 0);
                    if (job_runqueue())
                        sleep(10);
                    virtualTime++;
                    if (virtualTime >= timeRunning)
                        /* always run also the other timezone jobs in the last step */
                        oldGMToff = GMToff;
                    find_jobs(virtualTime, &database, TRUE, TRUE, oldGMToff);
                } while (virtualTime < timeRunning);
                break;

            case medium:
                /*
                 * case 2: timeDiff is a medium-sized positive
                 * number, for example because we went to DST
                 * run wildcard jobs once, then run any
                 * fixed-time jobs that would otherwise be
                 * skipped if we use up our minute (possible,
                 * if there are a lot of jobs to run) go
                 * around the loop again so that wildcard jobs
                 * have a chance to run, and we do our
                 * housekeeping.
                 */
                Debug(DSCH, ("[%ld], DST begins %d minutes to go\n",
                        (long) pid, timeDiff));
                /* run wildcard jobs for current minute */
                find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);

                /* run fixed-time jobs for each minute missed */
                do {
                    log_it(MYLOG, pid, "INFO", "todo medium run queue", 0);
                    if (job_runqueue())
                        sleep(10);
                    virtualTime++;
                    if (virtualTime >= timeRunning)
                        /* always run also the other timezone jobs in the last step */
                        oldGMToff = GMToff;
                    find_jobs(virtualTime, &database, FALSE, TRUE, oldGMToff);
                    set_time(FALSE);
                } while (virtualTime < timeRunning && clockTime == timeRunning);
                break;

            case negative:
                /*
                 * case 3: timeDiff is a small or medium-sized
                 * negative num, eg. because of DST ending.
                 * Just run the wildcard jobs. The fixed-time
                 * jobs probably have already run, and should
                 * not be repeated.  Virtual time does not
                 * change until we are caught up.
                 */
                Debug(DSCH, ("[%ld], DST ends %d minutes to go\n",
                        (long) pid, timeDiff));
                find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);
                break;
            default:
                /*
                 * other: time has changed a *lot*,
                 * jump virtual time, and run everything
                 */
                Debug(DSCH, ("[%ld], clock jumped\n", (long) pid));
                virtualTime = timeRunning;
                oldGMToff = GMToff;
                find_jobs(timeRunning, &database, TRUE, TRUE, GMToff);
            }
        }

        /* Jobs to be run (if any) are loaded; clear the queue. */
        log_it(MYLOG, pid, "L_INFO", "to do default job runqueue", 0);
        job_runqueue();

        // handle_signals(&database);
    }

    return 0;
}
