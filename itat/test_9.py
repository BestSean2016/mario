# -*- coding: UTF-8 -*-
# author: star
# created_at: 17-5-5 14:09
import libitat
import threading
import time


global brother


def mario_run(plid):
    global brother
    brother = libitat.new_mario(plid)
    print brother
    libitat.initial(brother, 1, 'test_transfers', 0, 0)
    libitat.run_mario(brother, 0, 12) #123 + plid)


def test_all_pipeline():
  global brother
  for plid in range(9, 10):
    print "RUN PIPELINE", plid
    mario_run(plid)
    print "mario_run has returned"
 
    while(1 != libitat.mario_is_done(brother)):
      time.sleep(1)

    print "killing mario ..."
    libitat.kill_mario(brother)
    print "killed", plid


test_all_pipeline()

