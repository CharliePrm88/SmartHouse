from flask import Flask, abort, request, flash, redirect, url_for, render_template
import requests,requests.auth, urllib, json, os, serial, time

app = Flask(__name__)
arduinoSerialData= serial.Serial('/dev/ttyACM0',19600)

@app.route('/', methods=['GET','POST'])
def homepage():
	if request.method == 'GET':
		arduinoSerialData.write('\n')
		time.sleep(0.1)
		temperatura=(arduinoSerialData.read(8))[:4]
		return render_template("home.html",temperatura=temperatura)
		
#POST
	if request.method == 'POST':
		if request.form.get('Luce1') and request.form.get('Luce2'):
			arduinoSerialData.write('2')
			time.sleep(0.1)
			arduinoSerialData.write('3')
		elif request.form.get('Luce1'):
			arduinoSerialData.write('2')
		elif request.form.get('Luce2'):
			arduinoSerialData.write('3')
		#temperatura=(arduinoSerialData.read(8))[:4]	
		return redirect(url_for('homepage'))
		
if __name__ == '__main__':
    app.run(debug=True, port=5000)
