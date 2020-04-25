<h1>Smarthouse in AVR Arduino</h1>
<h2>Progetto sistemi operativi 2019/2020</h2>
<h3>Requisiti<h3><p>
1x Raspberry pi >=2 con Raspbian buster</br>
1x Arduino Mega 2560</br>
2x Led</br>
2x Resistenze da 220 ohm</br>
Inoltre:</p>
<code>sudo apt-get install arduino arduino-mk python-flask</code>

<img src="static/Arduino.jpg" title="Arduino Mega 2560">

<h3>ISTRUZIONI</h3>
<ol>
<li>Montare i dispositivi come in foto</li>
<li>Collegare Arduino a Raspberry Pi via porta USB</li>
<li>Clonare la repository in Raspberry pi</li>
<li>make</li>
<li>make main.hex</li>
<li>Eseguire su terminale:</br>
<code>python server.py</code>
</li>
<li>Collegarsi all'indirizzo indicato sul terminale con un browser per eseguire i comandi</li>
</ol>
