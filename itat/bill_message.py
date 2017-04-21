class Message:
    def __init__(self):
        mine = "this is my object"

    def accept(self, pl_id, pl_ex_id, node_id, status, check_status, code, strout, strerr):
        print pl_id, pl_ex_id, node_id, status, check_status, code, strout, strerr



def test():
    m = Message()
    m.accept(1, 2, 3, 4, 5, 0, "every thing is ok", "here is no error")


if __name__ == '__main__':
    test()
