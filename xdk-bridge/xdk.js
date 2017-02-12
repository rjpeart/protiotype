const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const http = require('http');

var values = {};

postData = function(data) {

  var str_data = JSON.stringify(data);

  var post_options = {
    host: '192.168.0.8',
    port: '8000',
    path: '/environment',
    method: 'POST', 
    headers: {
      'Content-Type': 'text/json',
      'Content-Length': Buffer.byteLength(str_data)
    }
  } 

  var post_req = http.request(post_options, function(res) {
    res.setEncoding('utf8');
    res.on('data', function(chunk) {
      console.log('Response: ' + chunk);
    });
  });

  post_req.on('error', (err) => {
	console.log(err);
    });

  post_req.write(str_data);
  post_req.end();
}

server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  console.log(`server got: message from ${rinfo.address}:${rinfo.port}`);

  var len = msg.readUInt8(0);

  console.log(`length: ${len}`);

  var accel = {};
  accel["x"] = msg.readInt32LE(1);
  accel["y"] = msg.readInt32LE(5);
  accel["z"] = msg.readInt32LE(9);
  values["accel"] = accel;

  console.log(`accel x = ${accel.x}, y = ${accel.y}, z = ${accel.z}`);

  var gyro = {};
  gyro["x"] = msg.readInt32LE(13);
  gyro["y"] = msg.readInt32LE(17);
  gyro["z"] = msg.readInt32LE(21);
  values["gyro"] = gyro;

  console.log(`gyro x = ${gyro.x}, y = ${gyro.y}, z = ${gyro.z}`);

  var lux = msg.readUInt32LE(25);
  values["lux"] = lux;

  console.log(`lux = ${lux}`);

  var noise = msg.readUInt8(29);
  values["noise"] = noise;

  console.log(`noise = ${noise}`);

  var mag = {};
  mag["x"] = msg.readInt32LE(30);
  mag["y"] = msg.readInt32LE(34);
  mag["z"] = msg.readInt32LE(38);
  values["mag"] = mag;

  console.log(`mag x = ${mag.x}, y = ${mag.y}, z = ${mag.z}`);

  var res = msg.readInt16LE(42);
  values["resistance"] = res;

  console.log(`resistance = ${res}`);

  var pressure = msg.readUInt32LE(44) / 100;
  values["pressure"] = pressure;

  console.log(`pressure = ${pressure}`);

  var temperature = msg.readInt32LE(48) / 1000;
  values["temperature"] = temperature;

  console.log(`temperature = ${temperature}`);

  var humidity = msg.readUInt32LE(52);
  values["humidity"] = humidity;

  console.log(`humidity = ${humidity}`);

  console.log('values = ' + JSON.stringify(values));

  postData(values);
});

server.on('listening', () => {
  var address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(6666);

http.createServer(function (req, res) {
	res.writeHead(200, {'Content-Type': 'application/json'});
	res.end(JSON.stringify(values));
}).listen(9616);
