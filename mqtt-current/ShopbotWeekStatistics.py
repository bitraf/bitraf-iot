import datetime
import matplotlib.pyplot as plt
import urllib.request, json
import numpy as np

def getUsage(fra, til):
    f = int(fra.strftime("%s"))
    t = int(til.strftime("%s"))
    day = fra.strftime("%A")
    url = "http://mqtt.bitraf.no:8080/telemetry/shopbot_current_draw?start=%d000&end=%d000" % (f, t)
    #print(url)

    with urllib.request.urlopen(url) as url:
        data = json.loads(url.read().decode())
    minutesOn = 1
    minutesRunning = 1
    totalMinutes = 1
    for d in data:
        totalMinutes += 1
        if int(d['value']) > 10:
            minutesOn += 1
        if int(d['value']) > 15:
            minutesRunning += 1
    idle = totalMinutes - minutesOn;

    idle = totalMinutes - minutesOn
    minutesOn -= minutesRunning
    return minutesOn / 60, minutesRunning / 60,day;

def getWeeklyUsage():
    result = []
    til = datetime.date.today() + datetime.timedelta(days=1)
    fra = til;
    for x in range(0,7):
        til = fra
        fra = til - datetime.timedelta(days=1)
        usage = getUsage(fra,til)
        #print ("fra ", fra, "til ",  til)
        result.append(usage)
    return result



#print (getWeeklyUsage())

N = 0
shopbotOn = []
ShopbotSpinning = []
Labels = [];


for usage in reversed(getWeeklyUsage()):
    shopbotOn.append(usage[0])
    ShopbotSpinning.append(usage[1])
    Labels.append(usage[2])
    N+=1



ind = np.arange(N)    # the x locations for the groups
width = 0.35       # the width of the bars: can also be len(x) sequence

p1 = plt.bar(ind, shopbotOn, width)
p2 = plt.bar(ind, ShopbotSpinning, width,
             bottom=shopbotOn)

plt.ylabel('Hours')
plt.title('ShopBot usage')
plt.xticks(ind, Labels)
plt.yticks(np.arange(0, 20, 5))
plt.legend((p1[0], p2[0]), ('On', 'Spinning'))

plt.savefig('Weekly.png')
