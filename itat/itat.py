from libitat import *

mario = long(new_mario(8))
test_setup(mario, 1, 1, -1, -1, -1, -1, -1, -1, 1000)
#initial(mario, 1, "bill_message", 0, 0)
initial(mario, 1, "transfers", 0, 0)
run_mario(mario, 0)
