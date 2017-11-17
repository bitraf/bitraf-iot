import matplotlib.pyplot as plt
import urllib.request, json, time

timeTo = int(time.time())
timeFrom = timeTo - (60*60*24)


url = "http://mqtt.bitraf.no:8080/telemetry/shopbot_current_draw?start=%d000&end=%d000" % (timeFrom,timeTo)
print(url)

with urllib.request.urlopen(url) as url:
    data = json.loads(url.read().decode())
minutesOn = 0
minutesRunning = 0
totalMinutes = 0
for d in data:
	totalMinutes +=1
	if int(d['value']) > 10:
		minutesOn +=1
	if int(d['value']) > 15:
		minutesRunning +=1
idle = totalMinutes - minutesOn;

idle= totalMinutes-minutesOn
minutesOn-=minutesRunning


print ('Minutes idle:\t\t ', idle)
print ('Minutes on:\t\t ', minutesOn)
print ('Minutes Running:\t ', minutesRunning)
print ('Total minutes:\t\t ', totalMinutes)


labels = 'Off', 'On', 'Spinning'
sizes = [idle, minutesOn, minutesRunning]
colors = ['yellowgreen', 'gold', 'lightskyblue']
explode = (0, 0, 0.1)
plt.pie(sizes, explode=explode, labels=labels, colors=colors,
        autopct='%1.1f%%', shadow=True, startangle=90)
plt.axis('equal')
plt.title('Shopbot last 24 hours')
plt.savefig('24Hours.png')
