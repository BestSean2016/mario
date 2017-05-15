# -*- coding: UTF-8 -*-
# author: star
# created_at: 17-5-5 14:09
import libitat
import threading
import time

brother = libitat.new_mario(12)


def mario_run():
    print brother
    libitat.initial(brother, 1, 'test_transfers', 0, 0)
    libitat.run_mario(brother, 0, 1)


def mario_pause():
  for i in range(0, 15):
    print "pause ", i
    libitat.pause_mario(brother)
    time.sleep(10)
    print "go on ", i
    libitat.go_on(brother)
    time.sleep(10)

# mario_thread = threading.Thread(target=mario_run)
# mario_thread.start()

#threading.Timer(30, mario_pause())

mario_run()
time.sleep(30)
mario_pause()

while(1 != libitat.mario_is_done(brother)):
  time.sleep(1)

libitat.kill_mario(brother)
print "killed"
