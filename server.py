from flask import Flask, abort, request, flash, redirect, url_for, render_template
import requests,requests.auth, urllib, json, os, serial, time

app = Flask(__name__)
arduinoSerialData= serial.Serial('/dev/ttyACM0',19600)

def calcTemp(voltaggio):
	temperatura=voltaggio[:3]
	temperatura=float(temperatura)
	temperatura=(temperatura/1024)*5
	temperatura=(temperatura-0.5)*100
	return round(temperatura,1)

@app.route('/', methods=['GET','POST'])
def homepage():
	if request.method == 'GET':
		arduinoSerialData.write('\n')
		voltaggio=arduinoSerialData.readline()
		temperatura=calcTemp(voltaggio)
		return render_template("home.html",temperatura=temperatura)
		
#POST
	if request.method == 'POST':
		if request.form.get('Luce1') and request.form.get('Luce2'):
			arduinoSerialData.write('2\n')
			time.sleep(0.1)
			arduinoSerialData.write('3\n')
			voltaggio=arduinoSerialData.readline();
		elif request.form.get('Luce1'):
			arduinoSerialData.write('2\n')
			voltaggio=arduinoSerialData.readline();
		elif request.form.get('Luce2'):
			arduinoSerialData.write('3\n')
			voltaggio=arduinoSerialData.readline();
		temperatura=calcTemp(voltaggio)		
		return redirect(url_for('homepage', temperatura=temperatura))
		
if __name__ == '__main__':
    app.run(debug=True, port=5000)
