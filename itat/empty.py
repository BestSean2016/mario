import libitat
mario = libitat.Mario(0)
mario.initial(0, "bill_message")
STATE_TYPE_OK = 1
mario.test_setup(10, 2, STATE_TYPE_OK, STATE_TYPE_OK, -1, -1, -1, -1, -1, -1, 1000)
mario.run(0)

print 1 + 1
