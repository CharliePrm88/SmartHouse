from flask import Flask, abort, request, flash, redirect, url_for, render_template
import requests,requests.auth, urllib, json, os, serial, time

app = Flask(__name__)
arduinoSerialData= serial.Serial('/dev/ttyACM0',19600)
@app.route('/', methods=['GET','POST'])
def homepage():
	if request.method == 'GET':
		return render_template("home.html")
		
#POST
	if request.method == 'POST':
		if request.form.get('Luce1'):
			print("Luce1")
			arduinoSerialData.write('2\n')
		if request.form.get('Luce2'):
			print("Luce2")
			arduinoSerialData.write('3\n')
		if request.form.get('Luce1') and request.form.get('Luce2'):
			arduinoSerialData.write('2\n')
			time.sleep(100)
			arduinoSerialData.write('3\n')
		return redirect(url_for('homepage'))
		
if __name__ == '__main__':
    app.run(debug=True, port=5000)
