const dgram = require('dgram');
const server = dgram.createSocket('udp4');

server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  console.log(`server got: message from ${rinfo.address}:${rinfo.port}`);

  var len = msg.readUInt8(0);

  console.log(`length: ${len}`);

  var i = 1;
  var accel_x = msg.readInt32LE(1);
  var accel_y = msg.readInt32LE(5);
  var accel_z = msg.readInt32LE(9);

  console.log(`accel x = ${accel_x}, y = ${accel_y}, z = ${accel_z}`);

  var gyro_x = msg.readInt32LE(13);
  var gyro_y = msg.readInt32LE(17);
  var gyro_z = msg.readInt32LE(21);

  console.log(`gyro x = ${gyro_x}, y = ${gyro_y}, z = ${gyro_z}`);

  var lux = msg.readUInt32LE(25);

  console.log(`lux = ${lux}`);

  var noise = msg.readUInt8(29);

  console.log(`noise = ${noise}`);

  var mag_x = msg.readInt32LE(30);
  var mag_y = msg.readInt32LE(34);
  var mag_z = msg.readInt32LE(38);

  console.log(`mag x = ${mag_x}, y = ${mag_y}, z = ${mag_z}`);

  var res = msg.readInt16LE(42);

  console.log(`resistance = ${res}`);

  var pressure = msg.readUInt32LE(44);

  console.log(`pressure = ${pressure}`);

  var temperature = msg.readInt32LE(48);

  console.log(`temperature = ${temperature}`);

  var humidity = msg.readUInt32LE(52);

  console.log(`humidity = ${humidity}`);
});

server.on('listening', () => {
  var address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(6666);
