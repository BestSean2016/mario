from libitat import *
import threading
import time

brother = new_mario(12)
initial(brother, 1, 'bill_message', 0, 0)

class TestRun (threading.Thread):
    def run(self):
        for i in range(0, 10):
           print "run ", i
           time.sleep(1)



class TestPause(threading.Thread):
    def run(self):
      for i in range(0, 10):
          print "pause ", i
          time.sleep(1)


class runMario (threading.Thread):
    def run(self):
      print brother
      run_mario(brother, 0, 123)

class pauseMario(threading.Thread):
    def run(self):
      for i in range(0, 3):
        print "Pause ", i
        pause_mario(brother)
        print "aha?"
        time.sleep(10)
        print "Go on"
        go_on(brother)
        time.sleep(10)


def my_run_mario():
  print brother
  run_mario(brother, 0, 123)
  print "mario returned"


def my_pause_mario():
  for i in range(0, 10):
    print "Pause ", i
    pause_mario(brother)
    print "aha?"
    time.sleep(10)
    print "Go on"
    go_on(brother)
    time.sleep(10)


def test_threads():
  thread1 = TestRun()
  thread2 = TestPause()

  thread1.start()
  time.sleep(3)
  thread2.start()

  thread2.join()
  thread1.join()



def test_mario():
  thread1 = runMario()
  thread2 = pauseMario()

  thread1.start()
  print "sleep 30"
  time.sleep(30)
  print "new thread for pause"
  thread2.start()

  thread2.join()
  thread1.join()



def test_mario_2():
  thread1 = runMario()
  thread1.start()

  t = threading.Timer(30, pause_mario)
  t.start()

  thread1.join()


def test_mario_3():
    my_run_mario()
    time.sleep(30)
    my_pause_mario()


# test_threads()
# test_mario()
test_mario_3()

while (mario_is_done(brother) != 1):
    time.sleep(1)

print "killing mario ...."
kill_mario(brother)
print "mario was killed by luigi!"

