// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const SerialPort = require('serialport')
const ByteLength = require('@serialport/parser-byte-length')
const tableify = require('tableify')

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
const port = new SerialPort('COM3',{baudRate: 9600}, true);

const parser = port.pipe(new ByteLength({length: 160}))

var rows = ["A","B","C","D","E"]

//parser.on('data', console.log)
parser.on('data', (data) => {
	var index = 0
    console.log(data)
    document.getElementById("incoming-data").innerHTML = data
	for( i=0;i<rows.length;i++){
		for( j=1;j<17;j++){
			eval("document.getElementById('V"+rows[i]+j+"').innerHTML = data["+index+"]")
			eval("document.getElementById('T"+rows[i]+j+"').innerHTML = data["+(index+79)+"]")
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