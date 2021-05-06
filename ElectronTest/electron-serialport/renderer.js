// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const SerialPort = require('serialport')
const ByteLength = require('@serialport/parser-byte-length')
const tableify = require('tableify')
const BUFFER_LENGTH_FROM_TEENSY = 2;//269

async function listSerialPorts() {
  await SerialPort.list().then((ports, err) => {
    if(err) {
      document.getElementById('error').textContent = err.message
      return
    } else {
      document.getElementById('error').textContent = ''
    }
    console.log('ports', ports);

    if (ports.length === 0) {
      document.getElementById('error').textContent = 'No ports discovered'
    }

    tableHTML = tableify(ports)
    document.getElementById('ports').innerHTML = tableHTML
  })
}


//const parser = new Readline()
//var port = new serialport('COM3',{baudRate: 9600}, true);
//port.pipe(parser)
const port = new SerialPort('COM3',{baudRate: 9600});

const parser = port.pipe(new ByteLength({length: 280}))

var rows = ["A","B","C","D","E"]

// parser.on('data', console.log)
parser.on('data', (data) => {
	var index = 0
    console.log(data)
	let voltagesA  = [0]
	let tempA = [0]
	let voltagesB  = [0]
	let tempB = [0]
	let voltagesC  = [0]
	let tempC = [0]
	let voltagesD  = [0]
	let tempD = [0]
	let voltagesE  = [0]
	let tempE = [0]
	voltagesA[0] = data[0] | (data[1]<<8);
	tempA[0] = data[32] | (data[33]<<8);
	voltagesB[0] = data[49] | (data[50]<<8);
	tempB[0] = data[81] | (data[82]<<8);
	voltagesC[0] = data[98] | (data[99]<<8);
	tempC[0] = data[130] | (data[131]<<8);
	voltagesC[0] = data[147] | (data[148]<<8);
	tempC[0] = data[179] | (data[180]<<8);
	voltagesC[0] = data[196] | (data[197]<<8);
	tempC[0] = data[228] | (data[229]<<8);

	// voltagesA[0] = voltagesA[0]/10000;
	// var i
	// for(i=0; i<data.length;i++){
		// console.log("!"+ i +": " + data[i]);
	// }
	for (i = 2; i < 32; i+=2) {
		voltagesA.push(data[i] | (data[i+1])<<8);
	}
	for (i = 2; i < 16; i+=2) {
		tempA.push(data[i+32] | (data[32+i+1])<<8);
	}
	for (i = 2; i < 32; i+=2) {
		voltagesB.push(data[i+49] | (data[49+i+1])<<8);
	}
	for (i = 2; i < 16; i+=2) {
		tempB.push(data[i+81] | (data[81+i+1])<<8);
	}
	for (i = 2; i < 32; i+=2) {
		voltagesC.push(data[i+98] | (data[98+i+1])<<8);
	}
	for (i = 2; i < 16; i+=2) {
		tempC.push(data[i+130] | (data[130+i+1])<<8);
	}
	for (i = 2; i < 32; i+=2) {
		voltagesD.push(data[i+147] | (data[147+i+1])<<8);
	}
	for (i = 2; i < 16; i+=2) {
		tempD.push(data[i+179] | (data[179+i+1])<<8);
	}
	for (i = 2; i < 32; i+=2) {
		voltagesE.push(data[i+196] | (data[196+i+1])<<8);
	}
	for (i = 2; i < 16; i+=2) {
		tempE.push(data[i+228] | (data[228+i+1])<<8);
	}
	// console.log(voltagesB)
	// let voltagesB  = [0]
	// voltagesB[0] = data[49] | (data[50]<<8);
	// var i
	// for (i = 2; i < 32; i+=2) {
		// voltagesB.push(data[49+i] |
		// (data[49+i+1])<<8);
	// }
	// let voltagesC  = [0]
	// let voltagesD  = [0]
	// let voltagesE  = [0]
    // document.getElementById("incoming-data").innerHTML = data
	// for( i=0;i<rows.length;i++){
	for( i=0;i<5;i++){

		for( j=1;j<17;j++){
			// eval("document.getElementById('V"+rows[i]+j+"').innerHTML = voltages"+rows[i]+"["+index+"]")
			eval("voltages"+rows[i]+"[j-1]  =voltages"+rows[i]+"[j-1]/10000;")
			eval("document.getElementById('V"+rows[i]+j+"').innerHTML = voltages"+rows[i]+"["+(j-1)+"]");
			eval("document.getElementById('T"+rows[i]+j+"').innerHTML = temp"+rows[i]+"["+Math.floor((j-1)/2)+"]")
			index++;
		}
	}
})

// Set a timeout that will check for new serialPorts every 2 seconds.
// This timeout reschedules itself.
setTimeout(function listPorts() {
  listSerialPorts();
  setTimeout(listPorts, 2000);
}, 2000);