import MySQLdb
import io

class HostConfig:
    def __init__(self, host_id, minion, host, ip):
        self.host_id = host_id
        self.minion = minion
        self.host = host
        self.ip = ip
    def __init__(self):
        self.host_id = 0
        self.minion = ""
        self.host = ""
        self.ip = ""
    def __init__(self, data):
        self.host_id = data[0]
        self.minion = data[4]
        self.host = data[1]
        self.ip = data[3]
    def show(self):
        print self.host_id, self.minion, self.host, self.ip
    def isMe(self, ip):
        return self.ip == ip
    def setMinion(self, minion, host):
        self.minion = host[0:3] + minion

def readMarioDb():
    db = MySQLdb.connect('localhost', 'mario', 'chaojimali', 'mario');
    cursor = db.cursor()
    cursor.execute("SELECT * from mr_host")
    result = cursor.fetchall()
    hcs = []
    for data in result:
        # print data
        hcs.append(HostConfig(data))
    db.close()
    return hcs

def get_minion_(line):
    return line[1: len(line) - 2]

def get_host_ip_(line):
    pos = line.find("=");
    return line[pos + 1: len(line) -1]

def match_db_(hcs, minion, host, ip):
    for hc in hcs:
        if hc.isMe(ip):
            return hc
    return None

def make_sql_(hc):
    print "update mr_host set minion = '" + hc.minion + "' where host_id = ", hc.host_id, ';'
    #hc.show()

def readConfigFile(filepath, hcs):
    minion = ""
    host = ""F
    ip = ""
    found = 0
    fd = io.open(filepath, encoding="ascii")
    line = fd.readline().encode("ascii")
    while line:
        if (line[0] == '['):
            minion = get_minion_(line)
            if (minion != "true"):
                found = 1
            else:
                found = 0
            # print minion,
        if (line.find("host") == 0):
            host = get_host_ip_(line)
            found += 1
            # print host,
        if (line.find("ip") == 0):
            ip = get_host_ip_(line)
            found += 1
            # print ip,
        if (found == 3):
            ret = match_db_(hcs, minion, host, ip)
            found = 0
            if ret:
                ret.setMinion(minion, host)
                make_sql_(ret)
        line = fd.readline()
    fd.close()

# read config.ini to get minion-name, host name and ip address

filepath = "/work/sean/learning/lgraph/attr-1/config.ini"
hcs = readMarioDb()
readConfigFile(filepath, hcs)
